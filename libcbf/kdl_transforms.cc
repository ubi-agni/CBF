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

#include <cbf/kdl_transforms.h>

#include <cbf/utilities.h>
#include <cbf/debug_macros.h>
#include <cbf/utilities.h>
#include <cbf/xml_object_factory.h>
#include <cbf/foreign_object.h>
//#include <cbf/xml_factory.h>

#include <kdl/chain.hpp>
#include <kdl/tree.hpp>
#include <kdl/jntarray.hpp>
#include <kdl/frames.hpp>
#include <kdl/chainjnttojacsolver.hpp>
#include <kdl/chainfksolverpos_recursive.hpp>
#include <kdl/treejnttojacsolver.hpp>
#include <kdl/treefksolverpos_recursive.hpp>
#include <kdl/jntarrayvel.hpp>

namespace CBF {
	
	
	BaseKDLChainSensorTransform::BaseKDLChainSensorTransform(boost::shared_ptr<KDL::Chain> chain) :
		m_Chain(chain),
		m_Frame(new KDL::Frame),
		m_Jacobian(new KDL::Jacobian)
	{
		init_solvers();
	}
	
	void BaseKDLChainSensorTransform::init_solvers() {
	
		m_Twists = FloatMatrix(6, m_Chain->getNrOfJoints());
	
		m_JacSolver = boost::shared_ptr<KDL::ChainJntToJacSolver>(
			new KDL::ChainJntToJacSolver(*m_Chain)
		);
	
		m_FKSolver = boost::shared_ptr<KDL::ChainFkSolverPos_recursive>(
			new KDL::ChainFkSolverPos_recursive(*m_Chain)
		);
	
		m_Jacobian = boost::shared_ptr<KDL::Jacobian>(new KDL::Jacobian(m_Chain->getNrOfJoints()));
	}
	
	void BaseKDLChainSensorTransform::update(const FloatVector &resource_value) {
		KDL::JntArray jnt_array(resource_dim());
	
		for (unsigned int i = 0; i < resource_dim(); ++i) {
			jnt_array(i) = resource_value[i];
		}
	
		m_JacSolver->JntToJac(jnt_array, *m_Jacobian);
		m_FKSolver->JntToCart(jnt_array, *m_Frame);
	}
	
	unsigned int BaseKDLChainSensorTransform::resource_dim() const {
		return m_Chain->getNrOfJoints();
	}
	
	
	
	KDLChainPositionSensorTransform::KDLChainPositionSensorTransform(boost::shared_ptr<KDL::Chain> chain) :
		BaseKDLChainSensorTransform(chain)
	{
		m_TaskDim = 3;
		m_ResourceDim = chain->getNrOfJoints();

		m_Result = FloatVector(3);
	}
	
	void KDLChainPositionSensorTransform::update(const FloatVector &resource_value) {
		BaseKDLChainSensorTransform::update(resource_value);

		//! Update the jacobian so we can hand it out...
		m_TaskJacobian = FloatMatrix(3, resource_dim());
		for (unsigned int i = 0; i < 3; ++i) {
			for (unsigned int j = 0; j < resource_dim(); ++j) {
				m_TaskJacobian(i,j) = (*m_Jacobian)(i,j);
			}
		}
	
		//! Buffer result, so we can return it when requested...
		for (unsigned int i = 0; i < 3; ++i) {
			m_Result[i] = m_Frame->p(i);
		}
	}
	
	
	KDLChainAxisAngleSensorTransform::KDLChainAxisAngleSensorTransform(boost::shared_ptr<KDL::Chain> chain) :
		BaseKDLChainSensorTransform(chain)
	{
		m_TaskDim = 3;
		m_ResourceDim = chain->getNrOfJoints();
		m_Result = FloatVector(3);
	}
	
	void KDLChainAxisAngleSensorTransform::update(const FloatVector &resource_value) {
		BaseKDLChainSensorTransform::update(resource_value);
	
		//! Update the jacobian so we can hand it out...
		m_TaskJacobian = FloatMatrix::Zero(3, resource_dim());
	
		for (unsigned int i = 0; i < 3; ++i) {
			for (unsigned int j = 0; j < resource_dim(); ++j) {
				m_TaskJacobian(i,j) = (*m_Jacobian)(i+3, j);
				//CBF_DEBUG("la! " << m_ConcreteJacobian(i,j))
			}
		}
	
		//! Buffer result, so we can return it when requested...
		KDL::Vector vec;
		float angle;
		angle = (m_Frame->M).GetRotAngle(vec, 0.0000000000001);
	
		// angle = fmod(angle + 2.0 * M_PI, 2.0 * M_PI);
		if (angle > M_PI) angle -= 2.0 * M_PI;
		if (angle <= -M_PI) angle += 2.0 * M_PI;
	
		for (unsigned int i = 0; i < 3; ++i) {
			m_Result[i] = vec(i) * angle;
		}
	}


