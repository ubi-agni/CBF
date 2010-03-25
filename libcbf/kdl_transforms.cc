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
#include <cbf/plugin_macros.h>

#include <kdl/chain.hpp>
#include <kdl/jntarray.hpp>
#include <kdl/frames.hpp>
#include <kdl/chainiksolvervel_pinv.hpp>
#include <kdl/chainjnttojacsolver.hpp>
#include <kdl/chainfksolverpos_recursive.hpp>
//#include <kdl/utilities/svd_boost_HH.hpp>
#include <kdl/chainfksolvervel_recursive.hpp>
#include <kdl/jntarrayvel.hpp>

namespace CBF {
	
	
	BaseKDLChainSensorTransform::BaseKDLChainSensorTransform(boost::shared_ptr<KDL::Chain> chain) :
		m_Chain(chain),
		m_Frame(new KDL::Frame),
		m_Jacobian(new KDL::Jacobian)
	{
		//init_solvers();
	}
	
	void BaseKDLChainSensorTransform::init_solvers() {
	
		m_FKVelSolver = boost::shared_ptr<KDL::ChainFkSolverVel_recursive>(
			new KDL::ChainFkSolverVel_recursive(*m_Chain)
		);
	
		m_Twists = FloatMatrix(6, m_Chain->getNrOfJoints());
	
		m_JacSolver = boost::shared_ptr<KDL::ChainJntToJacSolver>(
			new KDL::ChainJntToJacSolver(*m_Chain)
		);
	
		m_FKSolver = boost::shared_ptr<KDL::ChainFkSolverPos_recursive>(
			new KDL::ChainFkSolverPos_recursive(*m_Chain)
		);
	
		m_Jacobian = boost::shared_ptr<KDL::Jacobian>(new KDL::Jacobian(m_Chain->getNrOfJoints()));
	}
	
	void BaseKDLChainSensorTransform::update() {
		//SensorTransform::update();
	
		KDL::JntArray jnt_array(resource_dim());
	
		//const ublas::vector<Float> &resource = m_Resource->get();
	
		CBF_DEBUG(m_Resource->get())
	
		for (unsigned int i = 0; i < resource_dim(); ++i) {
			jnt_array(i) = m_Resource->get()[i];
		}
	
		m_JacSolver->JntToJac(jnt_array, *m_Jacobian);
		m_FKSolver->JntToCart(jnt_array, *m_Frame);
	
	#if 0
		for (unsigned int i = 0; i < get_resource_dim(); ++i) {
	
			//for (unsigned int j = 0; j < get_resource_dim(); ++j)
			//	jnt_array(i) = 0;
	
			KDL::FrameVel vel;
			//jnt_array(i) = 1.0;
			KDL::Twist twist;
			m_FKVelSolver->JntToCart(jnt_array, vel);
			twist = vel.GetTwist();
			for (unsigned int j = 0; j < 6; ++j) {
				m_Twists(j,i) = twist(j);
				CBF_DEBUG(twist(j))
			}
		}
		CBF_DEBUG(m_Twists)
	#endif
		
	}
	
	unsigned int BaseKDLChainSensorTransform::resource_dim() const {
		return m_Chain->getNrOfJoints();
	}
	
	
	
	KDLChainPositionSensorTransform::KDLChainPositionSensorTransform(boost::shared_ptr<KDL::Chain> chain) :
		BaseKDLChainSensorTransform(chain)
	{
		m_Result = ublas::vector<Float>(3);
		init_solvers();
	}
	
