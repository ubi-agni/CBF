/* -*- mode: c-non-suck; -*- */

#ifndef CBF_DECL_PLUGIN_MACROS_HH
#define CBF_DECL_PLUGIN_MACROS_HH

#include <cbf/plugin_pool.h>

/** 
	Declare the constructor that takes a ClassNameType XML entity
	instance.. Use this inside the class declaration..
*/
#define CBF_PLUGIN_DECL_METHODS(ClassName) \
	ClassName(const ClassName##Type &xml_instance);

/** This macro forward declares the corresponding XSD XML type. Always use outside of CBF namespace */
#define CBF_PLUGIN_PREAMBLE(ClassName) \
	class ClassName##Type;

#endif
