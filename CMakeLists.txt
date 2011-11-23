project(epics)

cmake_minimum_required(VERSION 2.8)

# specify boost version used
set(KEY_WIN_BOOST_VERSION "1.43")
# convert point to underscore in version name
STRING(REGEX REPLACE "\\." "_" KEY_WIN_BOOST_VERSION_UNDERSCORED ${KEY_WIN_BOOST_VERSION})
# set mingw version (required to select appropriate boost library when linking)
set(KEY_MINGW_VERSION "34")

set(KEY_INCLUDES
	"${CMAKE_SOURCE_DIR}/../third-party-include")

# Attempt to guess the compiler suffix
# stolen from FindBoost cmake file
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel"
	OR "${CMAKE_CXX_COMPILER}" MATCHES "icl" 
	OR "${CMAKE_CXX_COMPILER}" MATCHES "icpc")
  if(WIN32)
	set (KEY_LIB_COMPILER "iw")
  else()
	set (KEY_LIB_COMPILER "il")
  endif()
elseif (MSVC90)
  SET (KEY_LIB_COMPILER "vc90")
elseif (MSVC10)
  SET (KEY_LIB_COMPILER "vc100")
elseif (MSVC11)
  SET (KEY_LIB_COMPILER "vc110")
elseif (MSVC80)
  SET (KEY_LIB_COMPILER "vc80")
elseif (MSVC71)
  SET (KEY_LIB_COMPILER "vc71")
elseif (MSVC70) # Good luck!
  SET (KEY_LIB_COMPILER "vc7") # yes, this is correct
elseif (MSVC60) # Good luck!
  SET (KEY_LIB_COMPILER "vc6") # yes, this is correct
elseif (BORLAND)
  SET (KEY_LIB_COMPILER "bcb")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "SunPro")
  set(KEY_LIB_COMPILER "sw")
elseif (MINGW)
  SET(KEY_LIB_COMPILER "mgw") # no GCC version encoding prior to 1.34
elseif (UNIX)
  if (CMAKE_COMPILER_IS_GNUCXX)
	  IF(APPLE)
		SET(KEY_LIB_COMPILER "xgcc")
	  ELSE()
		SET(KEY_LIB_COMPILER "gcc")
	  ENDIF()
  endif (CMAKE_COMPILER_IS_GNUCXX)
endif()

message("CMAKE_SIZEOF_VOID_P = ${CMAKE_SIZEOF_VOID_P}")

# detect architecture for windows
if (WIN32)
	if (CMAKE_CL_64 MATCHES 1)
		set(KEY_LIB_ARCH "x64")
		message("64-bit Windows")
	else()
		set(KEY_LIB_ARCH "x86")
		message("32-bit Windows")
	endif()
elseif(NOT APPLE)
	if(${CMAKE_SIZEOF_VOID_P} MATCHES "8")
		set(KEY_LIB_ARCH "x64")
		message("64-bit Linux")
	else()
		set(KEY_LIB_ARCH "x86")
		message("32-bit Linux")
	endif()
else()
	message("Sytem architecture detection may be incorrect for APPLE")
	set(KEY_LIB_ARCH "x64")
endif()

if (MSVC)
	set(WITH_SHARED_LIBS NO)
	set(KEY_LIBRARIES 
		"${CMAKE_SOURCE_DIR}/../third-party-lib/windows/${KEY_LIB_ARCH}-${KEY_LIB_COMPILER}/${CMAKE_CFG_INTDIR}")
else()
	if (WIN32)
		set(KEY_LIBRARIES 
		"${CMAKE_SOURCE_DIR}/../third-party-lib/windows/${KEY_LIB_ARCH}-${KEY_LIB_COMPILER}")
	else()
		set(KEY_LIBRARIES 
		"${CMAKE_SOURCE_DIR}/../third-party-lib/linux/${KEY_LIB_ARCH}-${KEY_LIB_COMPILER}")
	endif()
endif()

### BOOST

if(WIN32) # libraries for windows are prepared in known location
	find_path(Boost_INCLUDE_DIRS boost/date_time.hpp
	  PATH_SUFFIXES
	  PATHS
	  ${KEY_INCLUDES}
	)
	message("Using windows...")
	set(Boost_LOCAL FALSE)
	if (MSVC)
		message("Using msvc for boost on windows...")
		if (Boost_INCLUDE_DIRS)
			set(Boost_LOCAL TRUE)
		endif()
	elseif(MINGW)
		message("Using mingw for boost on windows...")
		set(Boost_LIBRARIES "libboost_thread-mgw${KEY_MINGW_VERSION}-mt-${KEY_WIN_BOOST_VERSION_UNDERSCORED}.a")
		set(Boost_FOUND TRUE)
	endif()
else()
	message("Searching for boost...")
	set(Boost_LOCAL FALSE)
	set(Boost_USE_STATIC_LIBS   ON)
	set(Boost_USE_MULTITHREADED ON)
	set(Boost_ADDITIONAL_VERSIONS "1.35" "1.35.0" "1.35.1" "1.36" "1.36.0" "1.36.1" "1.37" "1.37.0" "1.38" "1.38.0" "1.39" "1.39.0" "1.40" "1.40.0" "1.41" "1.41.0" "1.42.0" "1.43.0" "1.47")
	find_package( Boost REQUIRED COMPONENTS date_time filesystem system thread)
endif()

if(Boost_FOUND OR Boost_LOCAL)
	include_directories(${Boost_INCLUDE_DIRS})
endif()

if(NOT (Boost_FOUND OR Boost_LOCAL))
	message("Boost libraries were not found. Please install libboost-all-dev libraries.")
endif()

#JSON

if (WIN32)
set(JSON_LIBRARY "json_vc71")
else()
set(JSON_LIBRARY "libjson.a")
endif()

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${KEY_INCLUDES})
link_directories(${KEY_LIBRARIES})

add_subdirectory(key-common)
add_subdirectory(key-net)
add_subdirectory(epics-libclient)
add_subdirectory(epics-proxy)
add_subdirectory(epics-auth)
add_subdirectory(epics-masterserver)
