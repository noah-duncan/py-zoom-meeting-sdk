cmake_minimum_required(VERSION 3.20.2)
project(zoom_sdk_python VERSION 1.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(ZOOM_SDK lib/zoomsdk)


# Find pybind11
execute_process(
    COMMAND python3 -m pybind11 --cmakedir
    OUTPUT_VARIABLE PYBIND11_CMAKE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
find_package(pybind11 CONFIG REQUIRED PATHS ${PYBIND11_CMAKE_DIR} NO_DEFAULT_PATH)


find_package(ada REQUIRED)
find_package(CLI11 REQUIRED)
find_path(JWT_CPP_INCLUDE_DIRS "jwt-cpp/base.h")

find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})
link_directories(${X11_LIBRARIES})

find_package(PkgConfig REQUIRED)
pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0 gio-2.0)

include_directories(${ZOOM_SDK}/h)
link_directories(${ZOOM_SDK} ${ZOOM_SDK})
link_directories(${ZOOM_SDK} ${ZOOM_SDK}/qt_libs/**)

# Create the Python module
pybind11_add_module(zoom_sdk_python src/pythonlib.cpp
        lib/zoomsdk/h/zoom_sdk.h
        src/events/AuthServiceEvent.cpp
        src/events/AuthServiceEvent.h
)

target_include_directories(zoom_sdk_python PRIVATE ${JWT_CPP_INCLUDE_DIRS})
target_link_libraries(zoom_sdk_python PRIVATE meetingsdk ada::ada CLI11::CLI11 PkgConfig::deps ${X11_LIBRARIES})

# Set the output directory for the Python module
set_target_properties(zoom_sdk_python PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/python
)
