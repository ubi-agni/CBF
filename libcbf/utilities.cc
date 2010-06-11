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

#ifdef CBF_HAVE_XSD
	#include <schemas.hxx>
	#include <sstream>
	#include <boost/numeric/ublas/io.hpp>
#endif

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

namespace CBF {

FloatVector &slerp(const FloatVector &start, const FloatVector &end, Float step, FloatVector &result) {
	Float angle = acos(ublas::inner_prod(start, end));

	if (fabs(angle) <= slerp_threshold) {
		CBF_DEBUG("small angle")
		result = end;
		return result;
	}

	result = start * (sin((1 - step)* angle)/sin(angle)) + (sin(step * angle)/sin(angle)) * end;

	CBF_DEBUG(result)

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
				CBF_DEBUG("SINGULAR")
				SvMatrix(i,i) = 0.0;
			}
		}
		CBF_DEBUG("deter:" << det)
		//for (int i = 0; i < Sv.rows(); ++i) SvMatrix(i,i) = Sv(i,0) / (1.0 + Sv(i,0));
	
		CBF_DEBUG("svd: "<< std::endl << SvMatrix)
	
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
		CBF_DEBUG("deter:" << det)
		//for (int i = 0; i < Sv.rows(); ++i) SvMatrix(i,i) = Sv(i,0) / (1.0 + Sv(i,0));
	
		CBF_DEBUG("svd: "<< std::endl << SvMatrix)
	
		Eigen::MatrixXd res = (svd.matrixV() * SvMatrix) * svd.matrixU().transpose();
	
		result = FloatMatrix(res.rows(), res.cols());
	
		for (int row = 0; row < res.rows(); ++row)
			for (int col = 0; col < res.cols(); ++col)
				result(row,col) = res(row,col);

		if (transpose) result = ublas::trans(result);
		return det;
	}
#endif

#ifdef CBF_HAVE_XSD
ublas::vector<Float> create_vector(const VectorType &xml_instance) {
	const SimpleVectorType *simple_vector = dynamic_cast<const SimpleVectorType*>(&xml_instance);

	if (simple_vector) {
		std::vector<Float> tmp;
		for (
			SimpleVectorType::Coefficient_const_iterator it = (*simple_vector).Coefficient().begin();
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

	const BoostVectorType *boost_vector = dynamic_cast<const BoostVectorType*>(&xml_instance);

	if (boost_vector) {
		std::stringstream stream(boost_vector->String());
		ublas::vector<Float> v;
		stream >> v;
		if (v.size() == 0) throw std::runtime_error("[utilities]: create_vector(): Empty Vector");

		return v;
	}

	throw std::runtime_error("[utilities]: create_vector(): Unknown VectorType");
}


FloatMatrix create_matrix(const MatrixType &xml_instance)
{
	const MatrixType *m = &xml_instance;

	const BoostMatrixType *m2 = dynamic_cast<const BoostMatrixType*>(m);
	if (m2) {
		FloatMatrix matrix;
		std::stringstream stream(std::string(m2->String()));
		stream >> matrix;
		CBF_DEBUG(matrix)
		if ((matrix.size1() == 0) && (matrix.size2() == 0)) throw std::runtime_error("[LinearEffectorTransform]: Matrix is empty");

		return matrix;
	}

	throw std::runtime_error("[create_matrix()]: Matrix type not supported yet");
}

#endif

#ifdef CBF_HAVE_XSD
#ifdef CBF_HAVE_KDL

boost::shared_ptr<KDL::Segment> create_segment(const SegmentType &xml_instance) {
	boost::shared_ptr<KDL::Frame> frame = create_frame(xml_instance.Frame());
	boost::shared_ptr<KDL::Joint> joint = create_joint(xml_instance.Joint());


	CBF_DEBUG("Extracting joint...")


	CBF_DEBUG("Adding Segment for real..")

	return boost::shared_ptr<KDL::Segment>(new KDL::Segment(xml_instance.Name(), *joint, *frame));
}

boost::shared_ptr<KDL::Frame> create_frame(const FrameType &xml_instance) {
	boost::shared_ptr<KDL::Frame> frame(new KDL::Frame);
	const MatrixFrameType *matrix_frame_instance = dynamic_cast<const MatrixFrameType*>(&(xml_instance));

	if (matrix_frame_instance != 0)
	{
		CBF_DEBUG("Extracting matrix...")

		FloatMatrix m;
		m = create_matrix((*matrix_frame_instance).Matrix());

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

boost::shared_ptr<KDL::Joint> create_joint(const JointType &xml_instance) {
	boost::shared_ptr<KDL::Joint> joint;

	if (xml_instance.Type() == "Rotational") {
		CBF_DEBUG("Extracting rotational joint...")
		if (xml_instance.Axis() == "X") {
			CBF_DEBUG("X")
			joint = boost::shared_ptr<KDL::Joint>(new KDL::Joint(KDL::Joint::RotX));
		}
		if (xml_instance.Axis() == "Y") {
			CBF_DEBUG("Y")
			joint = boost::shared_ptr<KDL::Joint>(new KDL::Joint(KDL::Joint::RotY));
		}
		if (xml_instance.Axis() == "Z") {
			CBF_DEBUG("Z")
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

boost::shared_ptr<KDL::Chain> create_chain(const ChainBaseType &xml_instance) {
	boost::shared_ptr<KDL::Chain> chain(new KDL::Chain);

	//! Check what kind of chain we have:

	const ChainType *chain_instance  = dynamic_cast<const ChainType*>(&xml_instance);

	if (chain_instance == 0)
		throw std::runtime_error("Chain type not handled yet..");

	for (
		ChainType::Segment_const_iterator it =(*chain_instance).Segment().begin(); 
		it != (*chain_instance).Segment().end();
		++it
	)
	{
		CBF_DEBUG("Adding Segment...")

		boost::shared_ptr<KDL::Segment> segment = create_segment(*it);

		chain->addSegment(*segment);
		CBF_DEBUG("number of joints: " << chain->getNrOfJoints())
	}
	return chain;
}

void tree_add_segment(boost::shared_ptr<KDL::Tree> tree, const std::string &current_hook_name, const TreeSegmentType &xml_instance) {
	boost::shared_ptr<KDL::Segment> segment = create_segment(xml_instance);

	if (tree->addSegment(*segment, current_hook_name) == false)
		throw std::runtime_error("Adding segment to tree failed");

	for (
		TreeSegmentType::Segment_const_iterator it =(xml_instance).Segment().begin(); 
		it != (xml_instance).Segment().end();
		++it
	) {
		tree_add_segment(tree, xml_instance.Name(), *it);
	}
}

boost::shared_ptr<KDL::Tree> create_tree(const TreeBaseType &xml_instance) {
	boost::shared_ptr<KDL::Tree> tree(new KDL::Tree);

	//! Check what kind of tree we have:

	const TreeType *tree_instance  = dynamic_cast<const TreeType*>(&xml_instance);

	if (tree_instance == 0)
		throw std::runtime_error("Tree type not handled yet..");

	for (
		TreeType::Segment_const_iterator it =(*tree_instance).Segment().begin(); 
		it != (*tree_instance).Segment().end();
		++it
	)
	{
		CBF_DEBUG("Adding Segment...")

		tree_add_segment(tree, "root", *it);

		CBF_DEBUG("Number of joints: " << tree->getNrOfJoints())
	}
	return tree;
}

#endif
#endif

} // namespace



