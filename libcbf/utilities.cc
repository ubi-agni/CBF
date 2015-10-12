/*
    This file is part of CBF.

    CBF is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    CBF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CBF.  If not, see <http://www.gnu.org/licenses/>.


    Copyright 2009, 2010 Florian Paul Schmidt, Viktor Richter
*/

#include <vector>

#include <cbf/utilities.h>
#include <cbf/debug_macros.h>
#include <cbf/exceptions.h>
#include <cbf/xml_object_factory.h>
#include <cbf/foreign_object.h>

#ifdef CBF_HAVE_KDL
	#include <kdl/jacobian.hpp>
	#include <kdl/tree.hpp>
	#include <kdl/chain.hpp>
	#include <kdl/segment.hpp>
	#include <kdl/joint.hpp>
#endif

#ifdef CBF_HAVE_EIGEN
	#include <Eigen/Core>
	#include <Eigen/SVD>
	#include <Eigen/LU>
#endif

#ifdef CBF_HAVE_XSD
	#include <cbf/xml_factory.h>
	#include <sstream>
#endif

namespace CBF {

void vector_from_eigen_string(const std::string str, FloatVectorPtr vec){
	CBF_DEBUG("start parsing string to vector");
	Float value;
	std::vector<Float> values;
	std::istringstream in(str);
	while (in >> value) {
		values.push_back(value);
	}
	vec -> resize(values.size());
	std::copy(values.begin(), values.end(), vec -> data());
	CBF_DEBUG("parsed string: \n" + str + "\n to FloatVector \n" << *vec);
}

void vector_from_boost_string(const std::string str, FloatVectorPtr vec){
	//TODO: use boost if exists
	CBF_DEBUG("start parsing string to vector");
	Float value; // will be written from stream
	int size; // size of the vector, from stream too
	char c_tmp; // used for removing chars from stream.
	std::istringstream in(str);

	// remove '[' - read size - remove ']' and '('
	//in.get(c_tmp);
	in >> c_tmp;
	if (c_tmp != '[') CBF_THROW_RUNTIME_ERROR(
			"[utilities]: vector_from_boost_string(" << str << "): expected '[' but got '" << c_tmp << "'");
	in >> size;
	in >> c_tmp;
	if (c_tmp != ']') CBF_THROW_RUNTIME_ERROR(
			"[utilities]: vector_from_boost_string(" << str << "): expected ']' but got '" << c_tmp << "'");
	in >> c_tmp;
	if (c_tmp != '(') CBF_THROW_RUNTIME_ERROR(
			"[utilities]: vector_from_boost_string(" << str << "): expected '(' but got '" << c_tmp << "'");

	// resize passed vector.
	vec -> resize(size);
	// write values from stream to vector
	for (int i = 0; i < size; ++i) {
		in >> value;
		(*vec)(i) = value;
		// remove ',' or ')'
		in >> c_tmp;
	}
	// check whether the boost-string-representation is at its end
	if (c_tmp != ')') CBF_THROW_RUNTIME_ERROR(
			"[utilities]: vector_from_boost_string(" << str << "): expected ')'");
	CBF_DEBUG("parsed string: \n" + str + "\n to FloatVector \n" << *vec);
}

void matrix_from_eigen_string(const std::string str, FloatMatrixPtr matr){
	CBF_DEBUG("start parsing string to matrix");
	float value;
	std::vector<float> values;
	int rows = count(str.begin(), str.end(), '\n') + 1;
	std::istringstream in(str);
	while (in >> value) {
		values.push_back(value);
	}
	matr -> resize(rows, values.size()/rows);
	int pos = 0;
	//No std::copy because eigen::matrix seems to be written column by column.
	//I tryed std::copy + transposeInPlace() but it is slower
	for (int i = 0; i < rows; ++i){
		for (int j = 0; j < values.size()/rows; ++j){
			(*matr)(i,j) = values.at(pos);
			++pos;
		}
	}
	CBF_DEBUG("parsed string: \n" + str + "\n to FloatMatrix \n" << *matr);
}

void matrix_from_boost_string(const std::string str, FloatMatrixPtr matr){
	//TODO: use boost if exists
	CBF_DEBUG("start parsing string to matrix");
	float value; // will be written from stream
	int rows, cols, row, col;
	char c_tmp; // used for removing chars from stream.
	std::istringstream in(str);

	// remove '[' - read rows - remove ',' - read cols - remove ']' and '('
	in.get(c_tmp);
	if (c_tmp != '[') CBF_THROW_RUNTIME_ERROR(
			"[utilities]: matrix_from_boost_string(" << str << "): wrong form");
	in >> rows;
	in.get(c_tmp);
	if (c_tmp != ',') CBF_THROW_RUNTIME_ERROR(
			"[utilities]: matrix_from_boost_string(" << str << "): wrong form");
	in >> cols;
	in.get(c_tmp);
	if (c_tmp != ']') CBF_THROW_RUNTIME_ERROR(
			"[utilities]: matrix_from_boost_string(" << str << "): wrong form");
	in.get(c_tmp);
	if (c_tmp != '(') CBF_THROW_RUNTIME_ERROR(
			"[utilities]: matrix_from_boost_string(" << str << "): wrong form");

	//resize passed matrix
	matr->resize(rows, cols);
	//write values from stream to matrix
	for (row = 0; row < rows; ++row) {
		in.get(c_tmp); //remove leading '(' of row
		if (c_tmp != '(') CBF_THROW_RUNTIME_ERROR(
				"[utilities]: matrix_from_boost_string(" << str << "): wrong form");
		for (int col = 0; col < cols; ++col) {
			in >> value;
			matr->operator()(row,col) = value;
			in.get(c_tmp); //remove ',' or ')'
		}
		// remove ',' between rows or last ')'
		in.get(c_tmp);
	}
	// check whether the boost-string-representation is at its end
	if (c_tmp != ')') CBF_THROW_RUNTIME_ERROR(
			"[utilities]: matrix_from_boost_string(" << str << "): wrong form");
	CBF_DEBUG("parsed string: \n" + str + "\n to FloatMatrix \n" << matr);
}

FloatVector &slerp(const FloatVector &start, const FloatVector &end, Float step, FloatVector &result) {
	CBF_DEBUG("start: " << start);
	CBF_DEBUG("end: " << end);
	Float inner = start.normalized().dot(end/start.norm());
	CBF_DEBUG("inner: " << 1.0 - inner);
	if (inner > 1.0) inner = 1.0;
	if (inner < -1.0) inner = -1.0;

	Float angle = acos(inner);
	CBF_DEBUG("angle: " << angle);

	if (fabs(angle) <= slerp_threshold) {
		CBF_DEBUG("small angle");
		result = end;
		return result;
	}

	result = start * (sin((1 - step)* angle)/sin(angle)) + (sin(step * angle)/sin(angle)) * end;

	CBF_DEBUG(result);

	return result;
}



#ifdef CBF_HAVE_KDL
FloatMatrix &assign(FloatMatrix &m, const KDL::Jacobian &j) {
	m = j.data;
	return m;
}

FloatMatrix &assign(FloatMatrix &m, const KDL::Frame &f) {
	if (m.rows() != 4 || m.cols() != 4)
		m.resize(4,4);

	for (unsigned int row = 0; row < 4; ++row) {
		for (unsigned int column = 0; column < 4; ++column) {
			m(row, column) = f(row, column);	
		}
	}
	return m;

}
#endif



#ifdef CBF_HAVE_EIGEN
	template<typename CustomUnaryOp>
	Float generic_pseudo_inverse(const FloatMatrix &M, FloatMatrix &result,
										  const CustomUnaryOp& inverter) {
		Eigen::JacobiSVD<FloatMatrix> svd = M.jacobiSvd(Eigen::ComputeThinV | Eigen::ComputeThinU);
		FloatVector tmp = svd.singularValues();
		CBF_DEBUG("singularValues: " << tmp.transpose());
	
		//! Invert singular vectors
		Float det = tmp.head(svd.nonzeroSingularValues()).prod();
		FloatVector si = tmp.unaryExpr(inverter);

		CBF_DEBUG("det: " << det);
		CBF_DEBUG("svd: " << si.transpose());
	
		result = (svd.matrixV() * si.asDiagonal()) * svd.matrixU().transpose();
		return det;
	}

