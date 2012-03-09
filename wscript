#./waf-light --tools=compat15,swig,fc,compiler_fc,fc_config,fc_scan,gfortran,g95,ifort,gccdeps;

import os
from textwrap import dedent

top = '.'
out = 'build'

def options(opt):
    opt.load('compiler_c')
    opt.load('compiler_cxx')
    opt.load('python')
    opt.load('inplace', tooldir='tools')

def configure(conf):
    conf.add_os_flags('PATH')
    conf.add_os_flags('PYTHON')
    conf.add_os_flags('PYTHONPATH')
    conf.add_os_flags('INCLUDES')
    conf.add_os_flags('LIB')
    conf.add_os_flags('LIBPATH')
    conf.add_os_flags('STLIB')
    conf.add_os_flags('STLIBPATH')
    conf.add_os_flags('CFLAGS')
    conf.add_os_flags('CXXFLAGS')
    conf.add_os_flags('LINKFLAGS')
    conf.add_os_flags('CYTHONFLAGS')
    conf.add_os_flags('CXX')
    conf.add_os_flags('CXXFLAGS')

    conf.load('compiler_c')
    conf.load('compiler_cxx')

    conf.load('python')
    conf.check_python_version((2,5))
    conf.check_python_headers()

    conf.check_mpi()
    conf.check_mpi4py()

    conf.check_tool('numpy', tooldir='tools')
    conf.check_numpy_version(minver=(1,3))
    conf.check_tool('cython', tooldir='tools')
    conf.check_cython_version(minver=(0,11,1))
    conf.check_tool('inplace', tooldir='tools')

    conf.env.CYTHONFLAGS = '--fast-fail'
    conf.env.CXX = 'mpic++'
#    conf.env.LD = 'mpic++'

def build(bld):
    
    bld(source=['src/distarray.cpp'],
        includes=['src'],
        target='distarray',
        use='',
        features='cxxshlib cxx')

    bld(source=['distarray/wrapper.pyx'],
        includes=['src'],
        target='wrapper',
        use='distarray MPI MPI4PY NUMPY',
        features='cxx cxxshlib pyext')

    bld(source=['examples/distarray_demo.cpp'],
        includes=['src'],
        target='distarray_demo',
        use='distarray MPI',
        features='cxxprogram cxx')


from waflib.Configure import conf
from os.path import join as pjoin
from waflib import TaskGen

@conf
def check_mpi4py(conf):
    conf.start_msg("Checking mpi4py includes")
    (mpi4py_include,) = conf.get_python_variables(
            ['mpi4py.get_include()'], ['import mpi4py'])
    conf.env.INCLUDES_MPI4PY = mpi4py_include
    conf.end_msg('ok (%s)' % mpi4py_include)

@conf
def check_mpi(conf):
    # try to detect openmpi installation
    mpi = conf.check_cfg(path='mpic++', args='-showme',
                         package='', uselib_store='MPI', mandatory=False)
    if not mpi:
        # try the MPICH2 flags
        mpi = conf.check_cfg(path='mpic++', args='-compile-info -link-info',
                             package='', uselib_store='MPI', mandatory=False)
    if mpi:
        conf.env['ENABLE_MPI'] = True
        for libpath in conf.env.LIBPATH_MPI:
            if 'mpi' in libpath:
                conf.env.append_value('LINKFLAGS_MPI', '-Wl,-rpath='+libpath)
            #conf.report_optional_feature("mpi", "MPI Support", True, '')            
    else:
        conf.fail('MPI not detected')
        pass
    #conf.report_optional_feature("mpi", "MPI Support", False, 'mpic++ not found')

