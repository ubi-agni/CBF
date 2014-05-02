/*
 * cbf_test.cpp
 *
 *  Created on: 30.04.2014
 *      Author: cemmeric
 */

#include <kdl/chain.hpp>
#include <kdl/segment.hpp>
#include <boost/shared_ptr.hpp>

#include <cbf/composite_transform.h>
#include <cbf/identity_transform.h>
#include <cbf/generic_transform.h>
#include <cbf/kdl_transforms.h>

using namespace KDL;
using namespace std;
using namespace boost;
using namespace CBF;

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

int main() {

	boost::shared_ptr<Chain> lwrchain(new Chain(createKDLChain()));

	SensorTransformPtr kdlst(new KDLChainPositionSensorTransform(lwrchain));
	cout << "kdlst->resourcedim=" << kdlst->resource_dim() << endl;
	cout << "kdlst->taskdim=" << kdlst->task_dim() << endl;

	return 0;
}