	Float simpleInverse(const Float s) {
		static const double pseudo_inv_precision_threshold = 0.001;
		if (fabs(s) > pseudo_inv_precision_threshold)
			return 1.0 / s;
		else
			return 0.0;
	}
	Float pseudo_inverse(const FloatMatrix &M, FloatMatrix &result) {
		return generic_pseudo_inverse(M, result, std::ptr_fun(simpleInverse));
	}

	template<typename Scalar>
	struct dampedInverse {
		dampedInverse(const Scalar damping_constant) : m_damping(damping_constant) {}
		const Scalar operator()(const Scalar s) const { return s / (m_damping + s*s); }
		Scalar m_damping;
	};
	Float damped_pseudo_inverse(const FloatMatrix &M, FloatMatrix &result, Float damping_constant) {
		return generic_pseudo_inverse(M, result, dampedInverse<Float>(damping_constant));
	}

	struct thesholdInverse {
		thesholdInverse(const Float threshold) : m_threshold(threshold) {}
		const Float operator()(const Float s) const {
			if (s > m_threshold) return 1. / s;
			if (m_threshold > 0) return s / (m_threshold*m_threshold);
			return 0.;
		}
		Float m_threshold;
	};
	Float threshold_pseudo_inverse(const FloatMatrix &M, FloatMatrix &result, const Float threshold) {
		return generic_pseudo_inverse(M, result, thesholdInverse(threshold));
	}
#endif

#ifdef CBF_HAVE_XSD

FloatVectorPtr create_boost_vector(const CBFSchema::BoostVector &xml_instance, ObjectNamespacePtr object_namespace) {
	FloatVectorPtr v(new FloatVector);
	CBF_DEBUG("string: " << xml_instance.String());
	vector_from_boost_string(xml_instance.String(), v);
	if (v -> size() == 0) CBF_THROW_RUNTIME_ERROR("[utilities]: create_vector(): Empty Vector");

	return v;
}

FloatVectorPtr create_simple_vector(const CBFSchema::SimpleVector &xml_instance, ObjectNamespacePtr object_namespace) {
	FloatVectorPtr v(new FloatVector);
	std::vector<Float> tmp;
	for (
		CBFSchema::SimpleVector::Coefficient_const_iterator it = xml_instance.Coefficient().begin();
		it != xml_instance.Coefficient().end();
		++it
	) {
		tmp.push_back(*it);
	}
	v->resize(tmp.size());
	std::copy(tmp.begin(), tmp.end(), v->data());
	
	return v;
}


FloatVectorPtr create_zero_vector(const CBFSchema::ZeroVector &xml_instance, ObjectNamespacePtr object_namespace) {
	FloatVectorPtr ret = FloatVectorPtr(new FloatVector(xml_instance.Dimension()));
	ret -> setZero();
	return ret;
}

FloatVectorPtr create_eigen_vector(const CBFSchema::EigenVector &xml_instance, ObjectNamespacePtr object_namespace) {
	FloatVectorPtr v(new FloatVector);
	CBF_DEBUG("string: " << xml_instance.String());
	vector_from_eigen_string(xml_instance.String(), v);
	if (v -> size() == 0) CBF_THROW_RUNTIME_ERROR("[utilities]: create_vector(): Empty Vector");
	return v;
}

FloatVectorPtr create_basis_vector(const CBFSchema::ZeroVector &xml_instance, ObjectNamespacePtr object_namespace) {
	return FloatVectorPtr(new FloatVector(xml_instance.Dimension()));
}



FloatMatrixPtr create_zero_matrix(const CBFSchema::ZeroMatrix &xml_instance, ObjectNamespacePtr object_namespace) {
	FloatMatrixPtr ret
		= FloatMatrixPtr(new FloatMatrix((int) xml_instance.Rows(), (int) xml_instance.Columns()));
	ret -> setZero();
	return ret;
}

FloatMatrixPtr create_boost_matrix(const CBFSchema::BoostMatrix &xml_instance, ObjectNamespacePtr object_namespace)
{
	FloatMatrixPtr matrix(new FloatMatrix);
	matrix_from_boost_string(xml_instance.String(), matrix);
	CBF_DEBUG(matrix);
	if ((matrix->rows() == 0) && (matrix->cols() == 0)) {
		CBF_THROW_RUNTIME_ERROR("Matrix is empty")
	}
	return matrix;
}

FloatMatrixPtr create_eigen_matrix(const CBFSchema::EigenMatrix &xml_instance, ObjectNamespacePtr object_namespace)
{
	FloatMatrixPtr matrix(new FloatMatrix);
	matrix_from_eigen_string(xml_instance.String(), matrix);
	CBF_DEBUG(matrix);
	if ((matrix->rows() == 0) && (matrix->cols() == 0)) {
		CBF_THROW_RUNTIME_ERROR("Matrix is empty")
	}
	return matrix;
}

#endif

#if defined(CBF_HAVE_XSD) && defined(CBF_HAVE_KDL)

boost::shared_ptr<KDL::Segment> create_tree_segment(const CBFSchema::TreeSegment &xml_instance, ObjectNamespacePtr object_namespace) {
	return create_segment(xml_instance, object_namespace);
}

boost::shared_ptr<KDL::Segment> create_segment(const CBFSchema::Segment &xml_instance, ObjectNamespacePtr object_namespace) {
	boost::shared_ptr<KDL::Frame> frame = create_frame(xml_instance.Frame(), object_namespace);
	boost::shared_ptr<KDL::Joint> joint = create_joint(xml_instance.Joint(), object_namespace);


	CBF_DEBUG("Extracting joint...");


	CBF_DEBUG("Adding Segment for real..");

	if (xml_instance.Name().present()) {
		return boost::shared_ptr<KDL::Segment>(new KDL::Segment(*xml_instance.Name(), *joint, *frame));
	} else {
		return boost::shared_ptr<KDL::Segment>(new KDL::Segment("KDL::Segment", *joint, *frame));
	}
}

boost::shared_ptr<KDL::Frame> create_frame(const CBFSchema::Frame &xml_instance, ObjectNamespacePtr object_namespace) {
	boost::shared_ptr<KDL::Frame> frame(new KDL::Frame);
	const CBFSchema::MatrixFrame *matrix_frame_instance = dynamic_cast<const CBFSchema::MatrixFrame*>(&(xml_instance));

	if (matrix_frame_instance != 0)
	{
		CBF_DEBUG("Extracting matrix...");

		FloatMatrix m;
		m = *XMLFactory<FloatMatrix>::instance()->create((*matrix_frame_instance).Matrix(), object_namespace);

		if (m.rows() != 4 || m.cols() != 4)
			throw std::runtime_error("Matrix is not 4x4");

		for (int row = 0; row < 3; ++row)
		{
			for (int col = 0; col < 3; ++col)
			{
				frame->M(row, col) = m(row, col);
			}
		}

		for (int row = 0; row < 3; ++row)
			frame->p(row) = m(row, 3);
	}
	else
		throw std::runtime_error("Frame type not supported yet..");

	return frame;
}

boost::shared_ptr<KDL::Joint> create_joint(const CBFSchema::Joint &xml_instance, ObjectNamespacePtr object_namespace) {
	boost::shared_ptr<KDL::Joint> joint;

	if (xml_instance.Type() == "Rotational") {
		CBF_DEBUG("Extracting rotational joint...");
		if (xml_instance.Axis() == "X") {
			CBF_DEBUG("X");
			joint = boost::shared_ptr<KDL::Joint>(new KDL::Joint(KDL::Joint::RotX));
		}
		if (xml_instance.Axis() == "Y") {
			CBF_DEBUG("Y");
			joint = boost::shared_ptr<KDL::Joint>(new KDL::Joint(KDL::Joint::RotY));
		}
		if (xml_instance.Axis() == "Z") {
			CBF_DEBUG("Z");
			joint = boost::shared_ptr<KDL::Joint>(new KDL::Joint(KDL::Joint::RotZ));
		}
	}

	if (xml_instance.Type() == "None")
		joint = boost::shared_ptr<KDL::Joint>(new KDL::Joint(KDL::Joint::None));

	if (xml_instance.Type() == "Translational") {
		throw std::logic_error("Translational joints not supported yet. TODO: fix this :)");
	}
	return joint;
}

boost::shared_ptr<KDL::Chain> create_chain(const CBFSchema::ChainBase &xml_instance, ObjectNamespacePtr object_namespace) {
	CBF_DEBUG("adding chain");
	boost::shared_ptr<KDL::Chain> chain;

	//! Check what kind of chain we have:

	const CBFSchema::Chain *chain_instance  = dynamic_cast<const CBFSchema::Chain*>(&xml_instance);

	if (chain_instance == 0) {
		// this is a referenced chain
		chain = XMLObjectFactory::instance()->create<ForeignObject<KDL::Chain> >(
			xml_instance, object_namespace)->m_Object;
		return chain;
	}

	chain.reset(new KDL::Chain);
	for (
		CBFSchema::Chain::Segment_const_iterator it =(*chain_instance).Segment().begin(); 
		it != (*chain_instance).Segment().end();
		++it
	)
	{
		CBF_DEBUG("Adding Segment...");

		boost::shared_ptr<KDL::Segment> segment = XMLFactory<KDL::Segment>::instance()->create(*it, object_namespace);

		chain->addSegment(*segment);
	}
	CBF_DEBUG("number of joints: " << chain->getNrOfJoints());
	return chain;
}

void tree_add_segment(boost::shared_ptr<KDL::Tree> tree, const std::string &current_hook_name, const CBFSchema::TreeSegment &xml_instance, ObjectNamespacePtr object_namespace) {
	CBF_DEBUG("adding segment");
	boost::shared_ptr<KDL::Segment> segment = XMLFactory<KDL::Segment>::instance()->create(xml_instance, object_namespace);

	if (tree->addSegment(*segment, current_hook_name) == false)
		throw std::runtime_error("Adding segment to tree failed");

	for (
		CBFSchema::TreeSegment::Segment1_const_iterator it =(xml_instance).Segment1().begin(); 
		it != (xml_instance).Segment1().end();
		++it
	) {
		if (xml_instance.Name().present()) {
			tree_add_segment(tree, *xml_instance.Name(), *it, object_namespace);
		}
		else {
			tree_add_segment(tree, "KDL::Segment", *it, object_namespace);
		}
	}
}


boost::shared_ptr<KDL::Tree> create_tree(const CBFSchema::Tree &xml_instance, ObjectNamespacePtr object_namespace) {

	CBF_DEBUG("creating tree from xml");
	boost::shared_ptr<KDL::Tree> tree(new KDL::Tree);

	CBF_DEBUG("iterating over segments");

	for (
		CBFSchema::Tree::Segment_const_iterator it =(xml_instance).Segment().begin(); 
		it != (xml_instance).Segment().end();
		++it
	)
	{
		CBF_DEBUG("Adding Segment...");

		tree_add_segment(tree, "root", *it, object_namespace);

		CBF_DEBUG("Number of joints: " << tree->getNrOfJoints());
	}
	return tree;
}


#endif

#ifdef CBF_HAVE_XSD
	static XMLCreator<
		FloatVector, 
		CBFSchema::BoostVector, 
		FloatVectorPtr(*)(const CBFSchema::BoostVector &, ObjectNamespacePtr)
	> x (create_boost_vector);

