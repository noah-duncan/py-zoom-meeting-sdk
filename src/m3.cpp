#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/trampoline.h>
#include <nanobind/stl/function.h>

#include "util/SocketServer.h"

#include <iostream>
#include <functional>
#include <memory>

namespace nb = nanobind;

void init_m3(nb::module_ &m) {
    nb::class_<SocketServer>(m, "SocketServer")
    .def(nb::init<>())
    .def("start", &SocketServer::start)
    .def("stop", &SocketServer::stop)
    .def("writeBuf", &SocketServer::writeBuf)
    .def("writeStr", &SocketServer::writeStr)
    .def("isReady", &SocketServer::isReady)
    .def("cleanup", &SocketServer::cleanup);
}