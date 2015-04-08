#################################################
# cmake file for building ArborContent framework
# @author Eté Rémi ,IPNL
# Copyright (c) CNRS / IPNL
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 FATAL_ERROR )
#################################################

# project name
PROJECT( ArborContent )

SET( ${PROJECT_NAME}_VERSION_MAJOR 01 )
SET( ${PROJECT_NAME}_VERSION_MINOR 00 )
SET( ${PROJECT_NAME}_VERSION_PATCH 00 )

# --------------- ilcsoft dependencies -----------
FIND_PACKAGE( ILCUTIL COMPONENTS ILCSOFT_CMAKE_MODULES REQUIRED )
INCLUDE( ilcsoft_default_settings )

# -------------- packages -------------
FIND_PACKAGE( PandoraSDK 01.00.00 REQUIRED )

FOREACH( pkg PandoraSDK )
  IF( ${pkg}_FOUND )
    INCLUDE_DIRECTORIES( ${${pkg}_INCLUDE_DIRS} )
    LINK_LIBRARIES( ${${pkg}_LIBRARIES} )
    ADD_DEFINITIONS ( ${${pkg}_DEFINITIONS} )
  ENDIF()
ENDFOREACH()

#IF( ARBOR_MONITORING )
#  FIND_PACKAGE( PandoraMonitoring REQUIRED )
#  IF( PandoraMonitoring_FOUND )
#    INCLUDE_DIRECTORIES( ${PandoraMonitoring_INCLUDE_DIRS} )
#    LINK_LIBRARIES( ${PandoraMonitoring_LIBRARIES} )
#    ADD_DEFINITIONS ( ${PandoraMonitoring_DEFINITIONS} )
#    ADD_DEFINITIONS( "-DARBOR_MONITORING" )
#  ENDIF()
#ENDIF()

OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )

IF( INSTALL_DOC AND EXISTS "${PROJECT_SOURCE_DIR}/doc/CMakeLists.txt" )
    ADD_SUBDIRECTORY( ./doc )
ELSE()
    MESSAGE( STATUS "INSTALL_DOC set to OFF" )
    SET( INSTALL_DOC OFF )
ENDIF()


### SOURCE ##################################################################

#include directories
INCLUDE_DIRECTORIES( ./include )
INSTALL_DIRECTORY( ./include DESTINATION . FILES_MATCHING PATTERN "*.h" )

AUX_SOURCE_DIRECTORY( ./src/ ArborContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/ArborApi ArborContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/ArborObjects ArborContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/ArborClustering ArborContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/ArborHelpers ArborContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/ArborMonitoring ArborContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/ArborPfoConstruction ArborContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/ArborPlugins ArborContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/ArborTopologicalAssociation ArborContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/ArborTrackClusterAssociation ArborContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/ArborUtility ArborContent_SRCS )

# require proper C++ and need long long for int64 for now
ADD_DEFINITIONS( "-Wall -ansi -pedantic -Wno-long-long -Wno-sign-compare -fno-strict-aliasing" )

### LIBRARY #################################################################

# add library
ADD_SHARED_LIBRARY( ${PROJECT_NAME} ${ArborContent_SRCS} )


### INSTALL #################################################################

# install library
INSTALL_SHARED_LIBRARY( ArborContent DESTINATION lib )

# install header files
INSTALL_DIRECTORY( ./include DESTINATION . )

# display some variables and write them to cache
DISPLAY_STD_VARIABLES()

# generate and install following configuration files
GENERATE_PACKAGE_CONFIGURATION_FILES( ${PROJECT_NAME}Config.cmake ${PROJECT_NAME}ConfigVersion.cmake ${PROJECT_NAME}LibDeps.cmake )

