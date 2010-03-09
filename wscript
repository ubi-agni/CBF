# -*- mode: python -*-
import Options
import distutils.sysconfig
import os

VERSION = '0.1'
APPNAME = 'CBF'
srcdir  = '.'
blddir  = 'build'

def set_options(opt):
	opt.tool_options('python')
	opt.tool_options('compiler_cc')

	opt.add_option(
		'--with-kdl-dir', 
		action='store', 
		help='PREFIX, where to find the KDL installation',
		dest='kdldir'
	)


	opt.add_option(
		'--build-docs', 
		action='store_true', 
		default=False, 
		help='Build documentation (given that doxygen is available)',
		dest='docs'
	)

	opt.add_option(
		'--plugin-support', 
		action='store_true', 
		default=True, 
		help='Enable adding code for dynamic loading and creation of controller components',
		dest='enable_plugins'
	)

	opt.add_option(
		'--optimize', 
		action='store_true', 
		default=True, 
		help='Optimize build aggressively',
		dest='optimized'
	)

	opt.add_option(
		'--debug',
		action='store_true',
		default=False,
		help="Add debugging symbols in library and executables",
		dest="debug"
	)

	opt.add_option(
		'--debug-messages',
		action='store_true',
		default=False,
		help="Add debugging symbols in library and executables",
		dest="debug_messages"
	)

	opt.add_option(
		'--profile', 
		action='store_true', 
		default=False, 
		help='Use profiling info',
		dest='profiled'
	)


	opt.add_option(
		'--debug-color', 
		action='store_true', 
		default=True, 
		help='Color debugging output to improve readability',
		dest='debug_color'
	)

	opt.add_option(
		'--with-xercesc-dir',
		dest='xercescdir',
		type='string',
		action='store',
		help='PREFIX, where xerces lib and headers are installed'
	)

	opt.add_option(
		'--with-xsd',
		dest='xsdbinary',
		type='string',
		action='store',
		help='Where XSD executable is installed (fully qualified program name)'
	)


	opt.add_option(
		'--with-xsd-include-dir',
		dest='xsdincludedir',
		type='string',
		action='store',
		help='Where XSD header files are installed'
	)

	opt.add_option(
		'--with-xsd-bin-dir',
		dest='xsdbindir',
		type='string',
		action='store',
		help='Path to binary directory of XSD installation'
	)


	opt.add_option(
		'--with-xsd-dir',
		dest='xsddir',
		type='string',
		action='store',
		help='PREFIX, where XSD is installed'
	)


	opt.add_option(
		'--with-doxygen',
		dest='doxygenbinary',
		type='string',
		action='store',
		help='Where DOXYGEN executable is installed (fully qualified program name)'
	)



	opt.add_option(
		'--with-python-c-dir',
		dest='pythoncdir',
		type='string',
		action='store',
		help='PREFIX, where Python-C lib and headers are installed'
	)
	

	opt.add_option(
		'--with-boost-python-dir',
		dest='boostpythondir',
		type='string',
		action='store',
		help='PREFIX, where Python-C lib and headers are installed'
	)

	opt.add_option(
		'--with-eigen2-dir',
		dest='eigen2dir',
		type='string',
		action='store',
		help='PREFIX, where Eigen2 distribution is installed'
	)

	opt.add_option(
		'--with-eigen2-include-dir',
		dest='eigen2inc',
		type='string',
		action='store',
		help='PATH, where Eigen2 headers are installed'
	)

	
	opt.add_option(
		'--with-pa10-dir',
		dest='pa10dir',
		type='string',
		action='store',
		help='PREFIX, where Mitsubishi PA10 lib and headers are installed'
	)
	
	opt.add_option(
		'--with-kdl-chain-viz-dir',
		dest='kdlchainvizdir',
		type='string',
		action='store',
		help='PREFIX, where kdl-chain-viz lib and headers are installed'
	)

	
	opt.add_option(
		'--with-spacenavi-dir',
		dest='spacenavidir',
		type='string',
		action='store',
		help='PREFIX, where spacenavi lib and headers are installed'
	)


	opt.sub_options('libcbf')
	opt.sub_options('qcbf')
	opt.tool_options('qt4')
	#opt.sub_options('test')
	#opt.sub_options('schemas')