	BaseKDLTreeSensorTransform::BaseKDLTreeSensorTransform(
		boost::shared_ptr<KDL::Tree> tree,
		const std::vector<std::string> &segment_names
	) :
		m_Tree(tree),
		m_SegmentNames(segment_names)
	{
		init_solvers();
	}
	
	void BaseKDLTreeSensorTransform::init_solvers() {
	
		m_Twists = FloatMatrix(6, m_Tree->getNrOfJoints());
		CBF_DEBUG("nr of joints: " << m_Tree->getNrOfJoints());
	
		m_JacSolver = boost::shared_ptr<KDL::TreeJntToJacSolver>(
			new KDL::TreeJntToJacSolver(*m_Tree)
		);
	
		m_FKSolver = boost::shared_ptr<KDL::TreeFkSolverPos_recursive>(
			new KDL::TreeFkSolverPos_recursive(*m_Tree)
		);

		for (unsigned int i = 0; i < m_SegmentNames.size(); ++i) {
			if (m_Tree->getSegments().find(m_SegmentNames[i]) == m_Tree->getSegments().end())
				CBF_THROW_RUNTIME_ERROR("The tree has no segment with name: " << m_SegmentNames[i]);

			m_Jacobians.push_back(boost::shared_ptr<KDL::Jacobian>(new KDL::Jacobian(m_Tree->getNrOfJoints())));
			m_Frames.push_back(boost::shared_ptr<KDL::Frame>(new KDL::Frame));
		}
	}
	
	void BaseKDLTreeSensorTransform::update(const FloatVector &resource_value) {
		KDL::JntArray jnt_array(resource_dim());
	
		CBF_DEBUG(resource_value);
	
		for (unsigned int i = 0; i < resource_dim(); ++i) {
			jnt_array(i) = resource_value[i];
		}

		for (unsigned int i = 0; i < m_SegmentNames.size(); ++i) {
			m_JacSolver->JntToJac(jnt_array, *(m_Jacobians[i]), m_SegmentNames[i]);
			m_FKSolver->JntToCart(jnt_array, *(m_Frames[i]), m_SegmentNames[i]);
		}
	
	}
	
	unsigned int BaseKDLTreeSensorTransform::resource_dim() const {
		return m_Tree->getNrOfJoints();
	}
	

	KDLTreePositionSensorTransform::KDLTreePositionSensorTransform(
		boost::shared_ptr<KDL::Tree> tree, 
		std::vector<std::string> segment_names
	) : 
		BaseKDLTreeSensorTransform(tree, segment_names)
	{
		m_TaskDim = 3;
		m_ResourceDim = tree->getNrOfJoints();
		m_Result = FloatVector(3 * segment_names.size());
		m_TaskJacobian = FloatMatrix((int) 3 * segment_names.size(), (int) tree->getNrOfJoints());

		CBF_DEBUG("# of segments: " << segment_names.size());
		for (unsigned int i = 0; i < segment_names.size(); ++i) {
			CBF_DEBUG("name: " << segment_names[i]);
		}
	}




	void KDLTreePositionSensorTransform::update(const FloatVector &resource_value) {
		BaseKDLTreeSensorTransform::update(resource_value);

		unsigned int total_row = 0;

		for (unsigned int i = 0, len = m_SegmentNames.size(); i < len; ++i) {
			for (unsigned int row = 0; row < 3; ++row, ++total_row) {
				for (unsigned int col = 0; col < resource_dim(); ++col) {
					m_TaskJacobian(total_row, col) = (*m_Jacobians[i])(row,col);
				}
				m_Result[total_row] = m_Frames[i]->p(row);
			}
		}
		CBF_DEBUG("TaskJacobian " << m_TaskJacobian);
	}
	




	KDLTreeAxisAngleSensorTransform::KDLTreeAxisAngleSensorTransform(
		boost::shared_ptr<KDL::Tree> tree, 
		std::vector<std::string> segment_names
	) : 
		BaseKDLTreeSensorTransform(tree, segment_names)
	{
		m_TaskDim = 3*segment_names.size();
		m_ResourceDim = tree->getNrOfJoints();

		m_Result = FloatVector(3 * segment_names.size());
		m_TaskJacobian = FloatMatrix((int) 3 * segment_names.size(),(int)  tree->getNrOfJoints());
	}


