################################################################################
##
## Isaac Genome Alignment Software
## Copyright (c) 2010-2017 Illumina, Inc.
## All rights reserved.
##
## This software is provided under the terms and conditions of the
## GNU GENERAL PUBLIC LICENSE Version 3
##
## You should have received a copy of the GNU GENERAL PUBLIC LICENSE Version 3
## along with this program. If not, see
## <https://github.com/illumina/licenses/>.
##
################################################################################
##
## file CMakeLists.txt
##
## Top level cmake configuration file
##
## author Come Raczy
##
################################################################################

cmake_minimum_required(VERSION 3.1.2)
enable_testing()

if (DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel.")
else()
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel.")
endif()

project (Isaac)
set(iSAAC_STRING "Isaac")
# The Isaac version number.
# According to http://rpm.org/api/4.4.2.2/rpmvercmp_8c-source.html the numeric version components are always 
# newer than alphabetic Jira ticket name in marketing version ensures it is easy to locate the branch and that
# the branch builds will never upgrade the properly released version 
set(iSAAC_VERSION_MAJOR 04)
set(iSAAC_VERSION_MINOR 18)
set(iSAAC_VERSION_PATCH 11)
set(iSAAC_VERSION_DEVEL 09)
set(iSAAC_VERSION_FULL "${iSAAC_STRING}-${iSAAC_VERSION_MAJOR}.${iSAAC_VERSION_MINOR}.${iSAAC_VERSION_PATCH}.${iSAAC_VERSION_DEVEL}")

if (CPACK_GENERATOR)
    message (STATUS "Configuring to produce the following package types: ${CPACK_GENERATOR}")
    SET(CPACK_PACKAGE_VENDOR "Illumina")
    SET(CPACK_PACKAGE_VERSION_MAJOR "${iSAAC_VERSION_MAJOR}")
    SET(CPACK_PACKAGE_VERSION_MINOR "${iSAAC_VERSION_MINOR}")
    SET(CPACK_PACKAGE_VERSION_PATCH "${iSAAC_VERSION_PATCH}.${iSAAC_VERSION_DEVEL}")
    SET(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
    SET(CPACK_SET_DESTDIR ON) # this one is required for tgz to work
    SET(CPACK_DEBIAN_PACKAGE_OWNER root)
    SET(CPACK_DEBIAN_PACKAGE_GROUP root)
    INCLUDE(CPack)
endif (CPACK_GENERATOR)

if   (NOT CMAKE_PARALLEL)
    set (CMAKE_PARALLEL "1")
endif (NOT CMAKE_PARALLEL)

#set(CMAKE_INSTALL_PREFIX "${iSAAC_PREFIX}")

if    (NOT iSAAC_EXEC_PREFIX)
    if    ((NOT iSAAC_BINDIR) AND (NOT iSAAC_LIBDIR) AND (NOT iSAAC_LIBEXECDIR) AND (NOT iSAAC_DATADIR))
        set(iSAAC_MOVABLE_INSTALLATION TRUE)
    endif ((NOT iSAAC_BINDIR) AND (NOT iSAAC_LIBDIR) AND (NOT iSAAC_LIBEXECDIR) AND (NOT iSAAC_DATADIR))
    set(iSAAC_EXEC_PREFIX "${iSAAC_PREFIX}")
else  (NOT iSAAC_EXEC_PREFIX)
    set(iSAAC_MOVABLE_INSTALLATION FALSE)
endif (NOT iSAAC_EXEC_PREFIX)

if    (NOT iSAAC_BINDIR)
    set(iSAAC_BINDIR "bin" CACHE PATH "Install bin directory" FORCE)
endif (NOT iSAAC_BINDIR)

if    (NOT iSAAC_LIBDIR)
    set(iSAAC_LIBDIR "lib/${iSAAC_VERSION_FULL}" CACHE PATH "Install lib directory" FORCE)
endif (NOT iSAAC_LIBDIR)

if    (NOT iSAAC_LIBEXECDIR)
    set(iSAAC_LIBEXECDIR "libexec/${iSAAC_VERSION_FULL}" CACHE PATH "Install libexec directory" FORCE)
endif (NOT iSAAC_LIBEXECDIR)

if    (NOT iSAAC_DATADIR)
    set(iSAAC_DATADIR "share/${iSAAC_VERSION_FULL}" CACHE PATH "Install data directory" FORCE)
endif (NOT iSAAC_DATADIR)


message (STATUS "install      prefix: ${iSAAC_PREFIX}")
message (STATUS "install exec prefix: ${iSAAC_EXEC_PREFIX}")
message (STATUS "install     bin dir: ${iSAAC_BINDIR}")
message (STATUS "install     lib dir: ${iSAAC_LIBDIR}")
message (STATUS "install libexec dir: ${iSAAC_LIBEXECDIR}")
message (STATUS "install    data dir: ${iSAAC_DATADIR}")

add_custom_target(iSAAC_CXX)
add_custom_target(iSAAC_OPT)

set (ISAAC_BUILD_TYPE "${CMAKE_BUILD_TYPE}")

# Get around broken BOOST cmake macros if BOOST_ROOT set
set(CMAKEBOOSTROOT $ENV{BOOST_ROOT})
if( NOT ${CMAKEBOOSTROOT} STREQUAL "" )
   set(Boost_NO_SYSTEM_PATHS ON)
   message("BOOST_ROOT is set, Boost_NO_SYSTEM_PATHS is ${Boost_NO_SYSTEM_PATHS}")
endif( NOT ${CMAKEBOOSTROOT} STREQUAL "" )

set(BOOST_REDIST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../redist")

set (iSAAC_BOOST_VERSION 1.62.0)
set (iSAAC_BOOST_VERSION_MIN 1.58.0)

set (iSAAC_BOOST_COMPONENTS date_time filesystem iostreams program_options
                                  regex serialization system thread)
set (Boost_USE_MULTITHREAD ON)

# required libxml2 and libxslt libraries
set (iSAAC_LIBXML2_VERSION 2.9.6)
set (LIBXML2_REDIST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../redist")
set (LIBXSLT_REDIST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../redist")

set (LIBXML2_INSTAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../redist")
set (LIBXSLT_INSTAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../redist")

set (iSAAC_LIBXSLT_VERSION 1.1.31)
set (LIBXSLT_REDIST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../redist")

set (iSAAC_LIBEXSLT_VERSION 0.8.19)


set (ISAAC_READ_LENGTH_MAX 600 CACHE STRING "Maximum read lengt supported by the aligner")
set (ISAAC_CONTIG_LENGTH_MIN 0x10000 CACHE STRING "Minimum length assumed for a contig")
set (ISAAC_GENOME_OFFSET_MAX 0x0ffffffff CACHE STRING "Maximum total length of all contigs combined")

# globals and macros
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
set (iSAAC_GLOBALS_CMAKE "${CMAKE_SOURCE_DIR}/cmake/globals.cmake")
set (iSAAC_MACROS_CMAKE "${CMAKE_SOURCE_DIR}/cmake/macros.cmake")

# redist includes

if (WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    # In Windows, build boost, zlib, libxml2 and libxslt using the Intel C++ compiler

    set (iSAAC_ZLIB_VERSION 1.2.8)
    set (ZLIB_REDIST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../redist")
    set (ZLIB_INSTAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../redist")

    string (TOUPPER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_UPPER)

    include ("${CMAKE_SOURCE_DIR}/cmake/win32/boost_win_intel.cmake")
    include ("${CMAKE_SOURCE_DIR}/cmake/win32/xml_xslt_win_intel.cmake")
    include ("${CMAKE_SOURCE_DIR}/cmake/win32/vs_intel.cmake")

else ()
  if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
     include ("${CMAKE_SOURCE_DIR}/cmake/clang.cmake")

     include ("${CMAKE_SOURCE_DIR}/cmake/boost.cmake")
  else ()

    # configure compiler
    include ("${CMAKE_SOURCE_DIR}/cmake/gcc.cmake")

    include ("${CMAKE_SOURCE_DIR}/cmake/boost.cmake")
  endif()

endif ()

include ("${CMAKE_SOURCE_DIR}/cmake/isaac_redist_macros.cmake")

# subdirs
add_subdirectory (cmake/preInstall)
add_subdirectory (bash)
add_subdirectory (c++)
add_subdirectory (css)
add_subdirectory (data)
add_subdirectory (validation)
add_subdirectory (make)
add_subdirectory (xsl)
add_subdirectory (cmake/postInstall)
add_subdirectory (markdown)