	void KDLChainPositionSensorTransform::update() {
		BaseKDLChainSensorTransform::update();
	
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
	
	KDLChainOrientationQuatSensorTransform::KDLChainOrientationQuatSensorTransform(boost::shared_ptr<KDL::Chain> chain) :
		BaseKDLChainSensorTransform(chain)
	{
		m_Result = ublas::vector<Float>(3);
		init_solvers();
	}
	void KDLChainOrientationQuatSensorTransform::update() {
		BaseKDLChainSensorTransform::update();
	
		CBF_DEBUG("Updating Jacobian")
		//! Update the jacobian so we can hand it out...
		m_TaskJacobian = FloatMatrix(4, resource_dim());
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
		m_Result = ublas::vector<Float>(3);
		init_solvers();
	}
	
	void KDLChainAxisAngleSensorTransform::update() {
		BaseKDLChainSensorTransform::update();
	
		CBF_DEBUG("Updating Jacobian")
		//! Update the jacobian so we can hand it out...
		m_TaskJacobian = ublas::zero_matrix<Float>(3, resource_dim());
	
		//CBF_DEBUG(m_ConcreteJacobian)
		//CBF_DEBUG(get_resource_dim())
	
		for (unsigned int i = 0; i < 3; ++i) {
			for (unsigned int j = 0; j < resource_dim(); ++j) {
				m_TaskJacobian(i,j) = (*m_Jacobian)(i+3, j);
				//CBF_DEBUG("la! " << m_ConcreteJacobian(i,j))
			}
		}
	
	#if 0
		for (unsigned int i = 0; i < 3; ++i) {
			for (unsigned int j = 0; j < resource_dim(); ++j) {
				CBF_DEBUG(m_ConcreteJacobian(i,j));
			}
		}
	#endif
	
		CBF_DEBUG("m_ConcreteJacobian: " << m_TaskJacobian)
	
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
	
	
	#ifdef CBF_HAVE_XSD
		BaseKDLChainSensorTransform::BaseKDLChainSensorTransform(const ChainBaseType &xml_instance) :
			m_Frame(new KDL::Frame),
			m_Jacobian(new KDL::Jacobian)
		{
			CBF_DEBUG("[KDLChainSensorTransform(const KDLChainSensorTransformType &xml_instance)]: yay!")
			//! Check what kind of chain we have:
		
			const ChainType *chain_instance  = dynamic_cast<const ChainType*>(&xml_instance);
		
			if (chain_instance == 0)
				throw std::runtime_error("[KDLChainSensorTransform]: Chain type not handled yet..");
		
		
			m_Chain = boost::shared_ptr<KDL::Chain>(new KDL::Chain());
		
			for (
				ChainType::Segment_const_iterator it =(*chain_instance).Segment().begin(); 
				it != (*chain_instance).Segment().end();
				++it
			)
			{
				CBF_DEBUG("[KDLChainSensorTransform]: Adding Segment...")
		
				KDL::Frame frame;
				KDL::Joint joint;
		
				const MatrixFrameType *matrix_frame_instance = dynamic_cast<const MatrixFrameType*>(&(*it).Frame());
		
				if (matrix_frame_instance != 0)
				{
					CBF_DEBUG("[KDLChainSensorTransform]: Extracting matrix...")
		
					FloatMatrix m;
					m = create_matrix((*matrix_frame_instance).Matrix());
		
					if (m.size1() != 4 || m.size2() != 4)
						throw std::runtime_error("[KDLChainSensorTransform]: Matrix is not 4x4");
		
					for (int row = 0; row < 3; ++row)
					{
						for (int col = 0; col < 3; ++col)
						{
							frame.M(row, col) = m(row, col);
						}
					}
		
					for (int row = 0; row < 3; ++row)
						frame.p(row) = m(row, 3);
				}
				else
					throw std::runtime_error("[KDLChainSensorTransform]: Frame type not supported yet..");
		
				CBF_DEBUG("[KDLChainSensorTransform]: Extracting joint...")
		
				const JointType joint_instance = (*it).Joint();
				if (joint_instance.Type() == "Rotational") {
					CBF_DEBUG("[KDLChainSensorTransform]: Extracting rotational joint...")
					if (joint_instance.Axis() == "X") {
						CBF_DEBUG("X")
						joint = KDL::Joint(KDL::Joint::RotX);
					}
					if (joint_instance.Axis() == "Y") {
						CBF_DEBUG("Y")
						joint = KDL::Joint(KDL::Joint::RotY);
					}
					if (joint_instance.Axis() == "Z") {
						CBF_DEBUG("Z")
						joint = KDL::Joint(KDL::Joint::RotZ);
					}
				}
		
				if (joint_instance.Type() == "Translational") {
					throw std::logic_error("[KDLChainSensorTransform]: Translational joints not supported yet. TODO: fix this :)");
				}
		
				CBF_DEBUG("[KDLChainSensorTransform]: Adding Segment for real..")
				m_Chain->addSegment(KDL::Segment(joint, frame));
				CBF_DEBUG("[KDLChainSensorTransform]: number of joints: " << m_Chain->getNrOfJoints())
			}
		
			init_solvers();
		}
		
		KDLChainPositionSensorTransform::KDLChainPositionSensorTransform(const KDLChainPositionSensorTransformType &xml_instance) :
			BaseKDLChainSensorTransform(xml_instance.Chain())
		{
			//! TODO: recheck this function to make sure it works in all cases..
		
			m_Result = FloatVector(3);
		}
		
		KDLChainOrientationQuatSensorTransform::KDLChainOrientationQuatSensorTransform(
			const KDLChainOrientationQuatSensorTransformType &xml_instance
		) :
			BaseKDLChainSensorTransform(xml_instance.Chain())
		{
			//! TODO: recheck this function to make sure it works in all cases..
		
			m_Result = FloatVector(4);
		}
		
		KDLChainAxisAngleSensorTransform::KDLChainAxisAngleSensorTransform(
			const KDLChainAxisAngleSensorTransformType &xml_instance
		) :
			BaseKDLChainSensorTransform(xml_instance.Chain())
		{
			//! TODO: recheck this function to make sure it works in all cases..
		
			m_Result = FloatVector(3);
		}
	#endif
	
	CBF_PLUGIN_CLASS(KDLChainPositionSensorTransform, SensorTransform)
	CBF_PLUGIN_CLASS(KDLChainAxisAngleSensorTransform, SensorTransform)
	CBF_PLUGIN_CLASS(KDLChainOrientationQuatSensorTransform, SensorTransform)
} // namespace

