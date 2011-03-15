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


    Copyright 2009, 2010 Florian Paul Schmidt
*/

#include <cbf/utilities.h>
#include <cbf/debug_macros.h>
#include <cbf/exceptions.h>
#include <cbf/xml_factory.h>

#ifdef CBF_HAVE_KDL
	#include <kdl/jacobian.hpp>
	#include <kdl/tree.hpp>
	#include <kdl/chain.hpp>
	#include <kdl/segment.hpp>
	#include <kdl/joint.hpp>
#endif

#ifdef CBF_HAVE_EIGEN2
	#include <Eigen/Core>
	#include <Eigen/Array>
	#include <Eigen/SVD>
	#include <Eigen/LU>
#endif

#ifndef CBF_HAVE_EIGEN2
	#include <cbf/svd.h>
#endif

#ifdef CBF_HAVE_XSD
	#include <cbf/xml_factory.h>
	#include <sstream>
	#include <boost/numeric/ublas/io.hpp>
#endif

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

namespace CBF {

FloatVector &slerp(const FloatVector &start, const FloatVector &end, Float step, FloatVector &result) {
	CBF_DEBUG("start: " << start);
	CBF_DEBUG("end: " << end);
	Float inner = ublas::inner_prod(start/ublas::norm_2(start), end/ublas::norm_2(start));
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
	if (m.size1() != j.rows() || m.size2() != j.columns())
		m = ublas::zero_matrix<Float>(j.rows(), j.columns());

	unsigned int rows, columns;
	rows = j.rows();
	columns = j.columns();

	for (unsigned int row = 0; row < rows; ++row) {
		for (unsigned int column = 0; column < columns; ++column) {
			m(row, column) = j(row, column);	
		}
	}
	return m;
}

FloatMatrix &assign(FloatMatrix &m, const KDL::Frame &f) {
	if (m.size1() != 4 || m.size2() != 4)
		m = ublas::zero_matrix<Float>(4,4);

	for (unsigned int row = 0; row < 4; ++row) {
		for (unsigned int column = 0; column < 4; ++column) {
			m(row, column) = f(row, column);	
		}
	}
	return m;

}
#endif



#ifdef CBF_HAVE_EIGEN2
	static const double pseudo_inv_precision_threshold = 0.001;

	Float pseudo_inverse(const FloatMatrix &M, FloatMatrix &result) {
		bool transpose = false;
	
		if (M.size2() > M.size1()) transpose = true;
	
		//! Placeholders for the singular value decomposition
		Eigen::MatrixXd m((int)M.size1(), (int)M.size2());
	
		//! rows and cols hold dimensions of input matrix
		int rows = (int)M.size1();
		int cols = (int)M.size2();
	
		for (int row = 0; row < rows; ++row)
			for (int col = 0; col < cols; ++col)
				m(row,col) = M(row,col);	
	
		if (transpose) m.transposeInPlace();
	
		Eigen::SVD<Eigen::MatrixXd> svd = m.svd();
	
		const Eigen::MatrixXd& Sv = svd.singularValues();
		CBF_DEBUG("singularValues: " << Sv);
	
		//! Prepare a diagonal matrix from the singularValues vector
		Eigen::MatrixXd SvMatrix(Sv.rows(), Sv.rows());
		SvMatrix.setZero();
	
		Float det = 1.0;
		for (int i = 0; i < Sv.rows(); ++i) {
			det *= SvMatrix(i,i);
			if (fabs(Sv(i,0)) > pseudo_inv_precision_threshold)
				SvMatrix(i,i) = 1.0 / (Sv(i,0));
			else {
				CBF_DEBUG("SINGULAR");
				SvMatrix(i,i) = 0.0;
			}
		}
		CBF_DEBUG("deter:" << det);
		//for (int i = 0; i < Sv.rows(); ++i) SvMatrix(i,i) = Sv(i,0) / (1.0 + Sv(i,0));
	
		CBF_DEBUG("svd: "<< std::endl << SvMatrix);
	
		Eigen::MatrixXd res = (svd.matrixV() * SvMatrix) * svd.matrixU().transpose();
	
		result = FloatMatrix(res.rows(), res.cols());
	
		for (int row = 0; row < res.rows(); ++row)
			for (int col = 0; col < res.cols(); ++col)
				result(row,col) = res(row,col);
	
		if (transpose) result = ublas::trans(result);
		return det;
	}

