#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/trampoline.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/vector.h>
#include "zoom_sdk.h"
#include "zoom_sdk_def.h"
#include "rawdata/rawdata_video_source_helper_interface.h"
#include "zoom_sdk_raw_data_def.h"
#include "rawdata/zoom_rawdata_api.h"

#include <iostream>
#include <functional>
#include <memory>
#include <vector>

namespace nb = nanobind;
using namespace ZOOMSDK;
using namespace std;

class ZoomSDKVideoSourceCallbacks : public ZOOM_SDK_NAMESPACE::IZoomSDKVideoSource {
private:
    function<void(IZoomSDKVideoSender*, vector<VideoSourceCapability>, VideoSourceCapability)> m_onInitializeCallback;
    function<void(vector<VideoSourceCapability>, VideoSourceCapability)> m_onPropertyChangeCallback;
    function<void()> m_onStartSendCallback;
    function<void()> m_onStopSendCallback;
    function<void()> m_onUninitializedCallback;

public:
    ZoomSDKVideoSourceCallbacks(
        const function<void(IZoomSDKVideoSender*, vector<VideoSourceCapability>, VideoSourceCapability)>& onInitializeCallback = nullptr,
        const function<void(vector<VideoSourceCapability>, VideoSourceCapability)>& onPropertyChangeCallback = nullptr,
        const function<void()>& onStartSendCallback = nullptr,
        const function<void()>& onStopSendCallback = nullptr,
        const function<void()>& onUninitializedCallback = nullptr
    ) : m_onInitializeCallback(onInitializeCallback),
        m_onPropertyChangeCallback(onPropertyChangeCallback),
        m_onStartSendCallback(onStartSendCallback),
        m_onStopSendCallback(onStopSendCallback),
        m_onUninitializedCallback(onUninitializedCallback) {}

    void onInitialize(IZoomSDKVideoSender* sender, IList<VideoSourceCapability>* support_cap_list, VideoSourceCapability& suggest_cap) override {
        if (m_onInitializeCallback) {
            vector<VideoSourceCapability> caps;
            if (support_cap_list) {
                int count = support_cap_list->GetCount();
                caps.reserve(count);
                for (int i = 0; i < count; i++) {
                    caps.push_back(support_cap_list->GetItem(i));
                }
            }
            m_onInitializeCallback(sender, caps, suggest_cap);
        }
    }

    void onPropertyChange(IList<VideoSourceCapability>* support_cap_list, VideoSourceCapability suggest_cap) override {
        if (m_onPropertyChangeCallback) {
            vector<VideoSourceCapability> caps;
            if (support_cap_list) {
                int count = support_cap_list->GetCount();
                caps.reserve(count);
                for (int i = 0; i < count; i++) {
                    caps.push_back(support_cap_list->GetItem(i));
                }
            }
            m_onPropertyChangeCallback(caps, suggest_cap);
        }
    }

    void onStartSend() override {
        if (m_onStartSendCallback)
            m_onStartSendCallback();
    }

    void onStopSend() override {
        if (m_onStopSendCallback)
            m_onStopSendCallback();
    }

    void onUninitialized() override {
        if (m_onUninitializedCallback)
            m_onUninitializedCallback();
    }
};

void init_zoom_sdk_video_source_callbacks(nb::module_ &m) {
    nb::class_<ZoomSDKVideoSourceCallbacks, ZOOM_SDK_NAMESPACE::IZoomSDKVideoSource>(m, "ZoomSDKVideoSourceCallbacks")
        .def(nb::init<
            function<void(IZoomSDKVideoSender*, vector<VideoSourceCapability>, VideoSourceCapability)>&,
            function<void(vector<VideoSourceCapability>, VideoSourceCapability)>&,
            function<void()>&,
            function<void()>&,
            function<void()>&
        >(),
            nb::arg("onInitializeCallback") = nullptr,
            nb::arg("onPropertyChangeCallback") = nullptr,
            nb::arg("onStartSendCallback") = nullptr,
            nb::arg("onStopSendCallback") = nullptr,
            nb::arg("onUninitializedCallback") = nullptr
        );
}