
#include <cbf/primitive_controller.h>
#include <cbf/quaternion_potential.h>
#include <cbf/cddyn_filter.h>
#include <cbf/pid_filter.h>

#include <cbf/dummy_resource.h>
#include <cbf/dummy_reference.h>
#include <cbf/kdl_transforms.h>
#include <cbf/generic_transform.h>
#include <cbf/transpose_transform.h>
#include <cbf/convergence_criterion.h>
#include <cbf/combination_strategy.h>

#include <cbf/square_potential.h>
#include <cbf/composite_potential.h>
#include <cbf/composite_transform.h>
#include <cbf/kdl_chain_direction_sensor_transform.h>
#include <cbf/direction_potential.h>
#include <boost/assign/list_of.hpp>


#include <boost/shared_ptr.hpp>
#include <kdl/chain.hpp>
#include <algorithm>

using namespace KDL;
using namespace std;
using namespace boost;
using namespace CBF;


CBF::DummyReferencePtr mTargetReference;
CBF::PrimitiveControllerPtr createController (boost::shared_ptr<KDL::Chain> chain);
boost::shared_ptr<KDL::Chain> createChain (int iNumJointTriples);


Chain createKDLChain() {
  Chain chain;

  /// base segment (without mounting plate!!!)
  Segment base = Segment(Joint(Joint::None),
      Frame(Rotation(), Vector(0.0, 0.0, 0.118))); //11
  chain.addSegment(base);

  /// joint A1
  Segment a1 = Segment(Joint(Joint::RotZ),
      Frame(Rotation::RotZ(M_PI), Vector(0.0, 0.0, 0.192))); //20
  chain.addSegment(a1);

  /// joint A2
  Segment a2 = Segment(Joint(Joint::RotY),
      Frame(Rotation(), Vector(0.0, 0.0, 0.208))); //20
  chain.addSegment(a2);

  /// joint E1
  Segment e1 = Segment(Joint(Joint::RotZ),
      Frame(Rotation::RotZ(M_PI), Vector(0.0, 0.0, 0.192))); //20
  chain.addSegment(e1);

  /// joint A3
  Segment a3 = Segment(Joint(Joint::RotY),
      Frame(Rotation(), Vector(0.0, 0.0, 0.208))); //20
  chain.addSegment(a3);

  /// joint A4
  Segment a4 = Segment(Joint(Joint::RotZ),
      Frame(Rotation::RotZ(M_PI), Vector(0.0, 0.0, 0.182))); //19
  chain.addSegment(a4);

  /// joint A5
  Segment a5 = Segment(Joint(Joint::RotY),
      Frame(Rotation(), Vector(0.0, 0.0, 0.0)));
  chain.addSegment(a5);

  /// joint A6
  Segment a6 = Segment(Joint(Joint::RotZ),
      Frame(Rotation::RotZ(-M_PI), Vector(0.0, 0.0, 0.12)));
  chain.addSegment(a6);

  return chain;
}



CBF::PrimitiveControllerPtr createController (boost::shared_ptr<KDL::Chain> chain)
{
  unsigned int nJoints = chain->getNrOfJoints();
  unsigned int nRef = 4;
  Float dt = 1./100.;

  mTargetReference = CBF::DummyReferencePtr(new CBF::DummyReference(1,nRef));

  CBF::QuaternionPotentialPtr potential = CBF::QuaternionPotentialPtr(new CBF::QuaternionPotential());

  //CBF::PIDFilterPtr task_filter = CBF::PIDFilterPtr(new CBF::PIDFilter(dt, 3, 3));
  //task_filter->set_gain(1.0, 0.0, 1.5);
  CBF::CDDynFilterPtr task_filter = CBF::CDDynFilterPtr(new CBF::CDDynFilter(dt, 3, 3, 2.0));

  // controller
  CBF::PrimitiveControllerPtr controller (
      new CBF::PrimitiveController(
        dt,
        std::vector<CBF::ConvergenceCriterionPtr>(),
        mTargetReference,
        potential,
        task_filter,
        CBF::SensorTransformPtr(new CBF::KDLChainQuaternionSensorTransform(chain)),
        CBF::EffectorTransformPtr(new CBF::DampedGenericEffectorTransform(3, nJoints)),
        std::vector<CBF::SubordinateControllerPtr>(),
        CBF::CombinationStrategyPtr(new CBF::AddingStrategy),
        CBF::DummyResourcePtr(new CBF::DummyResource(nJoints))
      )
  );

  return controller;
}

int main() {

  KDL::Chain chain = createKDLChain();
  boost::shared_ptr<KDL::Chain> mChain = boost::shared_ptr<KDL::Chain>(new KDL::Chain(chain));

  PrimitiveControllerPtr mController;
  mController = createController(mChain);

  FloatVector lRef(mController->reference()->dim());
  FloatVector lJoint(mChain->getNrOfJoints());

  lJoint.setOnes();
  mController->resource()->update(lJoint*0.2, lJoint*0.0);
  mController->sensor_transform()->update(mController->resource()->get());

  lRef = mController->sensor_transform()->result();
  std::cout << "Initial Quaternion" << std::endl;
  std::cout << lRef << std::endl;

  lRef(0) += 0.2;
  lRef(1) += 0.2;
  lRef(2) -= 0.2;
  lRef(3) -= 0.2;
  lRef = lRef/lRef.norm();

  std::cout << "Target Quaternion" << std::endl;
  std::cout << lRef << std::endl;

  mTargetReference->set_reference(lRef);

  mController->reset();

  //mController->planner()->update(mTargetReference->get());

  FloatVector lEndPosture(4);

  int cnt=0;
  do {
    mController->step();
    lEndPosture = mController->sensor_transform()->result();
    //lEndPosture = mController->task_filter()->get_filtered_state();

    std::cout << "step " << cnt++ << ": "
               << lEndPosture[0] << " "
               << lEndPosture[1] << " "
               << lEndPosture[2] << " "
               << lEndPosture[3] << " "
               << std::endl;

    usleep(10000);
  } while(mController->finished() == false);
}