	static XMLCreator<
		FloatVector, 
		CBFSchema::ZeroVector, 
		FloatVectorPtr(*)(const CBFSchema::ZeroVector &, ObjectNamespacePtr)
	> x2 (create_zero_vector);

	static XMLCreator<
		FloatVector, 
		CBFSchema::SimpleVector, 
		FloatVectorPtr(*)(const CBFSchema::SimpleVector &, ObjectNamespacePtr)
	> x34857 (create_simple_vector);

	static XMLCreator<
		FloatVector,
		CBFSchema::EigenVector,
		FloatVectorPtr(*)(const CBFSchema::EigenVector &, ObjectNamespacePtr)
	> x34878 (create_eigen_vector);

	template <> XMLFactory<FloatVector> 
		*XMLFactory<FloatVector>::m_Instance = 0;

	static XMLDerivedFactory<ForeignObject<FloatVector>, CBFSchema::BoostVector> x7;
	static XMLDerivedFactory<ForeignObject<FloatVector>, CBFSchema::EigenVector> x9;
	static XMLDerivedFactory<ForeignObject<FloatVector>, CBFSchema::SimpleVector> x11;
	static XMLDerivedFactory<ForeignObject<FloatVector>, CBFSchema::ZeroVector> x543;
	static XMLDerivedFactory<ForeignObject<FloatVector>, CBFSchema::EigenVector> x544;


	
	static XMLCreator<
		FloatMatrix, 
		CBFSchema::BoostMatrix, 
		boost::shared_ptr<FloatMatrix>(*)(const CBFSchema::BoostMatrix &, ObjectNamespacePtr)
	> x12 (create_boost_matrix);