def configure(conf):
	conf.check_tool('gcc g++ qt4')
	conf.check_tool('misc')

	# the defines variable will be filled with defines we want to put into 
	# config.h
	defines = dict()

	# options handling =================================
	if Options.options.optimized:
		conf.env.append_value('CXXFLAGS', ['-O4', '-funroll-loops', '-Wall', '-Werror', '-march=native', '-ffast-math'])
	else:
		conf.env.append_value('CXXFLAGS', ['-Wall', '-Werror'])

	if Options.options.debug:
		conf.env.append_value('CXXFLAGS', ['-g'])
	
	if (Options.options.debug_messages == False):
		conf.define('CBF_NDEBUG',1)
		conf.define('NDEBUG', 1)
		defines['CBF_NDEBUG'] = 1
		
	if Options.options.debug_color:
		conf.define('CBF_DEBUG_COLOR',1)
		conf.define('CBF_DEBUG_COLOR', 1)
		defines['CBF_DEBUG_COLOR'] = 1

	if Options.options.profiled:
		conf.env.append_value('CXXFLAGS', ['-pg', '-g'])
		conf.env.append_value('LINKFLAGS', ['-pg', '-g'])
	
	if Options.options.enable_plugins:
		conf.define('CBF_HAVE_PLUGIN_SUPPORT', 1)
		defines['CBF_HAVE_PLUGIN_SUPPORT'] = 1

	# pkg-config
	conf.check_cfg(atleast_pkgconfig_version='0.9.4')

	# KDL
	if Options.options.kdldir:
		s = Options.options.kdldir + "/lib/pkgconfig/";
		if 'PKG_CONFIG_PATH' in os.environ:
			os.environ['PKG_CONFIG_PATH'] += ":" + s
		else:
			os.environ['PKG_CONFIG_PATH'] = s	

	conf.check_cfg(package='orocos-kdl', atleast_version='0.99.0', args='--cflags --libs', uselib_store='OROCOS-KDL')
	if conf.env['HAVE_OROCOS_KDL'] == 1:
		conf.define('CBF_HAVE_KDL', 1)
		defines['CBF_HAVE_KDL'] = 1

	# SPACEMOUSE 
	conf.env.STATICLIB_SPACEMOUSE = 'spacenavi'
	if Options.options.spacenavidir:
		conf.env.LIBPATH_SPACEMOUSE = Options.options.spacenavidir + "/lib"
		conf.env.CPPPATH_SPACEMOUSE = Options.options.spacenavidir + "/include"
	if conf.check(fragment = '#include "spacenavi.h"\n int main() { snavi_open("bla", 0); return 0; }\n', uselib = 'SPACEMOUSE', msg="Checking for spacenavi"):
		conf.define('CBF_HAVE_SPACEMOUSE', 1)
		defines['CBF_HAVE_SPACEMOUSE'] = 1

	# EIGEN2
	if Options.options.eigen2dir:
		conf.env.CPPPATH_EIGEN2 = Options.options.eigen2dir + "/include"
	if Options.options.eigen2inc:
		conf.env.CPPPATH_EIGEN2 = Options.options.eigen2inc
	if conf.check(fragment = '#include "eigen2/Eigen/Core"\n int main() { return 0; }\n', uselib = 'EIGEN2', msg="Checking for eigen2"):
		conf.define('CBF_HAVE_EIGEN2', 1)
		defines['CBF_HAVE_EIGEN2'] = 1

	# PA 10
	conf.env.LIB_PA10 = 'pa10'
	if Options.options.pa10dir:
		conf.env.LIBPATH_PA10 = Options.options.pa10dir + "/lib"
		conf.env.CPPPATH_PA10 = Options.options.pa10dir + "/include"
	if conf.check(fragment = '#include "pa10/pa.h"\n int main() { pa_ini_sys(); return 0; }\n', uselib = 'PA10', msg="Checking for pa10"):
		conf.define('CBF_HAVE_PA10', 1)
		defines['CBF_HAVE_PA10'] = 1

	# KDLCHAINVIZ
	conf.env.LIB_KDLCHAINVIZ = 'kdlchainview'
	conf.env.LIB_QKDLCHAINVIZ = 'qkdlchainview'
	if Options.options.kdlchainvizdir:
		conf.env.LIBPATH_KDLCHAINVIZ = Options.options.kdlchainvizdir + "/lib"
		conf.env.CPPPATH_KDLCHAINVIZ = Options.options.kdlchainvizdir + "/include"
		conf.env.LIBPATH_QKDLCHAINVIZ = Options.options.kdlchainvizdir + "/lib"
		conf.env.CPPPATH_QKDLCHAINVIZ = Options.options.kdlchainvizdir + "/include"
	if conf.check(fragment = '#include "qkdlchainview.h"\n int main() { return 0; }\n', uselib = 'KDLCHAINVIZ QKDLCHAINVIZ QTCORE QTGUI QTOPENGL OROCOS-KDL', msg="Checking for kdlchainviz"):
		conf.define('CBF_HAVE_KDLCHAINVIZ', 1)
		defines['CBF_HAVE_KDLCHAINVIZ'] = 1

	# XERCESC
	conf.env.LIB_XERCESC = "xerces-c"
	if Options.options.xercescdir:
		conf.env.LIBPATH_XERCESC = Options.options.xercescdir + "/lib"
	if conf.check(fragment='int main() { return 0; }\n', uselib='XERCESC', msg="Checking for xercesc"):
		conf.define('CBF_HAVE_XERCESC' ,1)
		defines['CBF_HAVE_XERCESC'] = 1

	# XSD
	xsdinc = ''
	xsdpath = ''
	if Options.options.xsddir:
		xsdinc =  Options.options.xsddir + '/include/'
		xsdpath =  Options.options.xsddir + "/bin/"

	if Options.options.xsdincludedir:
		xsdinc = Options.options.xsdincludedir

	if Options.options.xsdbindir:
		xsdpath = Options.options.xsdbindir 

	if Options.options.xsdbinary:
		conf.find_program(Options.options.xsdbinary, var='XSD')
	else:
		conf.find_program('xsdcxx', var='XSD')
		if not ('XSD' in conf.env):
			print ('xsdcxx not found - checking for xsd instead')
			conf.find_program('xsd', var='XSD')

	if xsdinc:
		conf.env.CPPPATH_XSD = xsdinc

	if conf.env.XSD and conf.check(fragment='#include "xsd/cxx/version.hxx"\n int main() { return 0; }\n', uselib='XSD XERCESC', msg="Checking for XSD headers"):
		conf.define('CBF_HAVE_XSD', 1)
		defines['CBF_HAVE_XSD'] = 1
		#conf.env.CXXDEFINES_XSD = ['HAVE_XSD=1']

	# DOXYGEN
	if Options.options.doxygenbinary:
		conf.find_program(Options.options.doxygenbinary, var = 'DOXYGEN')
	else:
		conf.find_program('doxygen', var = 'DOXYGEN')

	if 'DOXYGEN' in conf.env:
		conf.define('CBF_HAVE_DOXYGEN', 1)
		defines['CBF_HAVE_DOXYGEN'] = 1


	# boost.python
