import zoom_meeting_sdk as zoom
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
from multiprocessing import Process

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
        self.main_loop = None
        
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

        self.meeting_service_event = zoom.MeetingServiceEventCallbacks(onMeetingStatusChangedCallback=self.meeting_status_changed)
        self.meeting_service = zoom.CreateMeetingService()  
        self.meeting_service.SetEvent(self.meeting_service_event)

        self.auth_event = zoom.AuthServiceEventCallbacks(onAuthenticationReturnCallback=self.auth_return)
        self.auth_service = zoom.CreateAuthService()
        self.auth_service.SetEvent(self.auth_event)
    
        # Use the auth service
        auth_context = zoom.AuthContext()
        auth_context.jwt_token = generate_jwt(os.environ.get('ZOOM_APP_CLIENT_ID'), os.environ.get('ZOOM_APP_CLIENT_SECRET'))
        result = self.auth_service.SDKAuth(auth_context)
        if result != zoom.SDKError.SDKERR_SUCCESS:
            raise Exception('SDKAuth failed!')
        
    def meeting_status_changed(self, status, iResult):
        if status == zoom.MEETING_STATUS_INMEETING:
            print("joined meeting")

            my_user_name = self.meeting_service.GetMeetingParticipantsController().GetMySelfUser().GetUserName()
            if my_user_name != "TestJoinBot":
                raise Exception("Failed to get username")

            with open("/tmp/test_passed", 'w') as f:
                f.write('test_passed')
            print("self.meeting_service after joined", self.meeting_service)
            GLib.timeout_add_seconds(4, self.exit_process)

    def exit_process(self):
        if self.main_loop:
            self.main_loop.quit()
        return False  # To stop the timeout from repeating

    def auth_return(self, result):
        if result == zoom.AUTHRET_SUCCESS:
            print("Auth completed successfully.")

            meeting_number, password = get_random_meeting()

            display_name = "TestJoinBot"

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
            return

        raise Exception("Failed to authorize. result = ", result)

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

    def on_timeout(self):
        return True  # Keeping the timeout active
        
    def on_exit(self):
        print("Exiting...")
        self.leave()
        print("cleaning...")
        self.cleanup()
        
    def run(self):
        self.init()
        
        # Create a GLib main loop
        self.main_loop = GLib.MainLoop()
        print("self.main_loop", self.main_loop)
        # Add a timeout function that will be called every 100ms
        GLib.timeout_add(100, self.on_timeout)
        
        # Run the main loop
        try:
            self.main_loop.run()
        except KeyboardInterrupt:
            print("Interrupted by user, shutting down...")
        finally:
            self.main_loop.quit()

def on_signal(signum, frame):
    print(f"Received signal {signum}")
    sys.exit(0)

def main():
    load_dotenv()

    # Set up signal handlers
    signal.signal(signal.SIGINT, on_signal)
    signal.signal(signal.SIGTERM, on_signal)

    # Create and run multiple bots
    processes = []
    for i in range(3):
        # Create bot inside the process instead of before
        p = Process(target=run_bot)
        p.daemon = True
        p.start()
        processes.append(p)

    try:
        for p in processes:
            p.join()
    except KeyboardInterrupt:
        for p in processes:
            p.terminate()
            p.join()

def run_bot():
    bot = TestBot()
    import atexit
    atexit.register(bot.on_exit)
    bot.run()
    bot.on_exit()

if __name__ == "__main__":
    main()