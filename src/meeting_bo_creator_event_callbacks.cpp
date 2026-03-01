#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/trampoline.h>
#include <nanobind/stl/function.h>

#include "zoom_sdk.h"
#include "meeting_service_components/meeting_breakout_rooms_interface_v2.h"

#include <iostream>
#include <functional>
#include <memory>

namespace nb = nanobind;
using namespace ZOOMSDK;
using namespace std;

class BOCreatorEventCallbacks : public ZOOM_SDK_NAMESPACE::IBOCreatorEvent {
private:
    function<void(bool, const zchar_t*)> m_onCreateBOResponseCallback;
    function<void(PreAssignBODataStatus)> m_onWebPreAssignBODataDownloadStatusChangedCallback;
    function<void(const BOOption&)> m_onBOOptionChangedCallback;
    function<void(const zchar_t*)> m_onBOCreateSuccessCallback;
    function<void(bool, const zchar_t*)> m_onRemoveBOResponseCallback;
    function<void(bool, const zchar_t*)> m_onUpdateBONameResponseCallback;

public:
    BOCreatorEventCallbacks(
        const function<void(bool, const zchar_t*)>& onCreateBOResponseCallback = nullptr,
        const function<void(PreAssignBODataStatus)>& onWebPreAssignBODataDownloadStatusChangedCallback = nullptr,
        const function<void(const BOOption&)>& onBOOptionChangedCallback = nullptr,
        const function<void(const zchar_t*)>& onBOCreateSuccessCallback = nullptr,
        const function<void(bool, const zchar_t*)>& onRemoveBOResponseCallback = nullptr,
        const function<void(bool, const zchar_t*)>& onUpdateBONameResponseCallback = nullptr
    ) : m_onCreateBOResponseCallback(onCreateBOResponseCallback),
        m_onWebPreAssignBODataDownloadStatusChangedCallback(onWebPreAssignBODataDownloadStatusChangedCallback),
        m_onBOOptionChangedCallback(onBOOptionChangedCallback),
        m_onBOCreateSuccessCallback(onBOCreateSuccessCallback),
        m_onRemoveBOResponseCallback(onRemoveBOResponseCallback),
        m_onUpdateBONameResponseCallback(onUpdateBONameResponseCallback) {}

    void onCreateBOResponse(bool bSuccess, const zchar_t* strBOID) override {
        if (m_onCreateBOResponseCallback)
            m_onCreateBOResponseCallback(bSuccess, strBOID);
    }

    void OnWebPreAssignBODataDownloadStatusChanged(PreAssignBODataStatus status) override {
        if (m_onWebPreAssignBODataDownloadStatusChangedCallback)
            m_onWebPreAssignBODataDownloadStatusChangedCallback(status);
    }

    void OnBOOptionChanged(const BOOption& newOption) override {
        if (m_onBOOptionChangedCallback)
            m_onBOOptionChangedCallback(newOption);
    }

    void onBOCreateSuccess(const zchar_t* strBOID) override {
        if (m_onBOCreateSuccessCallback)
            m_onBOCreateSuccessCallback(strBOID);
    }

    void onRemoveBOResponse(bool bSuccess, const zchar_t* strBOID) override {
        if (m_onRemoveBOResponseCallback)
            m_onRemoveBOResponseCallback(bSuccess, strBOID);
    }

    void onUpdateBONameResponse(bool bSuccess, const zchar_t* strBOID) override {
        if (m_onUpdateBONameResponseCallback)
            m_onUpdateBONameResponseCallback(bSuccess, strBOID);
    }
};

void init_meeting_bo_creator_event_callbacks(nb::module_ &m) {
    nb::class_<BOCreatorEventCallbacks, ZOOM_SDK_NAMESPACE::IBOCreatorEvent>(m, "BOCreatorEventCallbacks")
        .def(nb::init<
            const function<void(bool, const zchar_t*)>&,
            const function<void(PreAssignBODataStatus)>&,
            const function<void(const BOOption&)>&,
            const function<void(const zchar_t*)>&,
            const function<void(bool, const zchar_t*)>&,
            const function<void(bool, const zchar_t*)>&
        >(),
            nb::arg("onCreateBOResponseCallback") = nullptr,
            nb::arg("onWebPreAssignBODataDownloadStatusChangedCallback") = nullptr,
            nb::arg("onBOOptionChangedCallback") = nullptr,
            nb::arg("onBOCreateSuccessCallback") = nullptr,
            nb::arg("onRemoveBOResponseCallback") = nullptr,
            nb::arg("onUpdateBONameResponseCallback") = nullptr
        );
}