#	conf.check_tool('compiler_cc')
	conf.check_tool('python')
	conf.check_python_version((2,4,2))
	conf.check_python_headers()
	conf.check(lib='boost_python-mt', uselib_store='BOOST_PYTHON', uselib='PYEXT PYEMBED')
	conf.check(header_name='boost/python.hpp', uselib_store='BOOST_PYTHON', uselib='PYEXT PYEMBED')
	if ('LIB_PYEMBED' in conf.env) and ('LIB_BOOST_PYTHON' in conf.env):
		print ("we got python and boost")
		conf.define('CBF_HAVE_BOOST_PYTHON', 1)
		defines['CBF_HAVE_BOOST_PYTHON'] = 1
#

	# since we don't want all kinds of junk in config.h, but just the ones
	# we have added explicitly in the dict 'defines' clear all defines
	# and refill them with the right ones
	conf.env.defines = []
	for key in defines.keys():
		conf.define(key, defines[key])

	conf.write_config_header('libcbf/cbf/config.h')
	#conf.write_config_header('config.h')

	print " ========================================================="
	print "|| If any of the above checks failed,                    ||"
	print "|| please check ./build/config.log for more information! ||"
	print " ========================================================="





def build(bld):
	bld.add_subdirs('schemas qcbf libcbf apps tests')

	bld.install_files('${PREFIX}/include/cbf', bld.path.ant_glob('libcbf/cbf/*.h', bld=True, flat=True))
	bld.install_files('${PREFIX}/include/cbf', bld.path.ant_glob('schemas/*.hxx', bld=True, flat=True))
	
	if Options.options.docs:
		if not 'DOXYGEN' in bld.env:
			raise Exception("Build of documentation requested, but doxygen seems not to be installed. Aborting....")
		task=bld.new_task_gen()
		task.rule=bld.env['DOXYGEN'] + ' ../../Doxyfile'
		task.cwd=bld.bdir + '/default'
		task.always = True
		#print "build doc"
	
	
