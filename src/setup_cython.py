from setuptools import setup, Extension
from Cython.Build import cythonize
import subprocess

ext_modules = [
    Extension(
        "zoom_sdk_wrapper_cython",
        ["zoom_sdk.pyx"],
        libraries=['meetingsdk', 'glib-2.0', 'gio-2.0'],  # Add the name of the Zoom SDK library
        library_dirs=["../lib/zoomsdk"],  # Add the path to the Zoom SDK library
        include_dirs=["../lib/zoomsdk/h"],  # Add the path to the Zoom SDK headers
        language="c++",
        extra_compile_args=['-fpermissive'],
        extra_link_args=[f'-Wl,-rpath,../lib/zoomsdk']
    )
]

setup(
    name="zoom_sdk_wrapper_cython",
    ext_modules=cythonize(ext_modules),
)
