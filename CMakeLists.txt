cmake_minimum_required(VERSION 3.15...3.27)
project(my_project) # Replace 'my_project' with the name of your project

if (CMAKE_VERSION VERSION_LESS 3.18)
  set(DEV_MODULE Development)
else()
  set(DEV_MODULE Development.Module)
endif()

find_package(Python 3.10 COMPONENTS Interpreter ${DEV_MODULE} REQUIRED)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

execute_process(
  COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir
  OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE nanobind_ROOT)
find_package(nanobind CONFIG REQUIRED)

set(ZOOM_SDK lib/zoomsdk)
find_package(ada REQUIRED)
find_package(CLI11 REQUIRED)
find_path(JWT_CPP_INCLUDE_DIRS "jwt-cpp/base.h")

find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})
link_directories(${X11_LIBRARIES})

link_directories(${ZOOM_SDK} ${ZOOM_SDK})
link_directories(${ZOOM_SDK} ${ZOOM_SDK}/qt_libs/**)

find_package(PkgConfig REQUIRED)
pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0 gio-2.0)

include_directories(${ZOOM_SDK}/h)

nanobind_add_module(zoom_sdk_python src/pythonlib.cpp
    lib/zoomsdk/h/zoom_sdk.h
    src/events/AuthServiceEvent.cpp
    src/events/AuthServiceEvent.h
    src/events/MeetingServiceEvent.cpp
    src/events/MeetingServiceEvent.h     
    src/events/MeetingReminderEvent.cpp
    src/events/MeetingReminderEvent.h        
    src/events/MeetingRecordingCtrlEvent.cpp
    src/events/MeetingRecordingCtrlEvent.h 
    src/raw_record/ZoomSDKAudioRawDataDelegate.cpp
    src/raw_record/ZoomSDKAudioRawDataDelegate.h
    src/util/SocketServer.cpp
    src/util/SocketServer.h
)

target_link_libraries(zoom_sdk_python PRIVATE 
    ada::ada 
    CLI11::CLI11 
    PkgConfig::deps 
    ${X11_LIBRARIES}
    -L${ZOOM_SDK} -lmeetingsdk
)

set_target_properties(zoom_sdk_python PROPERTIES
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib;${ZOOM_SDK}"
    BUILD_WITH_INSTALL_RPATH TRUE
)