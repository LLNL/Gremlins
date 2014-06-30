# CMake file
# Copyright (C) ZIH, Technische Universitaet Dresden, Federal Republic of Germany, 2011-2013
# Copyright (C) Lawrence Livermore National Laboratories, United States of America, 2011-2013
#
# See the file LICENSE.txt in the package base directory for details
#
# @file FindMsr.cmake
#       Detects the libmsrAPI installation.
#
# @author Joachim Protze
# @date 13.12.2012

#
# -DUSE_CALLPATH=ON
# -DLIBMSR_INSTALL_PREFIX=<libmsr_prefix>  or  -DLIBMSR_HOME=<libmsr_prefix>
#
# tries to autodetect the installation sceme of dyninst-7 an dyninst-8
# could fail when having builts for multiple architectures
# in this case set:
#       -DLIBMSR_LIB_PATH=<libmsr_prefix>/<arch>/lib
#


# use an explicitly given stackwalk path first
FIND_PATH(LIBMSR_INCLUDE_PATH msr_rapl.h 
            PATHS ${LIBMSR_INSTALL_PREFIX} ${LIBMSR_HOME} /usr /usr/local
            PATH_SUFFIXES   include  NO_DEFAULT_PATH)
# if not-found, try again at cmake locations
FIND_PATH(LIBMSR_INCLUDE_PATH msr_rapl.h)

# use an explicitly given stackwalk path first
FIND_PATH(LIBMSR_LIB_PATH libmsr.so 
            PATHS ${LIBMSR_INSTALL_PREFIX} ${LIBMSR_HOME} /usr /usr/local
            PATH_SUFFIXES   lib 
                            lib64/dyninst
		NO_DEFAULT_PATH)
# if not-found, try again at cmake locations
FIND_PATH(LIBMSR_LIB_PATH libmsr.so)

# collect all libraries (for the ubuntu users :)
set(LIBMSR_LIBRARIES "libmsr.so")

set(LIBMSR_LIBRARY_DEPENDENCIES
#	libpthread.so
	libm.so
     )

FOREACH ( LIB ${LIBMSR_LIBRARY_DEPENDENCIES} )
    FIND_LIBRARY(${LIB}_LOC ${LIB} 
            PATHS ${LIBMSR_LIB_PATH} NO_DEFAULT_PATH)
    IF (${LIB}_LOC)
        set(LIBMSR_LIBRARIES ${LIBMSR_LIBRARIES} "${${LIB}_LOC}" )
    ENDIF (${LIB}_LOC)
ENDFOREACH ( LIB )

set(LIBMSR_EXTRA_LIBRARIES  "" CACHE STRING "Additional libraries needed for compiling with the libmsr, this is usually required if you have a static library for libdwarf.")


find_package_handle_standard_args(LIBMSR  DEFAULT_MSG  LIBMSR_INCLUDE_PATH LIBMSR_LIBRARIES )

