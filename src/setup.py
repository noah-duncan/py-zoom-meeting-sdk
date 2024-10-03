from setuptools import setup, Extension
from pybind11.setup_helpers import Pybind11Extension, build_ext
import os

class ccache_build_ext(build_ext):
    def build_extensions(self):
        #print("DOG")
        #print(self.compiler.compiler_so)
        self.compiler.compiler_so[0] = 'g++'
        self.compiler.compiler[0] = 'g++'
        self.compiler.compiler_so = ['ccache'] + self.compiler.compiler_so
        self.compiler.compiler = ['ccache'] + self.compiler.compiler
        #self.compiler.compiler_cxx = ['ccache'] + self.compiler.compiler_cxx
        # Prefix the compiler with ccache
        #self.compiler.set_executable('compiler_so', 'ccache ' + self.compiler.compiler_so[0])
        #self.compiler.set_executable('compiler', 'ccache ' + self.compiler.compiler[0])
        #self.compiler.set_executable('compiler_cxx', 'ccache ' + self.compiler.compiler_cxx[0])
        super().build_extensions()

zoom_sdk_path = os.path.abspath("../lib/zoomsdk")

ext_modules = [
    Pybind11Extension(
        "zoom_sdk_python",
        ["pythonlib.cpp"],
        libraries=['meetingsdk', 'glib-2.0', 'gio-2.0'],
        library_dirs=[zoom_sdk_path],  # Use the absolute path
        include_dirs=[os.path.join(zoom_sdk_path, "h")],  # Use os.path.join for portability
        runtime_library_dirs=[zoom_sdk_path],  # This sets RPATH
        extra_link_args=[f'-Wl,-rpath,{zoom_sdk_path}'],  # This also sets RPATH
    ),
]

setup(
    name="zoom_sdk_python",
    ext_modules=ext_modules,
    cmdclass={"build_ext": ccache_build_ext},
)


#SLOW FUCKER

#ccache g++ -Wno-unused-result -Wsign-compare -DNDEBUG -g -fwrapv -O0 -Wall -fstack-protector-strong -Wformat -Werror=format-security -g -fwrapv -O2 -g -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 -fPIC -I/tmp/python-zoom-linux-sdk/lib/zoomsdk/h -I/usr/local/lib/python3.10/dist-packages/pybind11/include -I/usr/include/python3.10 -c pythonlib.cpp -o build/temp.linux-x86_64-3.10/pythonlib.o -std=c++17 -fvisibility=hidden -g0






# w no ccache
#x86_64-linux-gnu-gcc -I/usr/include/python3.10 -c flagcheck.cpp -o flagcheck.o -std=c++17
#x86_64-linux-gnu-gcc -Wno-unused-result -Wsign-compare -DNDEBUG -g -fwrapv -O2 -Wall -g -fstack-protector-strong -Wformat -Werror=format-security -g -fwrapv -O2 -g -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 -fPIC -I/usr/include/python3.10 -c flagcheck.cpp -o flagcheck.o -std=c++17

#x86_64-linux-gnu-gcc -Wno-unused-result -Wsign-compare -DNDEBUG -g -fwrapv -O2 -Wall -g -fstack-protector-strong -Wformat -Werror=format-security -g -fwrapv -O2 -g -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 -fPIC -I../lib/zoomsdk/h -I/usr/local/lib/python3.10/dist-packages/pybind11/include -I/usr/include/python3.10 -c pythonlib.cpp -o build/temp.linux-x86_64-3.10/pythonlib.o -std=c++17 -fvisibility=hidden -g0

# w ccache

#root@1535ef7914ae:/tmp/python-zoom-linux-sdk/src# python3 setup.py build_ext --inplace
#running build_ext
#ccache x86_64-linux-gnu-gcc -I/usr/include/python3.10 -c flagcheck.cpp -o flagcheck.o -std=c++17
#building 'zoom_sdk_wrapper_pybind' extension



#ccache x86_64-linux-gnu-gcc -I../lib/zoomsdk/h -I/usr/local/lib/python3.10/dist-packages/pybind11/include -I/usr/include/python3.10 -c pythonlib.cpp -o build/temp.linux-x86_64-3.10/pythonlib.o -std=c++17 -fvisibility=hidden -g0
#ccache -shared -Wl,-O1 -Wl,-Bsymbolic-functions -Wl,-Bsymbolic-functions -g -fwrapv -O2 -Wl,-Bsymbolic-functions -g -fwrapv -O2 -g -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 build/temp.linux-x86_64-3.10/pythonlib.o -L../lib/zoomsdk -Wl,--enable-new-dtags,-R../lib/zoomsdk -lmeetingsdk -lglib-2.0 -lgio-2.0 -o build/lib.linux-x86_64-3.10/zoom_sdk_wrapper_pybind.cpython-310-x86_64-linux-gnu.so

#--------------

#x86_64-linux-gnu-gcc -Wno-unused-result -Wsign-compare -DNDEBUG -g -fwrapv -O2 -Wall -g -fstack-protector-strong -Wformat -Werror=format-security -g -fwrapv -O2 -g -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 -fPIC -I/usr/include/x86_64-linux-gnu/c++/11 -I/usr/include/c++/11 -I../lib/zoomsdk/h -I/usr/local/lib/python3.10/dist-packages/pybind11/include -I/usr/include/python3.10 -std=c++17 -fvisibility=hidden pch.h



#x86_64-linux-gnu-g++ -Wno-unused-result -Wsign-compare -DNDEBUG -g -fwrapv -Wall -g -fstack-protector-strong -Wformat -Werror=format-security -g -fwrapv -O2 -g -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 -fPIC -I../lib/zoomsdk/h -I/usr/local/lib/python3.10/dist-packages/pybind11/include -I/usr/include/python3.10 -include pch.hpp -c pythonlib.cpp -o build/temp.linux-x86_64-3.10/pythonlib.o -std=c++17 -fvisibility=hidden -g0 -H --verbose -Winvalid-pch

# x86_64-linux-gnu-gcc -Wno-unused-result -Wsign-compare -DNDEBUG -g -fwrapv -O2 -Wall -g -fstack-protector-strong -Wformat -Werror=format-security -g -fwrapv -O2 -g -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 -fPIC -I/usr/include/x86_64-linux-gnu/c++/11 -I/usr/include/c++/11 -I../lib/zoomsdk/h -I/usr/local/lib/python3.10/dist-packages/pybind11/include -I/usr/include/python3.10 -std=c++17 -fvisibility=hidden pch.hpp