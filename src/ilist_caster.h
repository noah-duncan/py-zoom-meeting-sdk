#pragma once

#include <nanobind/nanobind.h>
#include <iostream>
#include "zoom_sdk_def.h"

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

template <typename Entry> struct ilist_caster {
    NB_TYPE_CASTER(ZOOMSDK::IList<Entry>*, io_name(NB_TYPING_SEQUENCE, NB_TYPING_LIST) +
                              const_name("[") + make_caster<Entry>::Name +
                              const_name("]"))

    using Caster = make_caster<Entry>;

    template <typename T> using has_reserve = decltype(std::declval<T>().reserve(0));

    bool from_python(handle src, uint8_t flags, cleanup_list *cleanup) noexcept {
        std::cout << "BAD522" << std::endl;
        // Python -> C++ is not supported for IList's because they do not expose a write interface
        return false;
    }

    // Python -> C++ is not supported for IList's because they do not expose a write interface
    operator ZOOMSDK::IList<Entry>*() {
        std::cout << "BAD22" << std::endl;
        return nullptr;
    }

    template <typename T>
    static handle from_cpp(T &&src, rv_policy policy, cleanup_list *cleanup) {
        size_t srcCount = src->GetCount();
        object ret = steal(PyList_New(srcCount));

        if (ret.is_valid()) {
            Py_ssize_t index = 0;

            for (size_t iListIndex = 0; iListIndex < srcCount; ++iListIndex) {
                handle h = Caster::from_cpp(forward_like_<T>(src->GetItem(iListIndex)), policy, cleanup);

                if (!h.is_valid()) {
                    ret.reset();
                    break;
                }

                NB_LIST_SET_ITEM(ret.ptr(), index++, h.ptr());
            }
        }

        return ret.release();
    }
};

template <typename Type> struct type_caster<ZOOMSDK::IList<Type>>
 : ilist_caster<Type> { };
 
NAMESPACE_END(detail)
NAMESPACE_END(NB_NAMESPACE)