	static XMLCreator<
		FloatMatrix, 
		CBFSchema::ZeroMatrix, 
		boost::shared_ptr<FloatMatrix>(*)(const CBFSchema::ZeroMatrix &, ObjectNamespacePtr)
	> x22 (create_zero_matrix);

	static XMLCreator<
		FloatMatrix,
		CBFSchema::EigenMatrix,
		boost::shared_ptr<FloatMatrix>(*)(const CBFSchema::EigenMatrix &, ObjectNamespacePtr)
	> x23 (create_eigen_matrix);

	static XMLDerivedFactory<ForeignObject<FloatMatrix>, CBFSchema::EigenMatrix> x10;
	static XMLDerivedFactory<ForeignObject<FloatMatrix>, CBFSchema::BoostMatrix> x8;
	static XMLDerivedFactory<ForeignObject<FloatMatrix>, CBFSchema::ZeroMatrix> x33;

	template <> XMLFactory<FloatMatrix> 
		*XMLFactory<FloatMatrix>::m_Instance = 0;


#ifdef CBF_HAVE_KDL

	static XMLCreator<
		KDL::Tree, 
		CBFSchema::Tree, 
		boost::shared_ptr<KDL::Tree>(*)(const CBFSchema::Tree &, ObjectNamespacePtr)
	> x3 (create_tree);

	static XMLDerivedFactory<ForeignObject<KDL::Tree>, CBFSchema::Tree> x4;

	template <> XMLFactory<KDL::Tree> 
		*XMLFactory<KDL::Tree>::m_Instance = 0;


	static XMLCreator<
		KDL::Segment, 
		CBFSchema::Segment, 
		boost::shared_ptr<KDL::Segment>(*)(const CBFSchema::Segment &, ObjectNamespacePtr)
	> x13 (create_segment);

	static XMLDerivedFactory<ForeignObject<KDL::Segment>, CBFSchema::Segment> x14;

	template <> XMLFactory<KDL::Segment> 
		*XMLFactory<KDL::Segment>::m_Instance = 0;


	static XMLCreator<
		KDL::Segment, 
		CBFSchema::TreeSegment, 
		boost::shared_ptr<KDL::Segment>(*)(const CBFSchema::TreeSegment &, ObjectNamespacePtr)
	> x1332 (create_tree_segment);

	static XMLDerivedFactory<ForeignObject<KDL::Segment>, CBFSchema::TreeSegment> x14323;

#endif
#endif


} // namespace