	void KDLTreeAxisAngleSensorTransform::update(const FloatVector &resource_value) {
		BaseKDLTreeSensorTransform::update(resource_value);

		CBF_DEBUG("Updating Jacobian");
	
		unsigned int total_row = 0;
		for (unsigned int i = 0, len = m_SegmentNames.size(); i < len; ++i) {
			//! Buffer forward kinematics, so we can return it when requested...
			const KDL::Vector& axis = m_Frames[i]->M.GetRot();
			for (unsigned int row = 0; row < 3; ++row) {
				m_Result[total_row+row] = axis(row);
			}
			
			//! Buffer task jacobian
			for (unsigned int row = 0; row < 3; ++row, ++total_row) {
				for (unsigned int col = 0; col < resource_dim(); ++col) {
					m_TaskJacobian(total_row,col) = (*m_Jacobians[i])(row+3, col);
				}
			}
		}

		CBF_DEBUG("m_ConcreteJacobian: " << m_TaskJacobian);
	
	}




	
	#ifdef CBF_HAVE_XSD
		BaseKDLChainSensorTransform::BaseKDLChainSensorTransform(const CBFSchema::ChainBase &xml_instance, const CBFSchema::SensorTransform &xml_st_instance, ObjectNamespacePtr object_namespace) :
			SensorTransform(xml_st_instance, object_namespace),
			m_Frame(new KDL::Frame),
			m_Jacobian(new KDL::Jacobian)
		{
			CBF_DEBUG("[KDLChainSensorTransform(const KDLChainSensorTransformType &xml_instance)]: yay!");
			m_Chain = create_chain(xml_instance, object_namespace);		
			init_solvers();
		}
		
		KDLChainPositionSensorTransform::KDLChainPositionSensorTransform(const CBFSchema::KDLChainPositionSensorTransform &xml_instance, ObjectNamespacePtr object_namespace) :
			BaseKDLChainSensorTransform(xml_instance.Chain(), xml_instance, object_namespace)
		{
			//! TODO: recheck this function to make sure it works in all cases..
		
			m_Result = FloatVector(3);
		}
		
		KDLChainAxisAngleSensorTransform::KDLChainAxisAngleSensorTransform(
			const CBFSchema::KDLChainAxisAngleSensorTransform &xml_instance, ObjectNamespacePtr object_namespace
		) :
			BaseKDLChainSensorTransform(xml_instance.Chain(), xml_instance, object_namespace)
		{
			//! TODO: recheck this function to make sure it works in all cases..
		
			m_Result = FloatVector(3);
		}


		BaseKDLTreeSensorTransform::BaseKDLTreeSensorTransform(const CBFSchema::TreeBase &xml_instance, const CBFSchema::SensorTransform &xml_st_instance, ObjectNamespacePtr object_namespace) :
			SensorTransform(xml_st_instance, object_namespace)
		{
			CBF_DEBUG("[KDLTreeSensorTransform(const KDLTreeSensorTransformType &xml_instance)]: yay!");

			m_Tree = 
				XMLObjectFactory::instance()->create<ForeignObject<KDL::Tree> >(
					xml_instance, object_namespace
				)->m_Object
			;

			CBF_DEBUG("tree created");

		}
		
		KDLTreePositionSensorTransform::KDLTreePositionSensorTransform(const CBFSchema::KDLTreePositionSensorTransform &xml_instance, ObjectNamespacePtr object_namespace) :
			BaseKDLTreeSensorTransform(xml_instance.Tree(), xml_instance, object_namespace)
		{

			for (
				CBFSchema::KDLTreePositionSensorTransform::SegmentName_const_iterator it = xml_instance.SegmentName().begin();
				it != xml_instance.SegmentName().end();
				++it
			) 
			{
				m_SegmentNames.push_back(*it);
			}

			init_solvers();

			m_Result = FloatVector(3 *  m_SegmentNames.size());

			m_TaskJacobian = FloatMatrix(
					(int) 3 * m_SegmentNames.size(),
					(int)  m_Tree->getNrOfJoints()
					);

		}
		
		KDLTreeAxisAngleSensorTransform::KDLTreeAxisAngleSensorTransform(
			const CBFSchema::KDLTreeAxisAngleSensorTransform &xml_instance, ObjectNamespacePtr object_namespace
		) :
			BaseKDLTreeSensorTransform(xml_instance.Tree(), xml_instance, object_namespace)
		{
			for (
				CBFSchema::KDLTreeAxisAngleSensorTransform::SegmentName_const_iterator it = xml_instance.SegmentName().begin();
				it != xml_instance.SegmentName().end();
				++it
			) 
			{
				m_SegmentNames.push_back(*it);
			}

			init_solvers();

			m_Result = FloatVector(3 *  m_SegmentNames.size());

			m_TaskJacobian = FloatMatrix(
					(int) 3 * m_SegmentNames.size(),
					(int)  m_Tree->getNrOfJoints()
					);

		}

		static XMLDerivedFactory<
			KDLChainPositionSensorTransform, 
			CBFSchema::KDLChainPositionSensorTransform
		> x1;

		static XMLDerivedFactory<
			KDLChainAxisAngleSensorTransform, 
			CBFSchema::KDLChainAxisAngleSensorTransform
		> x2;

		static XMLDerivedFactory<
			KDLTreePositionSensorTransform, 
			CBFSchema::KDLTreePositionSensorTransform
		> x3;

		static XMLDerivedFactory<
			KDLTreeAxisAngleSensorTransform, 
			CBFSchema::KDLTreeAxisAngleSensorTransform
		> x4;

	#endif

} // namespace

