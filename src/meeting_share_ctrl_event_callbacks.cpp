#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/trampoline.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/unique_ptr.h>
#include <nanobind/stl/vector.h>

#include "zoom_sdk.h"
#include "meeting_service_components/meeting_sharing_interface.h"

#include <iostream>
#include <functional>
#include <memory>

namespace nb = nanobind;
using namespace ZOOMSDK;

class MeetingShareCtrlEventCallbacks : public IMeetingShareCtrlEvent {
private:
    std::function<void(SharingStatus, unsigned int)> m_onSharingStatusCallback;
    std::function<void(bool)> m_onLockShareStatusCallback;
    std::function<void(ShareInfo&)> m_onShareContentNotificationCallback;
    std::function<void(IShareSwitchMultiToSingleConfirmHandler*)> m_onMultiShareSwitchToSingleShareNeedConfirmCallback;
    std::function<void(ShareSettingType)> m_onShareSettingTypeChangedNotificationCallback;
    std::function<void()> m_onSharedVideoEndedCallback;
    std::function<void(ZoomSDKVideoFileSharePlayError)> m_onVideoFileSharePlayErrorCallback;

public:
    MeetingShareCtrlEventCallbacks(
        const std::function<void(SharingStatus, unsigned int)>& onSharingStatusCallback = nullptr,
        const std::function<void(bool)>& onLockShareStatusCallback = nullptr,
        const std::function<void(ShareInfo&)>& onShareContentNotificationCallback = nullptr,
        const std::function<void(IShareSwitchMultiToSingleConfirmHandler*)>& onMultiShareSwitchToSingleShareNeedConfirmCallback = nullptr,
        const std::function<void(ShareSettingType)>& onShareSettingTypeChangedNotificationCallback = nullptr,
        const std::function<void()>& onSharedVideoEndedCallback = nullptr,
        const std::function<void(ZoomSDKVideoFileSharePlayError)>& onVideoFileSharePlayErrorCallback = nullptr
    ) : m_onSharingStatusCallback(onSharingStatusCallback),
        m_onLockShareStatusCallback(onLockShareStatusCallback),
        m_onShareContentNotificationCallback(onShareContentNotificationCallback),
        m_onMultiShareSwitchToSingleShareNeedConfirmCallback(onMultiShareSwitchToSingleShareNeedConfirmCallback),
        m_onShareSettingTypeChangedNotificationCallback(onShareSettingTypeChangedNotificationCallback),
        m_onSharedVideoEndedCallback(onSharedVideoEndedCallback),
        m_onVideoFileSharePlayErrorCallback(onVideoFileSharePlayErrorCallback) {}

    void onSharingStatus(SharingStatus status, unsigned int userId) override {
        if (m_onSharingStatusCallback)
            m_onSharingStatusCallback(status, userId);
    }

    void onLockShareStatus(bool bLocked) override {
        if (m_onLockShareStatusCallback)
            m_onLockShareStatusCallback(bLocked);
    }

    void onShareContentNotification(ShareInfo& shareInfo) override {
        if (m_onShareContentNotificationCallback)
            m_onShareContentNotificationCallback(shareInfo);
    }

    void onMultiShareSwitchToSingleShareNeedConfirm(IShareSwitchMultiToSingleConfirmHandler* handler) override {
        if (m_onMultiShareSwitchToSingleShareNeedConfirmCallback)
            m_onMultiShareSwitchToSingleShareNeedConfirmCallback(handler);
    }

    void onShareSettingTypeChangedNotification(ShareSettingType type) override {
        if (m_onShareSettingTypeChangedNotificationCallback)
            m_onShareSettingTypeChangedNotificationCallback(type);
    }

    void onSharedVideoEnded() override {
        if (m_onSharedVideoEndedCallback)
            m_onSharedVideoEndedCallback();
    }

    void onVideoFileSharePlayError(ZoomSDKVideoFileSharePlayError error) override {
        if (m_onVideoFileSharePlayErrorCallback)
            m_onVideoFileSharePlayErrorCallback(error);
    }
};

void init_meeting_share_ctrl_event_callbacks(nb::module_ &m) {
    nb::class_<MeetingShareCtrlEventCallbacks, IMeetingShareCtrlEvent>(m, "MeetingShareCtrlEventCallbacks")
        .def(nb::init<
            const std::function<void(SharingStatus, unsigned int)>&,
            const std::function<void(bool)>&,
            const std::function<void(ShareInfo&)>&,
            const std::function<void(IShareSwitchMultiToSingleConfirmHandler*)>&,
            const std::function<void(ShareSettingType)>&,
            const std::function<void()>&,
            const std::function<void(ZoomSDKVideoFileSharePlayError)>&
        >(),
            nb::arg("onSharingStatusCallback") = nullptr,
            nb::arg("onLockShareStatusCallback") = nullptr,
            nb::arg("onShareContentNotificationCallback") = nullptr,
            nb::arg("onMultiShareSwitchToSingleShareNeedConfirmCallback") = nullptr,
            nb::arg("onShareSettingTypeChangedNotificationCallback") = nullptr,
            nb::arg("onSharedVideoEndedCallback") = nullptr,
            nb::arg("onVideoFileSharePlayErrorCallback") = nullptr
        );
}