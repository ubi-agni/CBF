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

namespace CBF {

/**
	\mainpage A Control Basis Framework Library (CBF)

	\author Florian Paul Schmidt

	To start with, here's a simple example that creates a very simple controller. This example can be found in the doc/examples/cpp directory named cbf_simple_test.cc. It should have been built together with the library libcbf.so. All controllers created manually like this follow the same scheme. Read on below this example for more detailed information.

	\includelineno cbf_simple_test.cc

	\section README

	For easy reference, we repeat the README file from the source distribution here:

	\verbinclude README

	\section INSTALL

	For easy reference, we repeat the INSTALL file from the source distribution here:

	\verbinclude INSTALL

	\section Overview
	libcbf is an implementation of the closed loop controller part of the Control Basis Framework idea originally proposed by Grupen et. al. It provides means of "synthesizing" controllers from basic building blocks. These building blocks are:

	- PrimitiveController The controller is the only class of this list which is non-abstract as it is used to be parametrized by concrete instances of the following abstract classes

	- SensorTransform This transform implements the transformation from resource vectors to the task space. A real life example would be the mapping from joint angle space to cartesian 3D coordinates representing the end effector position of a robotic manipulator. Here the joint angles are the control resource and the task space is cartesian 3D space

	- Potential An artificial potential function. The potential function is usually only defined implicitly via a gradient step. This gradient step is a task space update step which gets mapped back to the control resources via the EffectorTransform of the controller

	- Resource A resource represents capabilities of a robot as a vector of CBFFloat. Often these are control affordances of a robotic manipulator, e.g. joint angles.

	- EffectorTransform This transform implements the inverse of the task jacobian. It is used to transform the gradient step in Task space to a gradient step in Resource space


	- CombinationStrategy This allows to specify how results from multiple subordinate controllers are combined before being projected into the nullspace of the controller. An example would be the simple AddingStrategy which simply adds all contributions

	Read on below for usage information...

	\section Derived-Classes

	CBF comes with some classes that you can use to assemble controllers without having to implement any base classes. Here is an incomplete list (make sure you browse the complete class list (see menu), too):

	\subsection SensorTransforms

	- IdentitySensorTransform - A SensorTransform that does what it says, nothing :)

	- LinearSensorTransform - A SensorTransform implementing a constant linear transform. Constant in the sense, that its coefficients do no change with the resource values. And consequently the jacobian is constant at all times

	- KDLChainPositionSensorTransform - A SensorTransform that is based on a KDL::Chain and provides access to the 3D cartesian position of the end effector of the chain

	- KDLChainAxisAngleSensorTransform - A SensorTransform that is based on a KDL::Chain and provides access to the orientation of the end effector of the chain in compact axis-angle representation

	- PythonSensorTransform - A SensorTransform that calls a python script for prototyping purposes

	- CompositeSensorTransform - A SensorTransform that combines several sensor transforms into a single composite one. This allows assembling the task space from different sensor readings. E.g. combining KDLChainPositionSensorTransform and KDLChainAxisAngleSensorTransform into a composite position/orientation SensorTransform. See also the CompositePotential class

	\subsection EffectorTransforms

	- GenericEffectorTransform - An EffectorTransform that is based on the Moore-Penrose Pseudoinverse

	\subsection Potentials

	- SquarePotential - A Potential implementing a gradient step of a polynom of second degree

	- AxisAnglePotential - A Potential operating on the space of orientations in axis-angle representation

	- PythonPotential - A Potential that calls a python script for prototyping purposes

	- CompositePotential - This is useful when the task space consists of fundamentally different spaces where a single (e.g. quadratic) Potential is not useful alone. See also the CompositeSensorTransform

	If you have built CBF with support for XML (using XSD and XERCES-C) then there are corresponding classes whose names have a suffix "Type", e.g. IdentitySensorTransformType. These are classes that are autogenerated by XSD and allow super simple access to the attributes of an XML instance. As a naming convention all of these classes end with the suffix "Type".

	\section Usage
	There exist several ways of synthesizing a controller:

	\subsection Manually

	- Use the PrimitiveController class itself, instantiate the desired Potential, Resource, EffectorTransform and SensorTransform (optionally a CombinationStrategy if you use multiple subordinate controllers) and set the PrimitiveController's members accordingly.

	- There exist several ways of composing the controllers or even the parts of controllers. The SensorTransformChain allows serial chaining of SensorTransform. The CompositePotential allows to use different potential functions for different parts of the task space (e.g. a SquarePotential for cartesian position and a QuaternionPotential for orientation). Finally the PrimitiveControllerResource allows to have one controller act as Resource for another which allows e.g. visual servoing.

	After creating the controller the Controller::step() function needs to be called to the user's content (e.g. until convergence)..

	\subsection XML-Description

	An alternative way is by using XML descriptions of controllers. Have a look into the doc/examples/xml folder for some examples. This example shows the most simple XML document to create a controller

	\include simple.xml 

	Here is an example that is a little more elaborate. It sports composite Potentials and SensorTransforms to create a composite position/orientation controller for a 7-DOF Kuka leightweight arm.

	\include kdl_kuka_composite.xml

	\section Examples
	You might take a look at one of the test programs in the test/ folder (follow the below link and then click  "Go to the source code of this file."):

	- cbf_run_controller_from_xml.cc This example shows how to create a controller from an XML file description.

	- cbf_simple_test.cc This example tests some basic CBF functionality. It also assembles a PrimitiveController "manually".

	- cbf_test_kdl_transforms.cc This example manually builds a PrimitiveController using the KDLChainEffectorTransform and KDLChainSensorTransform.

	- cbf_test_kdl_transforms_view.cc This example is like the one above, but it also visualizes the results using the QKDLChainView class.

	\section Third-Party-Class-Integration

	To integrate your own classes into the XML parsing functionality, you need to do these things:

	- Create a XSD-XML-Schema type in the file schemas/controller.xsd. Take a look at the other types in this file. It should be rather self explanatory.From the schema type a C++ class is created using the XSD schema to c++ translator. This C++ class allows typesafe and easy access to the attributes of an xml instance of the type. See e.g. the source of the PrimitiveController(const PrimitiveControllerType &) constructor of the PrimitiveController class.

	- Add a static create() function to your class taking a const reference to a schema type corresponding to your class's supertype (see. e.g. IdentitySensorTransform for an example on how to do this). This static function has to dynamic_cast the passed reference to its own schema type and then call the corresponding constructor.

	- Add a constructor to your class that takes a const reference to its corresponding schema type. See e.g. IdentitySensorTransform.

	- Register your class type at startup with the superclass. The superclass (see e.g. Controller) has a std::vector of creator functions. When it is time to instantiate a derived class, the superclass will go through the creator functions until it finds one that does not return an "empty" shared_ptr ("empty" here means, that p.get() == 0, if p is the shared_ptr).

	\section C-Bindings

	There exists a rudimentary C-binding which allows to create controllers from XML files and use these. Here's a small example

	\include cbf_test_c_bindings.c

	\section Python-Bindings

	There also exist some rudimentary python bindings which can be used for testing purposes.. Here's a simple example program..

	\include cbf_test_python.py
*/

}

