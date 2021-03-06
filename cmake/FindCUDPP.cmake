# - Find CUDPP
# Find the CUDPP includes and library
#
#  CUDPP_INCLUDE_DIR - Where to find CUDPP includes
#  CUDPP_LIBRARIES   - List of libraries when using CUDPP
#  CUDPP_FOUND       - True if CUDPP was found

IF(CUDPP_INCLUDE_DIR)
  SET(CUDPP_FIND_QUIETLY TRUE)
ENDIF(CUDPP_INCLUDE_DIR)

FIND_PATH(CUDPP_INCLUDE_DIR NAMES "cudpp.h"
  PATHS
  $ENV{NVSDKCOMPUTE_ROOT}/C/common/inc/cudpp
  $ENV{NVSDKCOMPUTE_ROOT}/C/common/inc
  $ENV{EXTERNLIBS}/cuda/C/common/inc/cudpp
  $ENV{EXTERNLIBS}/cuda/C/common/inc
  $ENV{EXTERNLIBS}/cudpp/include/cudpp
  $ENV{EXTERNLIBS}/cudpp/include
  $ENV{EXTERNLIBS}/cuda/include/cudpp
  $ENV{EXTERNLIBS}/cuda/include
  DOC "CUDPP - Headers"
)

SET(CUDPP_NAMES cudpp64 cudpp_x86_64 cudpp)
SET(CUDPP_DBG_NAMES cudpp64d)

FIND_LIBRARY(CUDPP_LIBRARY NAMES ${CUDPP_NAMES}
  PATHS
  $ENV{NVSDKCOMPUTE_ROOT}/C/common/lib/linux
  $ENV{EXTERNLIBS}/cuda/C/common/lib/linux
  $ENV{EXTERNLIBS}/cudpp
  $ENV{EXTERNLIBS}/cuda
  PATH_SUFFIXES lib lib64
  DOC "CUDPP - Library"
)

INCLUDE(FindPackageHandleStandardArgs)

IF(MSVC)
  # VisualStudio needs a debug version
  FIND_LIBRARY(CUDPP_LIBRARY_DEBUG NAMES ${CUDPP_DBG_NAMES}
    PATHS
    $ENV{CUDPP_HOME}/lib
    $ENV{EXTERNLIBS}/cudpp/lib
    DOC "CUDPP - Library (Debug)"
  )
  
  IF(CUDPP_LIBRARY_DEBUG AND CUDPP_LIBRARY)
    SET(CUDPP_LIBRARIES optimized ${CUDPP_LIBRARY} debug ${CUDPP_LIBRARY_DEBUG})
  ENDIF(CUDPP_LIBRARY_DEBUG AND CUDPP_LIBRARY)

  FIND_PACKAGE_HANDLE_STANDARD_ARGS(CUDPP DEFAULT_MSG CUDPP_LIBRARY CUDPP_LIBRARY_DEBUG CUDPP_INCLUDE_DIR)

  MARK_AS_ADVANCED(CUDPP_LIBRARY CUDPP_LIBRARY_DEBUG CUDPP_INCLUDE_DIR)
  
ELSE(MSVC)
  # rest of the world
  SET(CUDPP_LIBRARIES ${CUDPP_LIBRARY})
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(CUDPP DEFAULT_MSG CUDPP_LIBRARY CUDPP_INCLUDE_DIR)
  MARK_AS_ADVANCED(CUDPP_LIBRARY CUDPP_INCLUDE_DIR)
  
ENDIF(MSVC)

IF(CUDPP_FOUND)
  SET(CUDPP_INCLUDE_DIRS ${CUDPP_INCLUDE_DIR})
ENDIF(CUDPP_FOUND)
