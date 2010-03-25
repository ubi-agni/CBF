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

#ifndef CBF_KDL_TRANSFORMS_HH
#define CBF_KDL_TRANSFORMS_HH

#include <cbf/plugin_decl_macros.h>

#include <boost/shared_ptr.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include <sstream>
#include <iostream>
#include <map>
#include <string>

#include <cbf/sensor_transform.h>
#include <cbf/effector_transform.h>

#include <kdl/chain.hpp>

//! Forward declarations for stuff from KDL namespace that's only
//! used bu way of references
namespace KDL {
	class Chain;
	class JntArray;
	class Frame;
	class Jacobian;
	class ChainJntToJacSolver;
	class ChainFkSolverPos_recursive;
	class ChainFkSolverVel_recursive;
}

//! Forward declarations for XML instance types
// CBF_PLUGIN_PREAMBLE EffectorTransformType;
CBF_PLUGIN_PREAMBLE(KDLChainPositionSensorTransform)
CBF_PLUGIN_PREAMBLE(KDLChainOrientationQuatSensorTransform)
CBF_PLUGIN_PREAMBLE(KDLChainAxisAngleSensorTransform)

class ChainBaseType;

namespace CBF {
	namespace ublas = boost::numeric::ublas;
	
	/**
		@brief Abstract base class for KDL based transform classes.
	
		This class encapsulates the basic funcionality of the different concrete classes
		which implement different representations for the orientation part of the transform 
		(e.g. euler angles, quaternions, screws and twists).
	
		This is an abstract class and thus it cannot be instantiated directly..
	*/
	struct BaseKDLChainSensorTransform : public SensorTransform {
		protected:
			boost::shared_ptr<KDL::Chain> m_Chain;
			boost::shared_ptr<KDL::ChainJntToJacSolver> m_JacSolver;
			boost::shared_ptr<KDL::ChainFkSolverPos_recursive> m_FKSolver;
			boost::shared_ptr<KDL::ChainFkSolverVel_recursive> m_FKVelSolver;
	
			FloatMatrix m_Twists;
	
			//! Intermediate result
			boost::shared_ptr<KDL::Frame> m_Frame;
	
			//! Intermediate result
			boost::shared_ptr<KDL::Jacobian> m_Jacobian;
	
		public:
			/**
				Do not use instances of this class unless the chain has been set with a valid chain
				and init_solvers() was called. This should have been done by subclasses without
				any user intervention.
			*/
			BaseKDLChainSensorTransform
				(boost::shared_ptr<KDL::Chain> chain = boost::shared_ptr<KDL::Chain>());
	
			//! This constructor is only implemented when XSD support is enabled..
			BaseKDLChainSensorTransform(const ChainBaseType &xml_instance);
	
			/**
				For all derived types the resource dim is always the same: the number of joints
				of the kinematic chain
			*/
			virtual unsigned int resource_dim() const ;

			/**
				Call this function once m_Chain is set to a valid chain..
			*/
			virtual void init_solvers();
	
			/**
				This reads the current resource values and updates the KDL::Jacobian matrix..
			*/
			virtual void update();

			boost::shared_ptr<KDL::Chain> chain() { return m_Chain; }
	};
	
	
	
	
	/**
		@brief This class implements the SensorTransform for an arbitrary KDL chain. The task space is
		the position of the end effector of the chain.
	*/
	struct KDLChainPositionSensorTransform : public BaseKDLChainSensorTransform
	{
		CBF_PLUGIN_DECL_METHODS(KDLChainPositionSensorTransform)
	
		KDLChainPositionSensorTransform(boost::shared_ptr<KDL::Chain> chain = boost::shared_ptr<KDL::Chain>(new KDL::Chain));

		virtual unsigned int task_dim() const { return 3u; }

		virtual void update();
	};
	
	typedef boost::shared_ptr<KDLChainPositionSensorTransform> KDLChainPositionSensorTransformPtr;
	
	
	
	
	
	
	
	

	
	/**
		@brief This class implements the SensorTransform for an arbitrary KDL chain. The task space is
		the orientation of the end effector of the chain specified as quaternion
	*/
	struct KDLChainOrientationQuatSensorTransform : public BaseKDLChainSensorTransform
	{
		CBF_PLUGIN_DECL_METHODS(KDLChainOrientationQuatSensorTransform)
	
		protected:
			// FloatVector m_Result;
	
		public:
			KDLChainOrientationQuatSensorTransform(
				boost::shared_ptr<KDL::Chain> chain = 
					boost::shared_ptr<KDL::Chain>(new KDL::Chain)
			);
		

			virtual unsigned int task_dim() const { return 4u; }

			virtual void update();
	};

	typedef boost::shared_ptr<KDLChainOrientationQuatSensorTransform> KDLChainOrientationQuatSensorTransformPtr;
	
	
	
	
	
	
	

	
	
	/**
		@brief This class implements the SensorTransform for an arbitrary KDL chain. The task space is
		the orientation of the end effector of the chain specified in compact axis-angle representation
		(compact meaning here that the angle is absorbed into the length of the axid vector resulting
		in a 3D representation.
	*/
	struct KDLChainAxisAngleSensorTransform : public BaseKDLChainSensorTransform
	{
		CBF_PLUGIN_DECL_METHODS(KDLChainAxisAngleSensorTransform)
	
		KDLChainAxisAngleSensorTransform(
			boost::shared_ptr<KDL::Chain> chain =
				boost::shared_ptr<KDL::Chain>(new KDL::Chain)
		);
		
		virtual unsigned int task_dim() const { return 3u; }

		virtual void update();
	};
	
	typedef boost::shared_ptr<KDLChainAxisAngleSensorTransform> KDLChainAxisAngleSensorTransformPtr;	

} // namespace

#endif
