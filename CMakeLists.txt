cmake_minimum_required(VERSION 3.15...3.27)
project(zoom_meeting_sdk LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Debug)

# Add rpath configuration
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

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

set(ZOOM_SDK src/zoomsdk/h)
include_directories(${ZOOM_SDK})

# Link against zoom sdk static lib
set(ZOOM_SDK_LIB_DIR src/zoomsdk)
link_directories(${ZOOM_SDK_LIB_DIR} ${ZOOM_SDK_LIB_DIR})

# Link against some required libs that should be in the system
find_package(PkgConfig REQUIRED)
pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0 gio-2.0)

nanobind_add_module(
  # Name of the extension
  _zoom_meeting_sdk_impl

  # Target the stable ABI for Python 3.12+, which reduces
  # the number of binary wheels that must be built. This
  # does nothing on older Python versions
  STABLE_ABI

  # Source code goes here
  src/module.cpp

  src/binding/auth_service_interface_binding.cpp
  src/binding/meeting_service_interface_binding.cpp
  src/binding/zoom_rawdata_api_binding.cpp
  src/binding/zoom_sdk_raw_data_def_interface_binding.cpp
  src/binding/meeting_recording_interface_binding.cpp
  src/binding/rawdata_audio_helper_interface_binding.cpp
  src/binding/zoom_sdk_binding.cpp
  src/binding/meeting_reminder_ctrl_interface_binding.cpp
  src/binding/setting_service_interface_binding.cpp
  src/binding/zoom_sdk_def_binding.cpp
  src/binding/meeting_participants_ctrl_interface_binding.cpp

  src/auth_service_event_callbacks.cpp
  src/meeting_service_event_callbacks.cpp
  src/meeting_reminder_event_callbacks.cpp
  src/zoom_sdk_audio_raw_data_delegate_callbacks.cpp
  src/zoom_sdk_virtual_audio_mic_event_callbacks.cpp
  src/meeting_recording_ctrl_event_callbacks.cpp

  src/zoomsdk/h/zoom_sdk.h
)

target_link_libraries(_zoom_meeting_sdk_impl PRIVATE 
    PkgConfig::deps 
    -L${ZOOM_SDK_LIB_DIR} -lmeetingsdk
)

# Set rpath for the target
set_target_properties(_zoom_meeting_sdk_impl PROPERTIES
    INSTALL_RPATH ${ZOOM_SDK_LIB_DIR}
)

# Install directive for scikit-build-core
install(TARGETS _zoom_meeting_sdk_impl LIBRARY DESTINATION zoom_meeting_sdk)