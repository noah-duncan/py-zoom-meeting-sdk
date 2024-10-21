import zoom_meeting_sdk_python as zoom
from datetime import datetime, timedelta
import jwt
from dotenv import load_dotenv
import os
import time
import random
import urllib.parse

import gi
gi.require_version('GLib', '2.0')
from gi.repository import GLib

import signal
import sys

testBot = None
main_loop = None

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

def get_random_meeting():
    urls = os.environ.get('MEETING_URLS')
    url_list = urls.split('\n')

    # Choose a random URL from the list
    chosen_url = random.choice(url_list)

    # Parse the URL
    parsed_url = urllib.parse.urlparse(chosen_url)

    # Extract the path and query components
    path = parsed_url.path
    query = urllib.parse.parse_qs(parsed_url.query)

    # Extract meeting ID from the path
    meeting_id = path.split('/')[-1]

    password = query.get('pwd', [None])[0]

    return int(meeting_id), password

class TestBot():
    def __init__(self):
        pass

    def init(self):
        init_param = zoom.InitParam()

        init_param.strWebDomain = "https://zoom.us"
        init_param.strSupportUrl = "https://zoom.us"
        init_param.enableGenerateDump = True
        init_param.emLanguageID = zoom.SDK_LANGUAGE_ID.LANGUAGE_English
        init_param.enableLogByDefault = True

        init_sdk_result = zoom.InitSDK(init_param)
        if init_sdk_result != zoom.SDKERR_SUCCESS:
            raise Exception('InitSDK failed')

        self.meeting_service_event = zoom.MeetingServiceEvent()
        self.meeting_service_event.setOnMeetingJoin(self.on_join)
        self.meeting_service = zoom.CreateMeetingService()  
        self.meeting_service.SetEvent(self.meeting_service_event)

        self.auth_event = zoom.AuthServiceEvent(self.on_auth)
        self.auth_service = zoom.CreateAuthService()
        self.auth_service.SetEvent(self.auth_event)
    
        # Use the auth service
        auth_context = zoom.AuthContext()
        auth_context.jwt_token = generate_jwt(os.environ.get('ZOOM_APP_CLIENT_ID')+"s", os.environ.get('ZOOM_APP_CLIENT_SECRET'))
        result = self.auth_service.SDKAuth(auth_context)
        if result != zoom.SDKError.SDKERR_SUCCESS:
            raise Exception('SDKAuth failed!')

    def on_join(self):
        print("on_join called")
        with open("/tmp/test_passed", 'w') as f:
            f.write('test_passed')
        GLib.timeout_add_seconds(4, self.exit_process)

    def exit_process(self):
        if main_loop:
            main_loop.quit()
        return False  # To stop the timeout from repeating

    def on_auth(self):
        print("on_auth called")

        meeting_number, password = get_random_meeting()

        display_name = "TestBot"

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

        self.meeting_service.Join(join_param)

    def leave(self):
        if self.meeting_service is None:
            return
        
        if self.meeting_service.GetMeetingStatus() == zoom.MEETING_STATUS_IDLE:
            return
        
        self.meeting_service.Leave(zoom.LEAVE_MEETING)

    def cleanup(self):
        if self.meeting_service:
            print("DestroyMeetingService")
            zoom.DestroyMeetingService(self.meeting_service)
            print("EndDestroyMeetingService")

        if self.auth_service:
            print("DestroyAuthService")
            zoom.DestroyAuthService(self.auth_service)
            print("EndDestroyAuthService")

        print("CleanUPSDK")
        zoom.CleanUPSDK()
        print("EndCleanUPSDK")

def on_signal(signum, frame):
    print(f"Received signal {signum}")
    sys.exit(0)

def on_exit():
    print("Exiting...")
    testBot.leave()
    print("cleaning...")
    testBot.cleanup()

def on_timeout():
    return True  # Returning True keeps the timeout active

def run():
    global testBot, main_loop
    testBot = TestBot()
    testBot.init()   
    
    # Create a GLib main loop
    main_loop = GLib.MainLoop()

    # Add a timeout function that will be called every 100ms
    GLib.timeout_add(100, on_timeout)

    # Run the main loop
    try:
        main_loop.run()
    except KeyboardInterrupt:
        print("Interrupted by user, shutting down...")
    finally:
        main_loop.quit()

def main():
    load_dotenv()

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