################################################################################
#
# SmokeSignal CMake file
#
# Authors: Joseph Kuziel
# Date: 2013-11-30
#
################################################################################


cmake_minimum_required(VERSION 2.8)
project(SmokeSignal)

set(SMOKESIGNAL_VERSION_MAJOR 1)
set(SMOKESIGNAL_VERSION_MINOR 2)
set(SMOKESIGNAL_VERSION_PATCH 0)

# include version in code
configure_file (
  "${PROJECT_SOURCE_DIR}/version.h.in"
  "${PROJECT_SOURCE_DIR}/version.h"
  )

# include all header and source files under the src directory
file(GLOB SmokeSignal_ALL_SRC
	"*.h"
	"*.cpp"
)

# C++11
if(NOT MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()


## wxWidgets ###################################################################

set(wxWidgets_USE_LIBS)
find_package(wxWidgets COMPONENTS core base REQUIRED)

if(wxWidgets_FOUND)
	include("${wxWidgets_USE_FILE}")
	add_executable(SmokeSignal ${SmokeSignal_ALL_SRC})
	target_link_libraries(SmokeSignal ${wxWidgets_LIBRARIES})
else(wxWidgets_FOUND)
	message("wxWidgets not found!")
endif(wxWidgets_FOUND)

