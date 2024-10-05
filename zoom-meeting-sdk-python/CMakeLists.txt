cmake_minimum_required(VERSION 3.15...3.27)
project(zoom_meeting_sdk_python LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Debug)

if (NOT SKBUILD)
  message(WARNING "\
  This CMake file is meant to be executed using 'scikit-build-core'.
  Running it directly will almost certainly not produce the desired
  result. If you are a user trying to install this package, use the
  command below, which will install all necessary build dependencies,
  compile the package in an isolated environment, and then install it.
  =====================================================================
   $ pip install .
  =====================================================================
  If you are a software developer, and this is your own package, then
  it is usually much more efficient to install the build dependencies
  in your environment once and use the following command that avoids
  a costly creation of a new virtual environment at every compilation:
  =====================================================================
   $ pip install nanobind scikit-build-core[pyproject]
   $ pip install --no-build-isolation -ve .
  =====================================================================
  You may optionally add -Ceditable.rebuild=true to auto-rebuild when
  the package is imported. Otherwise, you need to rerun the above
  after editing C++ files.")
endif()

find_package(Python 3.10
  REQUIRED COMPONENTS Interpreter Development.Module
  OPTIONAL_COMPONENTS Development.SABIModule)

find_package(nanobind CONFIG REQUIRED)

set(ZOOM_SDK src/zoomsdk)
set(ZOOM_SAMPLE_APP src/zoom_sample_app)
include_directories(${ZOOM_SDK} ${ZOOM_SAMPLE_APP})

# Link against zoom sdk static lib
set(ZOOM_SDK_LIB_DIR /tmp/python-zoom-linux-sdk/lib/zoomsdk)
link_directories(${ZOOM_SDK_LIB_DIR} ${ZOOM_SDK_LIB_DIR})

# Link against some required libs that should be in the system
find_package(PkgConfig REQUIRED)
pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0 gio-2.0)

nanobind_add_module(
  # Name of the extension
  _zoom_meeting_sdk_python_impl

  # Target the stable ABI for Python 3.12+, which reduces
  # the number of binary wheels that must be built. This
  # does nothing on older Python versions
  STABLE_ABI

  # Source code goes here
  src/zoom_meeting_sdk_python.cpp
  src/m1.cpp
  src/m2.cpp
  src/zoomsdk/zoom_sdk.h
  src/zoom_sample_app/events/AuthServiceEvent.cpp
  src/zoom_sample_app/events/AuthServiceEvent.h
  src/zoom_sample_app/events/MeetingServiceEvent.cpp
  src/zoom_sample_app/events/MeetingServiceEvent.h
  src/zoom_sample_app/events/MeetingReminderEvent.cpp
  src/zoom_sample_app/events/MeetingReminderEvent.h
  src/zoom_sample_app/events/MeetingRecordingCtrlEvent.cpp
  src/zoom_sample_app/events/MeetingRecordingCtrlEvent.h
  src/zoom_sample_app/raw_record/ZoomSDKAudioRawDataDelegate.cpp
  src/zoom_sample_app/raw_record/ZoomSDKAudioRawDataDelegate.h
  src/zoom_sample_app/util/SocketServer.cpp
  src/zoom_sample_app/util/SocketServer.h
)

target_link_libraries(_zoom_meeting_sdk_python_impl PRIVATE 
    PkgConfig::deps 
    -L${ZOOM_SDK_LIB_DIR} -lmeetingsdk
)

# Install directive for scikit-build-core
install(TARGETS _zoom_meeting_sdk_python_impl LIBRARY DESTINATION zoom_meeting_sdk_python)