	Float damped_pseudo_inverse(const FloatMatrix &M, FloatMatrix &result, Float damping_constant) {
		bool transpose = false;
	
		if (M.size2() > M.size1()) transpose = true;
	
		//! Placeholders for the singular value decomposition
		Eigen::MatrixXd m((int)M.size1(), (int)M.size2());
	
		//! rows and cols hold dimensions of input matrix
		int rows = (int)M.size1();
		int cols = (int)M.size2();
	
		for (int row = 0; row < rows; ++row)
			for (int col = 0; col < cols; ++col)
				m(row,col) = M(row,col);	
	
		if (transpose) m.transposeInPlace();
	
		Eigen::SVD<Eigen::MatrixXd> svd = m.svd();
	
		const Eigen::MatrixXd& Sv = svd.singularValues();
		CBF_DEBUG("singularValues: " << Sv);
	
		//! Prepare a diagonal matrix from the singularValues vector
		Eigen::MatrixXd SvMatrix(Sv.rows(), Sv.rows());
		SvMatrix.setZero();
	
		Float det = 1.0;
		//! We use the ordinary reciprocal for testing purposes here
		for (int i = 0; i < Sv.rows(); ++i) {
			SvMatrix(i,i) = Sv(i,0) / (damping_constant + (Sv(i,0) * Sv(i,0)));
			det *= SvMatrix(i,i);
			// std::cout << Sv(i,0)  << std::endl;
		}
		CBF_DEBUG("deter:" << det);
		//for (int i = 0; i < Sv.rows(); ++i) SvMatrix(i,i) = Sv(i,0) / (1.0 + Sv(i,0));
	
		CBF_DEBUG("svd: "<< std::endl << SvMatrix);
	
		Eigen::MatrixXd res = (svd.matrixV() * SvMatrix) * svd.matrixU().transpose();
	
		result = FloatMatrix(res.rows(), res.cols());
	
		for (int row = 0; row < res.rows(); ++row)
			for (int col = 0; col < res.cols(); ++col)
				result(row,col) = res(row,col);

		if (transpose) result = ublas::trans(result);
		return det;
	}
#endif


#ifndef CBF_HAVE_EIGEN2
static const double pseudo_inv_precision_threshold = 0.001;

Float pseudo_inverse(const FloatMatrix &M, FloatMatrix &result) {
  bool transpose = false;

  if (M.size2() > M.size1()) transpose = true;

  FloatMatrix m = M;
  if (transpose){
    m = ublas::trans(M);
  }

  //! Placeholders for the singular value decomposition
  FloatMatrix u(m.size1(), m.size1());
  FloatMatrix v(m.size2(), m.size2());
  FloatMatrix q(m.size2(), m.size1());
  svd(1, 1, 10e-18, 10e-18, m, q, u, v);

  //! Working through the singularValues Matrix calculating the determinant
  Float det = 1.0;
  for (int i = 0; i < m.size2(); ++i) {
    det *= q(i,i); ////??????
    if (fabs(q(i,i)) > pseudo_inv_precision_threshold){
      q(i,i) = 1.0 / (q(i,i));
    } else {
      CBF_DEBUG("SINGULAR");
      q(i,i) = 0.0;
    }
  }

  CBF_DEBUG("det: " << det);

  CBF_DEBUG("svd: "<< std::endl << q);

  //! Calculating the Moore-Penrose-Pseudoinverse
  FloatMatrix res(v.size1(), q.size2());
  axpy_prod(v, q, res, true);
  std::cout << std::endl;
  FloatMatrix res2(m.size2(), m.size1());
  axpy_prod(res, ublas::trans(u), res2, true);
  
  if (transpose){
    result = ublas::trans(res2);
  } else {
    result = res2;
  }

  return det;
}

Float damped_pseudo_inverse(const FloatMatrix &M, FloatMatrix &result, 
                            Float damping_constant) {
  bool transpose = false;

  if (M.size2() > M.size1()) transpose = true;

  FloatMatrix m = M;
  if (transpose){
    m = ublas::trans(M);
  }

  //! Placeholders for Singular Value Decomposition
  FloatMatrix u(m.size1(), m.size1());
  FloatMatrix v(m.size2(), m.size2());
  FloatMatrix q(m.size2(), m.size1());

  svd(1, 1, 10e-18, 10e-18, m, q, u, v);

  Float det = 1.0;
  //! We use the ordinary reciprocal for testing purposes here
  for (int i = 0; i < m.size2(); ++i) {
    det *= q(i,i);
    q(i,i) = q(i,i) / (damping_constant + (q(i,i) * q(i,i)));
  }

  CBF_DEBUG("det: " << det);

  CBF_DEBUG("svd: "<< std::endl << q);

  //! Calculating the Moore-Penrose-Pseudoinverse
  FloatMatrix res(v.size1(), q.size2());
  axpy_prod(v, q, res, true);
  FloatMatrix res2(m.size2(), m.size1());
  axpy_prod(res, ublas::trans(u), res2, true);

  if (transpose){
    result = ublas::trans(res2);
  } else {
    result = res2;
  }

  return det;
}
#endif

#ifdef CBF_HAVE_XSD
FloatVector create_vector(const CBFSchema::Vector &xml_instance, ObjectNamespacePtr object_namespace) {
	const CBFSchema::SimpleVector *simple_vector = dynamic_cast<const CBFSchema::SimpleVector*>(&xml_instance);

	if (simple_vector) {
		std::vector<Float> tmp;
		for (
			CBFSchema::SimpleVector::Coefficient_const_iterator it = (*simple_vector).Coefficient().begin();
			it != (*simple_vector).Coefficient().end();
			++it
		)
		{
			tmp.push_back((*it));
		}
		ublas::vector<Float> ret(tmp.size());
		std::copy(tmp.begin(), tmp.end(), ret.begin());
		return ret;
	}

	const CBFSchema::BoostVector *boost_vector = dynamic_cast<const CBFSchema::BoostVector*>(&xml_instance);

	if (boost_vector) {
		std::stringstream stream(boost_vector->String());
		CBF_DEBUG("string: " << stream.str());
		ublas::vector<Float> v;
		stream >> v;
		if (v.size() == 0) CBF_THROW_RUNTIME_ERROR("[utilities]: create_vector(): Empty Vector");

		return v;
	}

	throw std::runtime_error("[utilities]: create_vector(): Unknown VectorType");
}

boost::shared_ptr<FloatVector> create_boost_vector(const CBFSchema::BoostVector &xml_instance, ObjectNamespacePtr object_namespace) {
	boost::shared_ptr<FloatVector> v(new FloatVector);
	std::stringstream stream(xml_instance.String());
	CBF_DEBUG("string: " << stream.str());
	stream >> *v;
	if ((*v).size() == 0) CBF_THROW_RUNTIME_ERROR("[utilities]: create_vector(): Empty Vector");

	return v;
}

boost::shared_ptr<FloatVector> create_zero_vector(const CBFSchema::ZeroVector &xml_instance, ObjectNamespacePtr object_namespace) {
	return boost::shared_ptr<FloatVector>(new FloatVector(xml_instance.Dimension(), 0));
}

boost::shared_ptr<FloatVector> create_basis_vector(const CBFSchema::ZeroVector &xml_instance, ObjectNamespacePtr object_namespace) {
	return boost::shared_ptr<FloatVector>(new FloatVector(xml_instance.Dimension()));
}


boost::shared_ptr<FloatMatrix> create_zero_matrix(const CBFSchema::ZeroMatrix &xml_instance, ObjectNamespacePtr object_namespace) {
	return boost::shared_ptr<FloatMatrix>(new FloatMatrix(xml_instance.Rows(), xml_instance.Columns(), 0));
}


FloatMatrix create_matrix(const CBFSchema::Matrix &xml_instance, ObjectNamespacePtr object_namespace)
{
	const CBFSchema::Matrix *m = &xml_instance;

	const CBFSchema::BoostMatrix *m2 = dynamic_cast<const CBFSchema::BoostMatrix*>(m);
	if (m2) {
		FloatMatrix matrix;
		std::stringstream stream(std::string(m2->String()));
		CBF_DEBUG("string: " << stream.str());
		stream >> matrix;
		CBF_DEBUG(matrix);
		if ((matrix.size1() == 0) && (matrix.size2() == 0)) {
			CBF_THROW_RUNTIME_ERROR("Matrix is empty")
		}

		return matrix;
	}

	const CBFSchema::ZeroMatrix *m3 = dynamic_cast<const CBFSchema::ZeroMatrix*>(m);
	if (m3) {
		return FloatMatrix(m3->Rows(), m3->Columns(), 0);
	}

	throw std::runtime_error("[create_matrix()]: Matrix type not supported yet");
}

#endif

#ifdef CBF_HAVE_XSD
#ifdef CBF_HAVE_KDL

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
		m = create_matrix((*matrix_frame_instance).Matrix(), object_namespace);

