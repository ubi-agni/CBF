/*
 * Testing lwr pos+quat control
 *
 *  Created on: 02.02.2021
 *      Author: Guillaume WALCK
 */

#include <boost/assign/list_of.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <cbf/bypass_filter.h>
#include <cbf/composite_potential.h>
#include <cbf/composite_transform.h>
#include <cbf/dummy_reference.h>
#include <cbf/dummy_resource.h>
#include <cbf/error_controllers.h>
#include <cbf/generic_limiters.h>
#include <cbf/generic_transform.h>
#include <cbf/identity_transform.h>
#include <cbf/kdl_transforms.h>
#include <cbf/primitive_controller.h>
#include <cbf/quaternion_potential.h>
#include <cbf/square_potential.h>
#include <kdl/chain.hpp>
#include <kdl/segment.hpp>

using namespace KDL;
using namespace std;
using namespace boost;
using namespace CBF;

Chain createKDLChain() {
  Chain chain;

  /// base segment (without mounting plate!!!)
  Segment base = Segment(Joint(Joint::None),
                         Frame(Rotation(), Vector(0.0, 0.0, 0.118))); // 11
  chain.addSegment(base);

  /// joint A1
  Segment a1 =
      Segment(Joint(Joint::RotZ),
              Frame(Rotation::RotZ(M_PI), Vector(0.0, 0.0, 0.192))); // 20
  chain.addSegment(a1);

  /// joint A2
  Segment a2 = Segment(Joint(Joint::RotY),
                       Frame(Rotation(), Vector(0.0, 0.0, 0.208))); // 20
  chain.addSegment(a2);

  /// joint E1
  Segment e1 =
      Segment(Joint(Joint::RotZ),
              Frame(Rotation::RotZ(M_PI), Vector(0.0, 0.0, 0.192))); // 20
  chain.addSegment(e1);

  /// joint A3
  Segment a3 = Segment(Joint(Joint::RotY),
                       Frame(Rotation(), Vector(0.0, 0.0, 0.208))); // 20
  chain.addSegment(a3);

  /// joint A4
  Segment a4 =
      Segment(Joint(Joint::RotZ),
              Frame(Rotation::RotZ(M_PI), Vector(0.0, 0.0, 0.182))); // 19
  chain.addSegment(a4);

  /// joint A5
  Segment a5 =
      Segment(Joint(Joint::RotY), Frame(Rotation(), Vector(0.0, 0.0, 0.0)));
  chain.addSegment(a5);

  /// joint A6
  Segment a6 = Segment(Joint(Joint::RotZ),
                       Frame(Rotation::RotZ(-M_PI), Vector(0.0, 0.0, 0.12)));
  chain.addSegment(a6);

  return chain;
}

int main() {

  auto lwrchain = boost::make_shared<Chain>(createKDLChain());
  unsigned int nJoints = lwrchain->getNrOfJoints();
  double mTimeStep = 0.001;

  FloatVector res(nJoints);
  res.setZero(nJoints);
  // set non singular start pos
  res[0] = -0.438;
  res[1] = -0.423;
  res[3] = -0.348;
  res[5] = -0.445;
  // corresponding to
  // p [0.22916, -0.107325, 1.16779]
  // quat = [0.943308, 0.0558517, 0.25094, -0.209952]
  auto resource = boost::make_shared<DummyResource>(res);

  // sensor transform for position + quaternion control
  std::vector<CBF::SensorTransformPtr> sensorTrafos
          = {boost::make_shared<CBF::KDLChainPositionSensorTransform>(lwrchain),
             boost::make_shared<CBF::KDLChainQuaternionSensorTransform>(lwrchain)};
  auto sensorTransfo = boost::make_shared<CBF::CompositeSensorTransform>(sensorTrafos);

  // compute FK once
  sensorTransfo->update(resource->get_position());

  // potential
  std::vector<CBF::PotentialPtr> potentials = {boost::make_shared<CBF::SquarePotential>(3, 3),
                                               boost::make_shared<CBF::QuaternionPotential>()};
  auto potential = boost::make_shared<CBF::CompositePotential>(potentials);

  // controller
  int num_ref = 7;
  CBF::PrimitiveController c(mTimeStep, std::vector<ConvergenceCriterionPtr>(),
      boost::make_shared<CBF::DummyReference>(1, num_ref),
      boost::make_shared<CBF::BypassFilter>(mTimeStep, num_ref, num_ref), // no refFilter
      potential,
      boost::make_shared<CBF::PDPositionControl>(mTimeStep, potential->task_dim(), 1.0),
      sensorTransfo,
      // boost::make_shared<CBF::DampedGenericEffectorTransform>(potential->task_dim(), nJoints, 1e-3),
      boost::make_shared<CBF::ThresholdGenericEffectorTransform>(potential->task_dim(), nJoints, 0.5),
      std::vector<SubordinateControllerPtr>(), // No SubordinateController
      boost::make_shared<CBF::AddingStrategy>(),
      resource,
      boost::make_shared<CBF::BypassFilter>(mTimeStep, nJoints, nJoints), // no resourceFilter
      boost::make_shared<CBF::NullLimiter>(mTimeStep, nJoints) // no Limiter
      );

  // prepare target
  Eigen::VectorXd target, reached;
  target = sensorTransfo->result(); // x,y,z, qw, qx, qy, qz

  std::cerr << "time, targetY, Y, q_1, q_2, q_3, q_4, q_5, q_6, q_7" << std::endl;
  for (unsigned int n = 0; n < 20; ++n) {
    // create a step on y of 10 cm
    if (n == 5) {
      target(1) += 0.1;
      c.reference()->set_reference(target);
    }
    c.update();
    c.action();

    // compute FK of reached joint pos
    const auto& q = resource->get_position();
    c.sensor_transform()->update(q);
    reached = c.sensor_transform()->result();

    std::cout << n * mTimeStep << " " << target(1) << " " << reached(1) << " " << q.transpose() << std::endl;
  }

  return 0;
}
