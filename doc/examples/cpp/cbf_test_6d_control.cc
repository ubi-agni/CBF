
#include <cbf/primitive_controller.h>
#include <cbf/quaternion_potential.h>
#include <cbf/cddyn_filter.h>
#include <cbf/bypass_filter.h>
#include <cbf/error_controllers.h>

#include <cbf/dummy_resource.h>
#include <cbf/dummy_reference.h>
#include <cbf/kdl_transforms.h>
#include <cbf/generic_transform.h>
#include <cbf/transpose_transform.h>
#include <cbf/convergence_criterion.h>
#include <cbf/combination_strategy.h>
#include <cbf/generic_limiters.h>

#include <cbf/square_potential.h>
#include <cbf/composite_potential.h>
#include <cbf/composite_transform.h>
#include <cbf/composite_reference.h>
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

#define N_REF (3+4)
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

  std::vector<CBF::PotentialPtr> potentials = boost::assign::list_of
    (CBF::PotentialPtr(new CBF::SquarePotential(3,3)))
    (CBF::PotentialPtr(new CBF::QuaternionPotential()));
  CBF::PotentialPtr potential (new CBF::CompositePotential(potentials));

  mVirtualResource = CBF::DummyResourcePtr(new CBF::DummyResource(nJoints));
  mTargetReference = CBF::DummyReferencePtr(new CBF::DummyReference(1, N_REF));

  CBF::CDDynFilterPtr reference_filter = CBF::CDDynFilterPtr(new CBF::CDDynFilter(N_DT, potential->sensor_dim(), potential->task_dim(), 1.0));

  std::vector<CBF::SensorTransformPtr> sensor_transforms = boost::assign::list_of
    (CBF::SensorTransformPtr(new CBF::KDLChainPositionSensorTransform(chain)))
    (CBF::SensorTransformPtr(new CBF::KDLChainQuaternionSensorTransform(chain)));

  CBF::SensorTransformPtr sensor_transform (new CBF::CompositeSensorTransform(sensor_transforms));

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
        std::vector<CBF::SubordinateControllerPtr>(),
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
  FloatVector lQuat(4);
  FloatVector lJoint(mChain->getNrOfJoints());
  FloatVector lEndPosture(N_REF);

  // initialize resource
  lJoint.setOnes();
  mVirtualResource->set(lJoint*0.2, lJoint*0.0);

  // initialize controller
  mController->reset(mVirtualResource->get_position(), mVirtualResource->get_velocity());

  std::cout << "Initial resource" << std::endl;
  std::cout << mController->resource()->get_position() << std::endl;

  lRef = mController->sensor_transform()->result();
  std::cout << "Initial reference" << std::endl;
  std::cout << lRef << std::endl;

  std::cout << "Initial sensor dim" << std::endl;
  std::cout << mController->sensor_transform()->sensor_dim() << std::endl;
  std::cout << mController->sensor_transform()->task_dim() << std::endl;


  // set arbitrary reference
  lRef(0) += 0.1;
  lRef(1) += 0.1;
  lRef(2) -= 0.2;

  lQuat(0) = lRef(3)+0.01;
  lQuat(1) = lRef(4)-0.02;
  lQuat(2) = lRef(5)+0.03;
  lQuat(3) = lRef(6)-0.04;

  lQuat = lQuat/lQuat.norm();
  for(int i=0; i<4; i++) lRef(3+i) = lQuat(i);

  std::cout << "Target reference" << std::endl;
  std::cout << lRef << std::endl;

  mTargetReference->set_reference(lRef);

  int cnt=0;
  do {
    // control
    mController->step();

    // display
    lEndPosture = mController->sensor_transform()->result();

    std::cout << "step " << cnt++ << ": ";
    for(int i=0; i<N_REF; i++)
    {
      std::cout << lEndPosture[i] << " ";
    }
    std::cout << std::endl;

    usleep(N_DT*1000000);
  } while(mController->finished() == false);
}