		if (m.size1() != 4 || m.size2() != 4)
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
	boost::shared_ptr<KDL::Chain> chain(new KDL::Chain);

	//! Check what kind of chain we have:

	const CBFSchema::Chain *chain_instance  = dynamic_cast<const CBFSchema::Chain*>(&xml_instance);

	if (chain_instance == 0)
		throw std::runtime_error("Chain type not handled yet..");

	for (
		CBFSchema::Chain::Segment_const_iterator it =(*chain_instance).Segment().begin(); 
		it != (*chain_instance).Segment().end();
		++it
	)
	{
		CBF_DEBUG("Adding Segment...");

		boost::shared_ptr<KDL::Segment> segment = create_segment(*it, object_namespace);

		chain->addSegment(*segment);
		CBF_DEBUG("number of joints: " << chain->getNrOfJoints());
	}
	return chain;
}

void tree_add_segment(boost::shared_ptr<KDL::Tree> tree, const std::string &current_hook_name, const CBFSchema::TreeSegment &xml_instance, ObjectNamespacePtr object_namespace) {
	CBF_DEBUG("adding segment");
	boost::shared_ptr<KDL::Segment> segment = create_segment(xml_instance, object_namespace);

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

boost::shared_ptr<KDL::Tree> create_tree(const CBFSchema::TreeBase &xml_instance, ObjectNamespacePtr object_namespace) {

	CBF_DEBUG("creating tree from xml");
	boost::shared_ptr<KDL::Tree> tree(new KDL::Tree);

	//! Check what kind of tree we have:


	const CBFSchema::Tree *tree_instance  = dynamic_cast<const CBFSchema::Tree*>(&xml_instance);

	if (tree_instance == 0) {
		CBF_DEBUG("tree type not handled yet");
		throw std::runtime_error("Tree type not handled yet..");
	}


	for (
		CBFSchema::Tree::Segment_const_iterator it =(*tree_instance).Segment().begin(); 
		it != (*tree_instance).Segment().end();
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
#endif

#ifdef CBF_HAVE_XSD
	static XMLCreator<
		FloatVector, 
		CBFSchema::BoostVector, 
		boost::shared_ptr<FloatVector>(*)(const CBFSchema::BoostVector &, ObjectNamespacePtr)
	> x(create_boost_vector);

	static XMLCreator<
		FloatVector, 
		CBFSchema::ZeroVector, 
		boost::shared_ptr<FloatVector>(*)(const CBFSchema::ZeroVector &, ObjectNamespacePtr)
	> x2(create_zero_vector);
	
	template <> XMLFactory<FloatVector> 
		*XMLFactory<FloatVector>::m_Instance = 0;
#endif


} // namespace



