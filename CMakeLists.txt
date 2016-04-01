#################################################
# cmake file for building ArborContent framework
# @author Eté Rémi ,IPNL
# Copyright (c) CNRS / IPNL
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 FATAL_ERROR )
#################################################

# project name
PROJECT( ArborContent )

SET( ${PROJECT_NAME}_VERSION_MAJOR 01 )
SET( ${PROJECT_NAME}_VERSION_MINOR 01 )
SET( ${PROJECT_NAME}_VERSION_PATCH 00 )

# --------------- ilcsoft dependencies -----------
FIND_PACKAGE( ILCUTIL COMPONENTS ILCSOFT_CMAKE_MODULES REQUIRED )
INCLUDE( ilcsoft_default_settings )

OPTION( PANDORA_MONITORING "Whether to use PandoraMonitoring (requires ROOT)" ON )
OPTION( ARBOR_PARALLEL "Whether to use multithreading (requires OpenMP)" OFF )

### SOURCE ##################################################################

#include directories
INCLUDE_DIRECTORIES( ./include )
INSTALL_DIRECTORY( ./include DESTINATION . FILES_MATCHING PATTERN "*.h" )

AUX_SOURCE_DIRECTORY( ./src/ ArborContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/ArborApi ArborContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/ArborObjects ArborContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/ArborCheating ArborContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/ArborClustering ArborContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/ArborHelpers ArborContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/ArborMonitoring ArborContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/ArborPfoConstruction ArborContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/ArborPlugins ArborContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/ArborReclustering ArborContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/ArborTools ArborContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/ArborTopologicalAssociation ArborContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/ArborTrackClusterAssociation ArborContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/ArborUtility ArborContent_SRCS )

### C++11 check #############################################################

INCLUDE( CheckCXXCompilerFlag )

UNSET( COMPILER_SUPPORTS_CXX11 CACHE )
CHECK_CXX_COMPILER_FLAG( "-std=c++11" COMPILER_SUPPORTS_CXX11 )

IF( NOT CMAKE_CXX_FLAGS )
    SET( CMAKE_CXX_FLAGS "-std=c++11" )
    SET( PROCEED_WITHOUT_CXX_FLAGS ON )
ENDIF()

UNSET( COMPILER_SUPPORTS_CXX_FLAGS CACHE )
CHECK_CXX_COMPILER_FLAG( ${CMAKE_CXX_FLAGS} COMPILER_SUPPORTS_CXX_FLAGS )
IF( NOT COMPILER_SUPPORTS_CXX_FLAGS )
    MESSAGE( STATUS "The compiler ${CMAKE_CXX_COMPILER} does not support cxx flags ${CMAKE_CXX_FLAGS}" )
    IF( NOT PROCEED_WITHOUT_CXX_FLAGS )
        MESSAGE( FATAL_ERROR "CMake abort." )
    ENDIF()
    MESSAGE( STATUS "Proceed without cxx flags ${CMAKE_CXX_FLAGS}" )
    UNSET( CMAKE_CXX_FLAGS )
ENDIF()

SET( CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -ansi -pedantic -Wno-long-long -Wno-sign-compare -Wshadow -fno-strict-aliasing ${CMAKE_CXX_FLAGS}" )

# -------------- packages -------------
FIND_PACKAGE( PandoraSDK 02.00.00 REQUIRED )

FOREACH( pkg PandoraSDK )
  IF( ${pkg}_FOUND )
    INCLUDE_DIRECTORIES( ${${pkg}_INCLUDE_DIRS} )
    LINK_LIBRARIES( ${${pkg}_LIBRARIES} )
    ADD_DEFINITIONS ( ${${pkg}_DEFINITIONS} )
  ENDIF()
ENDFOREACH()

IF( PANDORA_MONITORING )

  FIND_PACKAGE( ROOT 5.26.00 REQUIRED COMPONENTS Eve Geom RGL EG ) # minimum required ROOT version
  INCLUDE_DIRECTORIES( ${ROOT_INCLUDE_DIRS} )
  LINK_LIBRARIES( ${ROOT_LIBRARIES} ${ROOT_COMPONENT_LIBRARIES} )

  FIND_PACKAGE( PandoraMonitoring REQUIRED )
  
  IF( PandoraMonitoring_FOUND )
    INCLUDE_DIRECTORIES( ${PandoraMonitoring_INCLUDE_DIRS} )
    LINK_LIBRARIES( ${PandoraMonitoring_LIBRARIES} )
    ADD_DEFINITIONS ( ${PandoraMonitoring_DEFINITIONS} )
    ADD_DEFINITIONS( "-DMONITORING" )
  ENDIF()

ENDIF()

IF( ARBOR_PARALLEL )
    FIND_PACKAGE( OpenMP )
    
    IF( NOT OPENMP_FOUND )
        MESSAGE( FATAL "Couldn't find OpenMP ! Please set the ARBOR_PARALLEL option to OFF" )
    ENDIF()
    
    ADD_DEFINITIONS( ${OpenMP_CXX_FLAGS} )
    ADD_DEFINITIONS( "-DARBOR_PARALLEL" )
ENDIF()


OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )

IF( INSTALL_DOC AND EXISTS "${PROJECT_SOURCE_DIR}/doc/CMakeLists.txt" )
    ADD_SUBDIRECTORY( ./doc )
ELSE()
    MESSAGE( STATUS "INSTALL_DOC set to OFF" )
    SET( INSTALL_DOC OFF )
ENDIF()

### LIBRARY #################################################################

# add library
ADD_SHARED_LIBRARY( ${PROJECT_NAME} ${ArborContent_SRCS} )

IF( ARBOR_PARALLEL )
    TARGET_LINK_LIBRARIES( ${PROJECT_NAME} gomp )
ENDIF()


### INSTALL #################################################################

# install library
INSTALL_SHARED_LIBRARY( ArborContent DESTINATION lib )

# install header files
INSTALL_DIRECTORY( ./include DESTINATION . )

# display some variables and write them to cache
DISPLAY_STD_VARIABLES()

# generate and install following configuration files
GENERATE_PACKAGE_CONFIGURATION_FILES( ${PROJECT_NAME}Config.cmake ${PROJECT_NAME}ConfigVersion.cmake ${PROJECT_NAME}LibDeps.cmake )

