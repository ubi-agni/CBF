
#include <stdio.h>
#include <cbf/primitive_controller.h>
#include <cbf/quaternion_potential.h>
#include <cbf/cddyn_filter.h>
#include <cbf/bypass_filter.h>
#include <cbf/error_controllers.h>

#include <cbf/dummy_resource.h>
#include <cbf/dummy_reference.h>
#include <cbf/kdl_transforms.h>
#include <cbf/generic_transform.h>
#include <cbf/identity_transform.h>
#include <cbf/convergence_criterion.h>
#include <cbf/combination_strategy.h>
#include <cbf/generic_limiters.h>

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

#define N_REF 3
#define N_DT (1./100.)

CBF::DummyReferencePtr mTargetReference;
CBF::DummyResourcePtr mVirtualResource;
CBF::PrimitiveControllerPtr createController (boost::shared_ptr<KDL::Chain> chain);

boost::shared_ptr<KDL::Chain> createChain (int iNumJointTriples);


Chain createKDLChain() {
  Chain chain;

  Segment base = Segment(Joint(Joint::None),
      Frame(Rotation(), Vector(0.0, 0.0, 0.055)));
  chain.addSegment(base);

  Segment a1 = Segment(Joint(Joint::RotZ),
      Frame(Rotation(), Vector(0.0, 0.0, 0.11)));
  chain.addSegment(a1);

  Segment a2 = Segment(Joint(Joint::RotY, -1.),
      Frame(Rotation(), Vector(0.0, 0.0, 0.20)));
  chain.addSegment(a2);

  Segment a3 = Segment(Joint(Joint::RotZ),
      Frame(Rotation(), Vector(0.0, 0.0, 0.2)));
  chain.addSegment(a3);

  Segment a4 = Segment(Joint(Joint::RotY),
      Frame(Rotation(), Vector(0.0, 0.0, 0.2)));
  chain.addSegment(a4);

  Segment a5 = Segment(Joint(Joint::RotZ),
      Frame(Rotation(), Vector(0.0, 0.0, 0.2)));
  chain.addSegment(a5);

  Segment a6 = Segment(Joint(Joint::RotY, -1.),
      Frame(Rotation(), Vector(0.0, 0.0, 0.19)));
  chain.addSegment(a6);

  Segment a7 = Segment(Joint(Joint::RotZ),
      Frame(Rotation(), Vector(0.0, 0.0, 0.0)));
  chain.addSegment(a7);

  Segment effector = Segment(Joint(Joint::None),
      Frame(Rotation(), Vector(0.1, 0.1, 0.1)));
  chain.addSegment(effector);

  return chain;
}



