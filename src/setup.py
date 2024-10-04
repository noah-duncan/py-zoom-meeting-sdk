from setuptools import setup
from setuptools.extension import Extension
from setuptools.command.build_ext import build_ext
import os
import nanobind

class ccache_build_ext(build_ext):
    def build_extensions(self):
        self.compiler.compiler_so[0] = 'g++'
        self.compiler.compiler[0] = 'g++'
        self.compiler.compiler_so = ['ccache'] + self.compiler.compiler_so
        self.compiler.compiler = ['ccache'] + self.compiler.compiler
        super().build_extensions()

zoom_sdk_path = os.path.abspath("../lib/zoomsdk")

ext_modules = [
    Extension(
        "zoom_sdk_python",
        ["pythonlib.cpp"],
        libraries=['meetingsdk', 'glib-2.0', 'gio-2.0', 'nanobind'],
        library_dirs=[zoom_sdk_path],
        include_dirs=[os.path.join(zoom_sdk_path, "h"), nanobind.include_dir()],
        runtime_library_dirs=[zoom_sdk_path],
        extra_link_args=[f'-Wl,-rpath,{zoom_sdk_path}'],
        extra_compile_args=['-std=c++17'],  # Ensure C++17 support for nanobind
    ),
]

setup(
    name="zoom_sdk_python",
    ext_modules=ext_modules,
    cmdclass={"build_ext": ccache_build_ext},
    setup_requires=["nanobind"],
)