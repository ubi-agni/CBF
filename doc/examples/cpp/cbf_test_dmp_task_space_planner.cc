
#include <cbf/primitive_controller.h>
#include <cbf/quaternion_potential.h>
#include <cbf/cddyn_filter.h>
#include <cbf/dmp_filter.h>
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

  mTargetReference = CBF::DummyReferencePtr(new CBF::DummyReference(1,N_REF));

  CBF::SquarePotentialPtr potential = CBF::SquarePotentialPtr(new CBF::SquarePotential(3,3));

  std::vector<FloatVector > dmp_basis_weights;

  dmp_basis_weights.resize(5,FloatVector(3));

  dmp_basis_weights[0] << -146.3034, -143.4989, -166.9290;
  dmp_basis_weights[1] <<  -45.8134,   88.0299, -187.8729;
  dmp_basis_weights[2] <<  122.0822,  614.6215, -103.4508;
  dmp_basis_weights[3] <<   46.6573,  475.2182,  -28.5828;
  dmp_basis_weights[4] <<    6.8930,   71.8078,    0.5606;

  CBF::DMPFilterPtr task_filter = CBF::DMPFilterPtr(new CBF::DMPFilter(N_DT,
                                                                       3,
                                                                       3,
                                                                       dmp_basis_weights,
                                                                       1.5873, //tau
                                                                       20.0,   //alpha_movement,
                                                                       5.0,    //beta_movement,
                                                                       1.5     //alpha_phase
                                                                       ));

  // controller
  CBF::PrimitiveControllerPtr controller (
      new CBF::PrimitiveController(
        N_DT,
        std::vector<CBF::ConvergenceCriterionPtr>(),
        mTargetReference,
        task_filter,
        potential,
        PDPositionControlPtr(new PDPositionControl(N_DT, potential->task_dim())),
        CBF::SensorTransformPtr(new CBF::KDLChainPositionSensorTransform(chain)),
        CBF::EffectorTransformPtr(new CBF::DampedGenericEffectorTransform(potential->task_dim(), nJoints)),
        std::vector<CBF::SubordinateControllerPtr>(),
        CBF::CombinationStrategyPtr(new CBF::AddingStrategy),
        CBF::DummyResourcePtr(new CBF::DummyResource(nJoints)),
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

  lJoint.setOnes();
  mController->reset(lJoint*0.2, lJoint*0.0);

  lRef = mController->sensor_transform()->result();
  std::cout << "Initial position" << std::endl;
  std::cout << lRef << std::endl;

  lRef(0) -= 0.1;
  lRef(1) -= 0.1;
  lRef(2) -= 0.1;

  std::cout << "Target position" << std::endl;
  std::cout << lRef << std::endl;

  mTargetReference->set_reference(lRef);

  FloatVector lEndPosture(3);

  int cnt=0;
  do {
    mController->step();
    lEndPosture = mController->sensor_transform()->result();

    //lEndPosture = mController->task_filter()->get_filtered_state();
    std::cout << "step " << cnt++ << ": "
               << lEndPosture[0] << " "
               << lEndPosture[1] << " "
               << lEndPosture[2] << " "
               << std::endl;

    usleep(10000);
  } while(mController->finished() == false);
}