CBF::PrimitiveControllerPtr createController (boost::shared_ptr<KDL::Chain> chain)
{
  unsigned int nJoints = chain->getNrOfJoints();

  CBF::DummyReferencePtr null_reference = CBF::DummyReferencePtr(new CBF::DummyReference(1, nJoints));
  CBF::SquarePotentialPtr null_potential = CBF::SquarePotentialPtr(new CBF::SquarePotential(nJoints, nJoints));
  CBF::CDDynFilterPtr null_reference_filter = CBF::CDDynFilterPtr(new CBF::CDDynFilter(N_DT, nJoints, nJoints, 5.0));

  FloatVector lRef(nJoints);
  lRef.setOnes(nJoints);
  lRef = lRef*0.3;
  null_reference->set_reference(lRef);
  std::cout << "Null space Reference" << std::endl;
  std::cout << lRef << std::endl;

  // null space controller
  CBF::SubordinateControllerPtr null_ontroller (
      new CBF::SubordinateController(
        N_DT,
        std::vector<CBF::ConvergenceCriterionPtr>(),
        null_reference,
        null_reference_filter,
        null_potential,
        PDPositionControlPtr(new PDPositionControl(N_DT, null_potential->task_dim())),
        CBF::SensorTransformPtr(new CBF::IdentitySensorTransform(nJoints)),
        CBF::EffectorTransformPtr(new CBF::IdentityEffectorTransform(nJoints)),
        std::vector<CBF::SubordinateControllerPtr>(),
        CBF::CombinationStrategyPtr(new CBF::AddingStrategy)
      )
  );

  std::vector<CBF::SubordinateControllerPtr > null_controllers;
  null_controllers.push_back(null_ontroller);


  // main primitive controller
  mVirtualResource = CBF::DummyResourcePtr(new CBF::DummyResource(nJoints));
  mTargetReference = CBF::DummyReferencePtr(new CBF::DummyReference(1, N_REF));

  CBF::SquarePotentialPtr potential = CBF::SquarePotentialPtr(new CBF::SquarePotential(N_REF, N_REF));

  CBF::CDDynFilterPtr reference_filter = CBF::CDDynFilterPtr(new CBF::CDDynFilter(N_DT, potential->sensor_dim(), potential->task_dim(), 2.0));

  CBF::SensorTransformPtr sensor_transform = CBF::SensorTransformPtr(new CBF::KDLChainPositionSensorTransform(chain));

  // controller
  CBF::PrimitiveControllerPtr controller (
      new CBF::PrimitiveController(
        N_DT,
        std::vector<CBF::ConvergenceCriterionPtr>(),
        mTargetReference,
        reference_filter,
        potential,
        PDPositionControlPtr(new PDPositionControl(N_DT, potential->task_dim())),
        sensor_transform,
        CBF::EffectorTransformPtr(new CBF::GenericEffectorTransform(potential->task_dim(), nJoints)),
        null_controllers,
        CBF::CombinationStrategyPtr(new CBF::AddingStrategy),
        mVirtualResource,
        CBF::BypassFilterPtr(new CBF::BypassFilter(N_DT, nJoints, nJoints)),
        CBF::NullLimiterPtr(new CBF::NullLimiter(N_DT, nJoints))
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

  // initialize resource
  lJoint.setOnes();
  mVirtualResource->set(lJoint*0.2, lJoint*0.0);

  // initialize controller
  mController->reset(mVirtualResource->get_position(), mVirtualResource->get_velocity());

  std::cout << "Initial resource" << std::endl;
  std::cout << mController->resource()->get_position() << std::endl;

  lRef = mController->sensor_transform()->result();
  std::cout << "Initial Reference" << std::endl;
  std::cout << lRef << std::endl;

  lRef(0) += 0.2;
  lRef(1) += 0.2;
  lRef(2) -= 0.2;

  std::cout << "Target Reference" << std::endl;
  std::cout << lRef << std::endl;

  mTargetReference->set_reference(lRef);

  FloatVector lEndPosture(3);
  int cnt=0;
  do {
    mController->step();

    lEndPosture = mController->sensor_transform()->result();

    printf("step: %04d, task-space error: %8.4lf\n", cnt++, (lRef-lEndPosture).norm());

/*
    std::cout << "Task Reference velocity" << std::endl;
    std::cout << mController->reference_filter()->get_filtered_state_vel() << std::endl;

    std::cout << "Task Jacobian" << std::endl;
    std::cout << mController->sensor_transform()->task_jacobian() << std::endl;

    std::cout << "Inverse Jacobian" << std::endl;
    std::cout << mController->effector_transform()->inverse_task_jacobian() << std::endl;

    std::cout << mController->result_resource_velocity() << std::endl;
    std::cout << mController->effector_transform()->inverse_task_jacobian() *  mController->reference_filter()->get_filtered_state_vel() << std::endl;

    std::cout << "Null space sensor" << std::endl;
    std::cout << mController->subordinate_controllers()[0]->sensor_transform()->result() << std::endl;

    std::cout << "Null space reference" << std::endl;
    std::cout << mController->subordinate_controllers()[0]->reference_filter()->get_filtered_state() << std::endl;

    std::cout << "Resource" << std::endl;
    std::cout << mController->resource()->get() << std::endl;

    std::cout << "mController->result_resource_velocity()" << std::endl;
    std::cout << mController->result_resource_velocity() << std::endl;
*/
    usleep(10000);
  } while(mController->finished() == false);
}
