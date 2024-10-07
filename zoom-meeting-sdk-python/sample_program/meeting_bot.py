import zoom_meeting_sdk_python as zoom
import jwt
from datetime import datetime, timedelta
from raw_audio_delegate import RawAudioDelegate

def dummy_func():
    print("yolod")

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

class MeetingBot:
    def __init__(self):
        
        self.meeting_service = None
        self.setting_service = None
        self.auth_service = None

        self.auth_event = None
        self.recording_event = None
        self.meeting_service_event = None

        self.audio_source = None
        self.audio_helper = None

        self.audio_settings = None

        self.use_raw_recording = True

        self.reminder_controller = None

        self.recording_ctrl = None

    def cleanup(self):

        #self.meeting_service_event.setOnMeetingJoin(None)

        if self.meeting_service:
            zoom.DestroyMeetingService(self.meeting_service)
        if self.setting_service:
            zoom.DestroySettingService(self.setting_service)
        if self.auth_service:
            zoom.DestroyAuthService(self.auth_service)
        zoom.CleanUPSDK()

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
        
        self.create_services()

    def on_join(self):
        print("Joined successfully")

        self.reminder_controller = self.meeting_service.GetMeetingReminderController()
        self.reminder_controller.SetEvent(zoom.MeetingReminderEvent())

        if self.use_raw_recording:
            self.recording_ctrl = self.meeting_service.GetMeetingRecordingController()

            def on_recording_privilege_changed(can_rec):
                print("can_rec = ", can_rec)
                if can_rec:
                    self.start_raw_recording()
                else:
                    self.stop_raw_recording()

            self.recording_event = zoom.MeetingRecordingCtrlEvent(on_recording_privilege_changed)
            self.recording_ctrl.SetEvent(self.recording_event)

            self.start_raw_recording()

    def on_one_way_audio_raw_data_received_callback(self, data, node_id):
        print("GDFFG", node_id)
        print("q", data.GetBufferLen())        
        
    
    def start_raw_recording(self):
        print("start_raw_recording")

        self.recording_ctrl = self.meeting_service.GetMeetingRecordingController()

        can_start_recording_result = self.recording_ctrl.CanStartRawRecording()
        if can_start_recording_result != zoom.SDKERR_SUCCESS:
            self.recording_ctrl.RequestLocalRecordingPrivilege()
            print("Requesting recording privilege.")
            return

        start_raw_recording_result = self.recording_ctrl.StartRawRecording()
        print("start_raw_recording_result")
        print(start_raw_recording_result)
        if start_raw_recording_result != zoom.SDKERR_SUCCESS:
            print("Start raw recording failed.")
            return

        self.audio_helper = zoom.GetAudioRawdataHelper()
        if self.audio_helper is None:
            print("audio_helper is None")
            return "BAD"
        if self.audio_source is None:
            mixedAudio = False
            transcribe = False
            self.audio_source = RawAudioDelegate()
            #self.audio_source = zoom.ZoomSDKAudioRawDataDelegatePassThrough()
            #self.audio_source.setOnOneWayAudioRawDataReceived(self.on_one_way_audio_raw_data_received_callback)
            #self.audio_source.setDir("out")
            #self.audio_source.setFilename("test.pcm")
            print("set some shit")

        print("self.audio_source", self.audio_source)
        print("self.audio_helper", self.audio_helper)
        audio_helper_subscribe_result = self.audio_helper.subscribe(self.audio_source, False)
        print("audio_helper_subscribe_result")
        print(audio_helper_subscribe_result)
        print("z")

    def stop_raw_recording(self):
        print("stop_raw_recording")
        rec_ctrl = self.meeting_service.StopRawRecording()
        if rec_ctrl.StopRawRecording() != zoom.SDKERR_SUCCESS:
            raise Exception("Error with stop raw recording")

    def leave(self):
        if self.meeting_service is None:
            return
        
        status = self.meeting_service.GetMeetingStatus()
        if status == zoom.MEETING_STATUS_IDLE:
            return

        self.meeting_service.Leave(zoom.LEAVE_MEETING)

    def join_meeting(self):
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
        print(self.meeting_service)
        join_result = self.meeting_service.Join(join_param)
        print("join_result")
        print(join_result)

        self.audio_settings = self.setting_service.GetAudioSettings()
        self.audio_settings.EnableAutoJoinAudio(True)
        print("MADE")

    def on_auth(self):
        print("Auth completed successfully")
        self.join_meeting()
        print("GRUN")


    def create_services(self):
        self.meeting_service = zoom.CreateMeetingService()
        
        self.setting_service = zoom.CreateSettingService()

        self.meeting_service_event = zoom.MeetingServiceEvent()
        
        self.meeting_service_event.setOnMeetingJoin(self.on_join)
        
        meeting_service_set_revent_result = self.meeting_service.SetEvent(self.meeting_service_event)
        if meeting_service_set_revent_result != zoom.SDKERR_SUCCESS:
            raise Exception("Meeting Service set event failed")
        
        self.auth_event = zoom.AuthServiceEvent(self.on_auth)

        self.auth_service = zoom.CreateAuthService()
        print("auth_service")
        print(self.auth_service)
        set_event_result = self.auth_service.SetEvent(self.auth_event)
        print("set_event_result")
        print(set_event_result)
    
        # Use the auth service
        auth_context = zoom.AuthContext()
        client_id="C_yBC775To66EK6MhNin9A"
        client_secret="jnLJW1BKXq4AAD7dgtjHPsUwAGYczPQZ"
        auth_context.jwt_token = generate_jwt(client_id, client_secret)
        print("auth_contexts")
        print(auth_context.jwt_token)
        result = self.auth_service.SDKAuth(auth_context)
    
        if result == zoom.SDKError.SDKERR_SUCCESS:
            print("Authentication successful")
            print("098")
        else:
            print(f"Authentication failed with errqor: {result}")
            print(f"Authentication failed with error: {self.auth_service.GetAuthResult()}")
