# zoom_sdk_wrapper.pyx
from zoom_sdk cimport InitParam, SDKError, RawDataOptions, SDK_LANGUAGE_ID
from libc.stdlib cimport malloc, free
from libc.string cimport memset
from cpython.mem cimport PyMem_Malloc, PyMem_Free
from cpython.unicode cimport PyUnicode_AsUTF8
from libc.stddef cimport wchar_t
from libc.string cimport memset, strcpy, strlen

ctypedef char zchar_t


cdef class PyInitParam:
    cdef InitParam _c_struct  # The internal C struct

    # Automatically expose the C struct fields as Python attributes
    cdef public  zchar_t* strWebDomain
    cdef public  zchar_t* strBrandingName
    cdef public  zchar_t* strSupportUrl

    def __init__(self, str strWebDomain, str strBrandingName, str strSupportUrl):
        # Initialize the C struct fields
        self.strWebDomain = PyUnicode_AsUTF8(strWebDomain)
        self.strBrandingName = PyUnicode_AsUTF8(strBrandingName)
        self.strSupportUrl = PyUnicode_AsUTF8(strSupportUrl)
