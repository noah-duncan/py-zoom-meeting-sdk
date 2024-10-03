import zoom_sdk_python as zoom
import time

import jwt
from datetime import datetime, timedelta

from typing import Callable, Optional
import asyncio


import signal
import sys

import gi
gi.require_version('GLib', '2.0')
from gi.repository import GLib


def on_signal(signum, frame):
    print(f"Received signal {signum}")
    sys.exit(0)

def on_exit():
    print("Exiting...")

def on_timeout():
    return True  # Returning True keeps the timeout active


def generate_jwt(client_id, client_secret):
    iat = datetime.utcnow()
    exp = iat + timedelta(hours=24)
    
    payload = {
        "iat": iat,
        "exp": exp,
        "appKey": client_id,
        "tokenExp": int(exp.timestamp())
    }
    
    token = jwt.encode(payload, client_secret, algorithm="HS256")
    return token


def run():
    def join_meeting():
        mid = "4849920355"
        password = "22No8yGYAajAoTaz5H00RIg5HkgEWk.1"
        display_name = "test bkkki"

        meeting_number = int(mid)

        join_param = zoom.JoinParam()
        join_param.userType = zoom.SDKUserType.SDK_UT_WITHOUT_LOGIN

        param = join_param.param
        param.meetingNumber = meeting_number
        param.userName = display_name
        param.psw = password
        param.vanityID = ""
        param.customer_key = ""
        param.webinarToken = ""
        param.isVideoOff = False
        param.isAudioOff = False

        print(param.meetingNumber)
        print(param.psw)
        print(param.userName)

        print("meeting_service before join")
        print(meeting_service)
        join_result = meeting_service.Join(join_param)
        print("join_result")
        print(join_result)


    init_param = zoom.InitParam()

    init_param.strWebDomain = "https://zoom.us"
    init_param.strSupportUrl = "https://zoom.us"
    init_param.enableGenerateDump = True
    init_param.emLanguageID = zoom.SDK_LANGUAGE_ID.LANGUAGE_English
    init_param.enableLogByDefault = True

    init_sdk_result = zoom.InitSDK(init_param)

    print("init_sdk_result")
    print(init_sdk_result)

    meeting_service = zoom.CreateMeetingService()
    print("meeting_service")
    print(meeting_service)

    setting_service = zoom.CreateSettingService()
    print("setting_service")
    print(setting_service)
    use_raw_recording = True

    def start_raw_recording():
        # Implement raw recording start logic here
        print("start_raw_recording")


        recording_ctrl = meeting_service.GetMeetingRecordingController()
        recording_ctrl.CanStartRawRecording()
        recording_ctrl.RequestLocalRecordingPrivilege()
        recording_ctrl.StartRawRecording()
        print("STARTED")

    def stop_raw_recording():
        # Implement raw recording stop logic here
        print("stop_raw_recording")

    def on_join():
        print("Joined successfully")

        reminder_controller = meeting_service.GetMeetingReminderController()
        reminder_controller.SetEvent(zoom.MeetingReminderEvent())

        if use_raw_recording:
            recording_ctrl = meeting_service.GetMeetingRecordingController()

            def on_recording_privilege_changed(can_rec):
                if can_rec:
                    start_raw_recording()
                else:
                    stop_raw_recording()

            recording_event = zoom.MeetingRecordingCtrlEvent(on_recording_privilege_changed)
            recording_ctrl.SetEvent(recording_event)

            start_raw_recording()

    meeting_service_event = zoom.MeetingServiceEvent()
    meeting_service_event.setOnMeetingJoin(on_join)
    meeting_service.SetEvent(meeting_service_event)

    # Create an auth service
    def on_auth():
        print("Auth completed successfully")
        join_meeting()

    def on_authentication_return(result: zoom.AuthResult):
        print(f"Authenticaztion returned: {result}")
        #join_meeting()

    # Create the event handler
    auth_event = zoom.AuthServiceEvent(on_auth)

    # Set callbacks
    #auth_event.setOnAuth(on_auth)
    #auth_event.setOnAuthenticationReturn(on_authentication_return)

    # Now you can pass this to the SDK's SetEvent method

    auth_service = zoom.CreateAuthService()
    print("auth_service")
    print(auth_service)
    set_event_result = auth_service.SetEvent(auth_event)
    print("set_event_result")
    print(set_event_result)

    # Use the auth service
    auth_context = zoom.AuthContext()
    client_id="C_yBC775To66EK6MhNin9A"
    client_secret="jnLJW1BKXq4AAD7dgtjHPsUwAGYczPQZ"
    auth_context.jwt_token = generate_jwt(client_id, client_secret)
    #auth_context.jwt_token = "Dfdf"
    #auth_context.jwt_token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJhcHBLZXkiOiJDX3lCQzc3NVRvNjZFSzZNaE5pbjlBIiwiZXhwIjoxNzI3NTQzODE2LCJpYXQiOjE3Mjc0NTc0MTYsInRva2VuRXhwIjoxNzI3NTQzODE2fQ.9beBgeqMmFT9prDBdpHiltvMJknhh94r_5xIxlvTPuU"
    print("auth_contexts")
    print(auth_context.jwt_token)
    result = auth_service.SDKAuth(auth_context)

    if result == zoom.SDKError.SDKERR_SUCCESS:
        print("Authentication successful")
        print("098")
    else:
        print(f"Authentication failed with errqor: {result}")
        print(f"Authentication failed with error: {auth_service.GetAuthResult()}")

    print("qqqq")

    
    # Create a GLib main loop
    main_loop = GLib.MainLoop()

    # Add a timeout function that will be called every 100ms
    GLib.timeout_add(100, on_timeout)

    # Run the main loop
    try:
        print("STARTLOOPP")
        main_loop.run()
    except KeyboardInterrupt:
        print("Interrupted by user, shutting down...")
    finally:
        # Cleanup code here
        # zoom_api.cleanup()
        main_loop.quit()

def main():
    # Set up signal handlers
    signal.signal(signal.SIGINT, on_signal)
    signal.signal(signal.SIGTERM, on_signal)

    # Set up exit handler
    import atexit
    atexit.register(on_exit)

    # Run the Meeting Bot
    run()


if __name__ == "__main__":
    main()




