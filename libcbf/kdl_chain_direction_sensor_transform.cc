#include <cbf/kdl_chain_direction_sensor_transform.h>

#include <cbf/kdl_transforms.h>

#include <cbf/xml_object_factory.h>
#include <cbf/foreign_object.h>

#include <kdl/chain.hpp>
#include <kdl/tree.hpp>
#include <kdl/jntarray.hpp>
#include <kdl/frames.hpp>

namespace CBF {


KDLChainDirectionSensorTransform::KDLChainDirectionSensorTransform(boost::shared_ptr<KDL::Chain> chain) :
  BaseKDLChainSensorTransform(chain)
{
  m_SensorDim = 3;
  m_TaskDim = 3;
  m_ResourceDim = chain->getNrOfJoints();

  this->resize_variables(m_SensorDim, m_TaskDim, m_ResourceDim);

  mLocalControlDirection(0) = 0.0;
  mLocalControlDirection(1) = 0.0;
  mLocalControlDirection(2) = 1.0;
}

void KDLChainDirectionSensorTransform::update(const FloatVector &resource_value) {

  BaseKDLChainSensorTransform::update(resource_value);

  unsigned int idxJoint;
  unsigned int lSegmentNr = m_Chain->getNrOfSegments();
  KDL::Frame lFrameAll = KDL::Frame::Identity();
  //KDL::Frame lFrameSeg = KDL::Frame::Identity();

  KDL::Vector lVJointAxis;

  mCurrentControlDirection = m_Frame->M * mLocalControlDirection;
  mCurrentControlDirection.Normalize();

  if(resource_value.rows()!=m_Chain->getNrOfJoints())
  {

  }
  else{

    idxJoint = 0;
    for(unsigned int idxSeg=0; idxSeg<lSegmentNr; idxSeg++)
    {

      if(m_Chain->getSegment(idxSeg).getJoint().getType()!= KDL::Joint::None){

        //lFrameSeg = m_Chain->getSegment(idxSeg).getJoint().pose(resource_value(idxJoint));
        //lVJointAxis = lFrameAll * lFrameSeg * m_Chain->getSegment(idxSeg).getJoint().JointAxis();
        lVJointAxis = lFrameAll.M * m_Chain->getSegment(idxSeg).getJoint().JointAxis();
        lVJointAxis.Normalize();

        lVJointAxis = lVJointAxis*mCurrentControlDirection;

        for (unsigned int i=0; i<3; i++) {
            m_TaskJacobian(i,idxJoint) = lVJointAxis.data[i];
        }

        lFrameAll = lFrameAll*m_Chain->getSegment(idxSeg).pose(resource_value(idxJoint));
        idxJoint++;

      }
      else
      {
          lFrameAll = lFrameAll*m_Chain->getSegment(idxSeg).pose(0.0);
      }
    }

  }

  for (unsigned int i=0; i<3; i++) {
      m_Result(i) = mCurrentControlDirection(i);
  }

}

void KDLChainDirectionSensorTransform::setLocalControlDirection(const KDL::Vector &control_dir)
{
  mLocalControlDirection = control_dir;
}

void KDLChainDirectionSensorTransform::setLocalControlDirection(const double x, const double y, const double z)
{
  mLocalControlDirection.data[0] = x;
  mLocalControlDirection.data[1] = y;
  mLocalControlDirection.data[2] = z;
}


// name space
}
