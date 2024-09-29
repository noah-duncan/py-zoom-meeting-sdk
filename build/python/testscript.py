import zoom_sdk_python as zoom
import time

import jwt
from datetime import datetime, timedelta

from typing import Callable, Optional
import asyncio


import signal
import sys

def on_signal(signum, frame):
    print(f"Received signal {signum}")
    sys.exit(0)

def on_exit():
    print("Exiting...")




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


async def run():
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
    else:
        print(f"Authentication failed with errqor: {result}")
        print(f"Authentication failed with error: {auth_service.GetAuthResult()}")



    zoom.runloopy();

    # # join start
    # mid = "4849920355"
    # password = "22No8yGYAajAoTaz5H00RIg5HkgEWk.1"
    # display_name = "test bi"

    # meeting_number = int(mid)

    # join_param = zoom.JoinParam()
    # join_param.userType = zoom.SDKUserType.SDK_UT_WITHOUT_LOGIN

    # param = join_param.param
    # param.meetingNumber = meeting_number
    # param.userName = display_name
    # param.psw = password
    # param.vanityID = ""
    # param.customer_key = ""
    # param.webinarToken = ""
    # param.isVideoOff = False
    # param.isAudioOff = False

    # print(param.meetingNumber)
    # print(param.psw)
    # print(param.userName)

    # join_result = meeting_service.Join(join_param)
    # print("join_result")
    # print(join_result)

    # zoom.runloopy();

    # zoom.DestroyAuthService(auth_service)
    # zoom.DestroySettingService(setting_service)
    # zoom.DestroyMeetingService(meeting_service)
    # time.sleep(5)

    # #meeting_service
    # #status = meeting_service.GetMeetingStatus()

async def main():
    # Set up signal handlers
    signal.signal(signal.SIGINT, on_signal)
    signal.signal(signal.SIGTERM, on_signal)

    # Set up exit handler
    import atexit
    atexit.register(on_exit)

    # Run the Meeting Bot
    err = await run()

    if zoom.Zoom.hasError(err, "waiting"):
        return err

    zoom.runloopy()

if __name__ == "__main__":
    asyncio.run(main())




