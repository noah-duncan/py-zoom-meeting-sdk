#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/trampoline.h>
#include <nanobind/stl/function.h>

#include "zoom_sdk.h"
#include "zoom_sdk_def.h"


#include "rawdata/rawdata_audio_helper_interface.h"
#include "zoom_sdk_raw_data_def.h"
#include "rawdata/zoom_rawdata_api.h"

#include <iostream>
#include <functional>
#include <memory>

namespace nb = nanobind;
using namespace std;
using namespace ZOOMSDK;

/*
class IZoomSDKAudioRawDataDelegate
{
public:
	~IZoomSDKAudioRawDataDelegate(){}
	virtual void onMixedAudioRawDataReceived(AudioRawData* data_) = 0;
	virtual void onOneWayAudioRawDataReceived(AudioRawData* data_, uint32_t node_id) = 0;
	virtual void onShareAudioRawDataReceived(AudioRawData* data_) = 0;

	/// \brief Invoked when individual interpreter's raw audio data received
	/// \param data_ Raw audio data, see \link AudioRawData \endlink.
	/// \param pLanguageName The pointer to interpreter language name.
	virtual void onOneWayInterpreterAudioRawDataReceived(AudioRawData* data_, const zchar_t* pLanguageName) = 0;
};
*/

class ZoomSDKAudioRawDataDelegateCallbacks : public ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate {
private:
    function<void(AudioRawData*)> m_onMixedAudioRawDataReceivedCallback;
    function<void(AudioRawData*, uint32_t)> m_onOneWayAudioRawDataReceivedCallback;
    function<void(AudioRawData*)> m_onShareAudioRawDataReceivedCallback;
    function<void(AudioRawData*, const zchar_t*)> m_onOneWayInterpreterAudioRawDataReceivedCallback;

public:
    ZoomSDKAudioRawDataDelegateCallbacks(
        const function<void(AudioRawData*)>& onMixedAudioRawDataReceivedCallback = nullptr,
        const function<void(AudioRawData*, uint32_t)>& onOneWayAudioRawDataReceivedCallback = nullptr,
        const function<void(AudioRawData*)>& onShareAudioRawDataReceivedCallback = nullptr,
        const function<void(AudioRawData*, const zchar_t*)>& onOneWayInterpreterAudioRawDataReceivedCallback = nullptr
    ) : m_onMixedAudioRawDataReceivedCallback(onMixedAudioRawDataReceivedCallback),
        m_onOneWayAudioRawDataReceivedCallback(onOneWayAudioRawDataReceivedCallback),
        m_onShareAudioRawDataReceivedCallback(onShareAudioRawDataReceivedCallback),
        m_onOneWayInterpreterAudioRawDataReceivedCallback(onOneWayInterpreterAudioRawDataReceivedCallback) {}

    void onMixedAudioRawDataReceived(AudioRawData* data_) override {
        if (m_onMixedAudioRawDataReceivedCallback)
            m_onMixedAudioRawDataReceivedCallback(data_);
    }

    void onOneWayAudioRawDataReceived(AudioRawData* data_, uint32_t node_id) override {
        if (m_onOneWayAudioRawDataReceivedCallback)
            m_onOneWayAudioRawDataReceivedCallback(data_, node_id);
    }

    void onShareAudioRawDataReceived(AudioRawData* data_) override {
        if (m_onShareAudioRawDataReceivedCallback)
            m_onShareAudioRawDataReceivedCallback(data_);
    }

    void onOneWayInterpreterAudioRawDataReceived(AudioRawData* data_, const zchar_t* pLanguageName) override {
        if (m_onOneWayInterpreterAudioRawDataReceivedCallback)
            m_onOneWayInterpreterAudioRawDataReceivedCallback(data_, pLanguageName);
    }
};

void init_zoom_sdk_audio_raw_data_delegate_callbacks(nb::module_ &m) {
    nb::class_<ZoomSDKAudioRawDataDelegateCallbacks, ZOOM_SDK_NAMESPACE::IZoomSDKAudioRawDataDelegate>(m, "ZoomSDKAudioRawDataDelegateCallbacks")
        .def(nb::init<
            const function<void(AudioRawData*)>&,
            const function<void(AudioRawData*, uint32_t)>&,
            const function<void(AudioRawData*)>&,
            const function<void(AudioRawData*, const zchar_t*)>&
        >(),
        nb::arg("onMixedAudioRawDataReceivedCallback") = nullptr,
        nb::arg("onOneWayAudioRawDataReceivedCallback") = nullptr,
        nb::arg("onShareAudioRawDataReceivedCallback") = nullptr,
        nb::arg("onOneWayInterpreterAudioRawDataReceivedCallback") = nullptr
    );
}