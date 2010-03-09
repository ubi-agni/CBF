#include <cbf/utilities.h>
#include <cbf/debug_macros.h>

#ifdef CBF_HAVE_KDL
	#include <kdl/jacobian.hpp>
#endif

#ifdef CBF_HAVE_EIGEN2
	#include <eigen2/Eigen/Core>
	#include <eigen2/Eigen/Array>
	#include <eigen2/Eigen/SVD>
	#include <eigen2/Eigen/LU>
#endif

#ifdef CBF_HAVE_XSD
	#include <schemas.hxx>
	#include <sstream>
	#include <boost/numeric/ublas/io.hpp>
#endif

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

namespace CBF {



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
	Float undamped_pseudo_inverse(const FloatMatrix &M, FloatMatrix &result) {
		bool transpose = false;
	
		if (M.size2() > M.size1()) transpose = true;
	
		//! Placeholders for the singular value decomposition
		Eigen::MatrixXd m((int)M.size1(), (int)M.size2());
	
		//! rows and cols hold dimensions of input matrix
		int rows = (int)M.size1();
		int cols = (int)M.size2();
	
		for (int row = 0; row < rows; ++row)
		{
			for (int col = 0; col < cols; ++col)
			{
				m(row,col) = M(row,col);	
			}
		}
	
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
			SvMatrix(i,i) = Sv(i,0) / (0.00 + (Sv(i,0) * Sv(i,0)));
			det *= SvMatrix(i,i);
#if 0
			if (Sv(i,0) > 0.001)
				SvMatrix(i,i) = 1.0 / (Sv(i,0));
			else {
				CBF_DEBUG("SINGULAR")
				SvMatrix(i,i) = 0.0;
			}
#endif
			// std::cout << Sv(i,0)  << std::endl;
		}
		CBF_DEBUG("deter:" << det)
		//for (int i = 0; i < Sv.rows(); ++i) SvMatrix(i,i) = Sv(i,0) / (1.0 + Sv(i,0));
	
		CBF_DEBUG("svd: "<< std::endl << SvMatrix)
	
		Eigen::MatrixXd res = (svd.matrixV() * SvMatrix) * svd.matrixU().transpose();
	
		result = FloatMatrix(res.rows(), res.cols());
	
		for (int row = 0; row < res.rows(); ++row)
		{
			for (int col = 0; col < res.cols(); ++col)
			{
				result(row,col) = res(row,col);
			}
		}
	
		if (transpose) result = ublas::trans(result);
		return det;
	}

	Float pseudo_inverse(const FloatMatrix &M, FloatMatrix &result) {
		bool transpose = false;
	
		if (M.size2() > M.size1()) transpose = true;
	
		//! Placeholders for the singular value decomposition
		Eigen::MatrixXd m((int)M.size1(), (int)M.size2());
	
		//! rows and cols hold dimensions of input matrix
		int rows = (int)M.size1();
		int cols = (int)M.size2();
	
		for (int row = 0; row < rows; ++row)
		{
			for (int col = 0; col < cols; ++col)
			{
				m(row,col) = M(row,col);	
			}
		}
	
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
			SvMatrix(i,i) = Sv(i,0) / (0.001 + (Sv(i,0) * Sv(i,0)));
			det *= SvMatrix(i,i);
#if 0
			if (Sv(i,0) > 0.001)
				SvMatrix(i,i) = 1.0 / (Sv(i,0));
			else {
				CBF_DEBUG("SINGULAR")
				SvMatrix(i,i) = 0.0;
			}
#endif
			// std::cout << Sv(i,0)  << std::endl;
		}
		CBF_DEBUG("deter:" << det)
		//for (int i = 0; i < Sv.rows(); ++i) SvMatrix(i,i) = Sv(i,0) / (1.0 + Sv(i,0));
	
		CBF_DEBUG("svd: "<< std::endl << SvMatrix)
	
		Eigen::MatrixXd res = (svd.matrixV() * SvMatrix) * svd.matrixU().transpose();
	
		result = FloatMatrix(res.rows(), res.cols());
	
		for (int row = 0; row < res.rows(); ++row)
		{
			for (int col = 0; col < res.cols(); ++col)
			{
				result(row,col) = res(row,col);
			}
		}
	
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

} // namespace



