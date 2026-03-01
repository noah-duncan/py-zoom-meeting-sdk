#!/usr/bin/env python3
"""
Simple Zoom Bot that hosts a meeting and creates 5 breakout rooms.
Usage: python breakout_room_host_bot.py

Debug Mode:
    DEBUG_MODE=true python breakout_room_host_bot.py
    
    Enables verbose terminal output of all SDK calls and callbacks
"""

import zoom_meeting_sdk as zoom
import time
import jwt
from datetime import datetime, timedelta
import os
from dotenv import load_dotenv
import signal
import sys
import gi
gi.require_version('GLib', '2.0')
from gi.repository import GLib

class BreakoutRoomHostBot:
    def __init__(self):
        # Check if debug mode is enabled
        self.debug_mode = os.environ.get('DEBUG_MODE', 'false').lower() == 'true'
        
        if self.debug_mode:
            print("\n" + "="*80)
            print("🔍 DEBUG MODE ENABLED - Verbose output enabled")
            print("="*80 + "\n")
        
        self.meeting_service = None
        self.auth_service = None
        self.setting_service = None
        self.auth_event = None
        self.meeting_service_event = None
        self.bo_controller = None
        self.bo_creator = None
        self.bo_admin = None  # Add admin object for user assignment
        self.bo_admin_event = None
        self.bo_event = None
        self.bo_data_helper = None
        self.main_loop = None
        self.is_authenticated = False
        self.is_meeting_started = False
        self.breakout_rooms_created = False
        
        # Participant management
        self.participants_ctrl = None
        self.participants_event = None
        self.my_participant_id = None
        self.participants_list = []
        self.room_assignments = {}
        self.created_rooms = {}  # Store actual BO IDs and names
        
        # Creator rights and event handling
        self.has_creator_rights = False
        self.creator_event_handler = None
        self.pending_room_creation = False

    def generate_jwt(self, client_id, client_secret):
        """Generate JWT token for Zoom authentication"""
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

    def init(self):
        """Initialize the Zoom SDK with safe settings"""
        # Check required environment variables
        required_env_vars = ['ZOOM_APP_CLIENT_ID', 'ZOOM_APP_CLIENT_SECRET', 'MEETING_ID', 'MEETING_PWD']
        for var in required_env_vars:
            if os.environ.get(var) is None:
                raise Exception(f'No {var} found in environment. Please define this in a .env file')

        # Initialize SDK with minimal safe settings
        init_param = zoom.InitParam()
        init_param.strWebDomain = "https://zoom.us"
        init_param.strSupportUrl = "https://zoom.us"
        init_param.enableGenerateDump = False  # Disable dump generation
        init_param.emLanguageID = zoom.SDK_LANGUAGE_ID.LANGUAGE_English
        init_param.enableLogByDefault = False  # Disable logging to reduce crashes

        print("🔧 Initializing Zoom SDK with safe settings...")
        init_result = zoom.InitSDK(init_param)
        if init_result != zoom.SDKERR_SUCCESS:
            raise Exception(f'InitSDK failed with result: {init_result}')

        print("✅ SDK initialized successfully")
        self.create_services()

    def create_services(self):
        """Create and configure SDK services with error handling"""
        try:
            # Create authentication service
            self.auth_service = zoom.CreateAuthService()
            if not self.auth_service:
                raise Exception("Failed to create auth service")

            # Set up authentication event handler with safe callbacks
            self.auth_event = zoom.AuthServiceEventCallbacks(
                onAuthenticationReturnCallback=self.on_auth_return
            )
            self.auth_service.SetEvent(self.auth_event)
            print("✅ Auth service created")

            # Create meeting service
            self.meeting_service = zoom.CreateMeetingService()
            if not self.meeting_service:
                raise Exception("Failed to create meeting service")

            # Set up meeting service event handler
            self.meeting_service_event = zoom.MeetingServiceEventCallbacks(
                onMeetingStatusChangedCallback=self.on_meeting_status_changed
            )
            self.meeting_service.SetEvent(self.meeting_service_event)
            print("✅ Meeting service created")

            # Create settings service with error handling
            try:
                self.setting_service = zoom.CreateSettingService()
                if self.setting_service:
                    print("✅ Setting service created")
                else:
                    print("⚠️  Setting service creation returned None")
            except Exception as e:
                print(f"⚠️  Failed to create setting service: {e}")
                self.setting_service = None
            
            print("✅ Core services setup completed")
            
        except Exception as e:
            print(f"❌ Error in service creation: {e}")
            raise

    def authenticate(self):
        """Authenticate with Zoom SDK"""
        client_id = os.environ.get('ZOOM_APP_CLIENT_ID')
        client_secret = os.environ.get('ZOOM_APP_CLIENT_SECRET')
        
        jwt_token = self.generate_jwt(client_id, client_secret)
        
        auth_context = zoom.AuthContext()
        auth_context.jwt_token = jwt_token

        auth_result = self.auth_service.SDKAuth(auth_context)
        print(f"🔐 Authentication initiated with result: {auth_result}")

    def on_auth_return(self, result):
        """Handle authentication response"""
        print(f"🔐 Authentication completed with result: {result}")
        if result == zoom.AuthResult.AUTHRET_SUCCESS:
            self.is_authenticated = True
            print("✅ Authentication successful! Joining meeting as host...")
            # Join meeting after successful authentication
            GLib.timeout_add(1000, self.start_meeting)
        else:
            print(f"❌ Authentication failed with result: {result}")
            self.shutdown()

    def on_login_return(self, ret, login_detail):
        """Handle login response"""
        print(f"Login return: {ret}")

    def start_meeting(self):
        """Join a meeting as host (using Join instead of Start for compatibility)"""
        try:
            meeting_id = os.environ.get('MEETING_ID')
            password = os.environ.get('MEETING_PWD')
            display_name = "Breakout Room Bot"

            meeting_number = int(meeting_id)

            # Use JoinParam like the working sample
            join_param = zoom.JoinParam()
            join_param.userType = zoom.SDKUserType.SDK_UT_WITHOUT_LOGIN

            param = join_param.param
            param.meetingNumber = meeting_number
            param.userName = display_name
            param.psw = password
            # Disable video and audio to prevent graphics/audio crashes
            param.isVideoOff = True
            param.isAudioOff = True
            param.isAudioRawDataStereo = False
            param.isMyVoiceInMix = False

            result = self.meeting_service.Join(join_param)
            print(f"🎥 Meeting join initiated with result: {result}")
            
            # The actual success/failure will be handled by the meeting status callback

            # Disable audio settings to prevent crashes
            try:
                self.audio_settings = self.setting_service.GetAudioSettings()
                if self.audio_settings:
                    self.audio_settings.EnableAutoJoinAudio(False)  # Disable audio
                    print("✅ Audio settings configured (disabled)")
            except Exception as e:
                print(f"⚠️  Could not configure audio settings: {e}")
                
            return False  # Don't repeat this timeout
            
        except Exception as e:
            print(f"❌ Error joining meeting: {e}")
            self.shutdown()
            return False

    def on_meeting_status_changed(self, status, iResult):
        """Handle meeting status changes"""
        print(f"📊 Meeting status changed: {status}, result: {iResult}")
        
        if status == zoom.MEETING_STATUS_INMEETING:
            print("✅ Successfully joined meeting as host!")
            self.is_meeting_started = True
            
            # Get and print meeting join link
            try:
                meeting_info = self.meeting_service.GetMeetingInfo()
                join_url = meeting_info.GetJoinMeetingUrl()
                meeting_topic = meeting_info.GetMeetingTopic()
                meeting_number = meeting_info.GetMeetingNumber()
                
                print("\n" + "="*60)
                print("🔗 MEETING JOIN INFORMATION")
                print("="*60)
                print(f"📝 Topic: {meeting_topic}")
                print(f"🔢 Meeting ID: {meeting_number}")
                print(f"🌐 Join URL: {join_url}")
                print("="*60 + "\n")
                
            except Exception as e:
                print(f"⚠️  Could not get meeting info: {e}")
            
            # Wait a bit before creating breakout rooms
            GLib.timeout_add(3000, self.setup_participants_and_breakout_rooms)
        elif status == zoom.MEETING_STATUS_ENDED:
            print("📋 Meeting ended")
            self.shutdown()
        elif status == zoom.MEETING_STATUS_FAILED:
            print(f"❌ Meeting failed: {iResult}")
            self.shutdown()

    def setup_participants_and_breakout_rooms(self):
        """Set up participant management and then create breakout rooms"""
        try:
            # Set up participants controller
            self.participants_ctrl = self.meeting_service.GetMeetingParticipantsController()
            if not self.participants_ctrl:
                print("❌ Failed to get participants controller")
                return False

            # Set up participant events with proper error handling
            try:
                self.participants_event = zoom.MeetingParticipantsCtrlEventCallbacks(
                    onUserJoinCallback=self.on_user_join_callback,
                    onHostChangeNotificationCallback=self.on_host_change_notification
                )
                self.participants_ctrl.SetEvent(self.participants_event)
                print("✅ Participants event handler set up")
            except Exception as e:
                print(f"⚠️  Error setting up participant events: {e}")
            
            # Get current participants
            self.my_participant_id = self.participants_ctrl.GetMySelfUser().GetUserID()
            participants_ids = self.participants_ctrl.GetParticipantsList()
            
            print(f"\n👥 PARTICIPANTS IN MEETING")
            print("="*50)
            print(f"🤖 Bot ID: {self.my_participant_id}")
            
            # Get participant details
            self.participants_list = []
            for participant_id in participants_ids:
                if participant_id != self.my_participant_id:
                    try:
                        user_info = self.participants_ctrl.GetUserByUserID(participant_id)
                        user_name = user_info.GetUserName()
                        email = user_info.GetEmail() if hasattr(user_info, 'GetEmail') else "N/A"
                        
                        participant_data = {
                            'id': participant_id,
                            'name': user_name,
                            'email': email
                        }
                        self.participants_list.append(participant_data)
                        
                        print(f"👤 {user_name} (ID: {participant_id}) | Email: {email}")
                        
                    except Exception as e:
                        print(f"⚠️  Error getting info for participant {participant_id}: {e}")
            
            print("="*50)
            print(f"📊 Total participants (excluding bot): {len(self.participants_list)}\n")
            
            # Check if bot is already host
            try:
                my_user = self.participants_ctrl.GetMySelfUser()
                if my_user:
                    print(f"🤖 Bot status check...")
                    # Check various host indicators
                    print(f"   User ID: {my_user.GetUserID()}")
                    print(f"   User Name: {my_user.GetUserName()}")
                    print("⏳ Waiting to be made meeting host before creating breakout rooms...")
                    print("💡 Please make this bot the meeting host to enable breakout room creation")
                    print("🔧 If you've made the bot host and rooms aren't created, trying manual trigger...")
                    
                    # Try manual trigger after delay in case callback doesn't work
                    GLib.timeout_add(10000, self.check_and_create_rooms_if_host)
                    
            except Exception as e:
                print(f"⚠️  Could not check host status: {e}")
                print("⏳ Waiting to be made meeting host...")
            
            # Don't create breakout rooms immediately - wait for host status
            return False

        except Exception as e:
            print(f"❌ Error setting up participants: {e}")
            return False

    def on_host_change_notification(self, user_id=None, *args):
        """Handle when someone becomes host"""
        try:
            print(f"🔄 Host change notification: User {user_id}, args: {args}")
            
            # Check if bot became host by comparing user IDs
            if user_id == self.my_participant_id:
                print("🎉 Bot is now the meeting host!")
                print("🏗️  Starting breakout room creation process...")
                
                # Start breakout room creation process
                GLib.timeout_add(2000, self.setup_breakout_rooms)
            else:
                print(f"👑 User {user_id} host status changed (not the bot)")
                
        except Exception as e:
            print(f"❌ Error in host change callback: {e}")

    def check_and_create_rooms_if_host(self):
        """Manual check if bot is host and create rooms"""
        try:
            print("🔍 Manual host status check...")
            
            # Try to access BO controller to see if we have host privileges
            if self.bo_controller and self.bo_controller.IsBOEnabled():
                print("✅ BO controller accessible - likely host!")
                
                # Try to get creator helper
                try:
                    creator = self.bo_controller.GetBOCreatorHelper()
                    if creator:
                        print("🎉 Bot appears to be host! (Creator helper accessible)")
                        print("⏳ Room creation will be handled by main flow...")
                        print("   (Skipping duplicate creation attempt)")
                        self.bo_creator = creator
                        
                        # NOTE: Don't create rooms here - will be created by main flow
                        # Duplicate creation attempt removed to prevent 10+ rooms
                        
                        return False  # Don't repeat
                    else:
                        print("⚠️  No creator helper - may not be host yet")
                        return True  # Try again
                except Exception as e:
                    print(f"⚠️  Could not get creator helper: {e}")
                    return True
                    
            else:
                print("⚠️  BO not accessible - not host yet")
                return True  # Try again in 10 seconds
                
        except Exception as e:
            print(f"❌ Error in manual host check: {e}")
            return True  # Try again

    def on_user_join_callback(self, joined_user_ids, user_name=None):
        """Handle when new users join the meeting"""
        try:
            # Handle potential None or type issues
            user_name_str = str(user_name) if user_name is not None else "Unknown User"
            user_ids_str = str(joined_user_ids) if joined_user_ids is not None else "Unknown IDs"
            print(f"👋 New user joined: {user_name_str} (IDs: {user_ids_str})")
        except Exception as e:
            print(f"⚠️  Error in user join callback: {e}")
            return
        
        # Update participants list
        for user_id in joined_user_ids:
            if user_id != self.my_participant_id:
                try:
                    user_info = self.participants_ctrl.GetUserByUserID(user_id)
                    user_name = user_info.GetUserName()
                    email = user_info.GetEmail() if hasattr(user_info, 'GetEmail') else "N/A"
                    
                    participant_data = {
                        'id': user_id,
                        'name': user_name,
                        'email': email
                    }
                    
                    # Add to list if not already there
                    if not any(p['id'] == user_id for p in self.participants_list):
                        self.participants_list.append(participant_data)
                        print(f"✅ Added to participant list: {user_name} | Email: {email}")
                        
                        # Auto-assign to breakout room if rooms are already created
                        if self.breakout_rooms_created and self.bo_creator and self.created_rooms:
                            self.assign_participant_to_room(participant_data)
                            
                except Exception as e:
                    print(f"⚠️  Error processing new participant {user_id}: {e}")

    def assign_participant_to_room(self, participant):
        """Assign a participant to a breakout room"""
        if not self.created_rooms:
            print("❌ No rooms available for assignment")
            return
            
        try:
            # Get available room numbers
            available_rooms = list(self.created_rooms.keys())
            if not available_rooms:
                print("❌ No successfully created rooms available")
                return
                
            # Simple round-robin assignment to available rooms
            room_index = len(self.room_assignments) % len(available_rooms)
            room_number = available_rooms[room_index]
            room_info = self.created_rooms[room_number]
            
            # Convert IDs to strings for the API call
            user_id_str = str(participant['id'])
            bo_id_str = room_info['id']
            
            result = self.bo_creator.AssignUserToBO(user_id_str, bo_id_str)
            self.room_assignments[participant['id']] = room_number
            
            print(f"🎯 Assigned {participant['name']} to {room_info['name']} | Result: {result}")
            
        except Exception as e:
            print(f"❌ Error assigning {participant['name']} to room: {e}")

    def assign_all_participants_to_rooms(self):
        """Assign all current participants to breakout rooms using creator interface"""
        if not self.bo_creator or not self.participants_list or not self.created_rooms:
            if not self.created_rooms:
                print("❌ No rooms were created successfully, cannot assign participants")
            return
            
        print(f"\n🎯 ASSIGNING PARTICIPANTS TO BREAKOUT ROOMS")
        print("="*60)
        print(f"📊 Available rooms: {len(self.created_rooms)}")
        print(f"👥 Participants to assign: {len(self.participants_list)}")
        print("-"*60)
        
        available_rooms = list(self.created_rooms.keys())
        successful_assignments = 0
        
        for i, participant in enumerate(self.participants_list):
            if not available_rooms:
                print("❌ No rooms available for assignment")
                break
                
            # Round-robin assignment to available rooms
            room_number = available_rooms[i % len(available_rooms)]
            room_info = self.created_rooms[room_number]
            
            try:
                user_id_str = str(participant['id'])
                bo_id_str = room_info['id']
                
                print(f"🔄 Assigning {participant['name']} (ID: {user_id_str}) to {room_info['name']}")
                print(f"   Using BO_ID: {bo_id_str}")
                
                # Use new method names from bindings (both uppercase and lowercase)
                result = False
                if hasattr(self.bo_creator, 'AssignUserToBO'):
                    result = self.bo_creator.AssignUserToBO(user_id_str, bo_id_str)
                    print(f"   AssignUserToBO: {result}")
                elif hasattr(self.bo_creator, 'assignUserToBO'):
                    result = self.bo_creator.assignUserToBO(user_id_str, bo_id_str)
                    print(f"   assignUserToBO: {result}")
                
                if result:
                    self.room_assignments[participant['id']] = room_number
                    print(f"✅ {participant['name']} → {room_info['name']} | SUCCESS")
                    successful_assignments += 1
                else:
                    print(f"❌ {participant['name']} → {room_info['name']} | FAILED")
                    
                    # Debug: Check if user is in correct state
                    try:
                        if hasattr(self.participants_ctrl, 'GetUserByUserID'):
                            user_info = self.participants_ctrl.GetUserByUserID(participant['id'])
                            if hasattr(user_info, 'GetUserName'):
                                current_name = user_info.GetUserName()
                                print(f"   🔍 User still in meeting: {current_name}")
                    except Exception as debug_e:
                        print(f"   ⚠️  Could not verify user state: {debug_e}")
                
            except Exception as e:
                print(f"❌ Failed to assign {participant['name']}: {e}")
        
        print("="*60)
        print(f"✅ Assignment complete! {successful_assignments}/{len(self.participants_list)} participants assigned.")
        
        # ✅ Verify attendee configuration after assignment
        print("\n🔍 VERIFYING ATTENDEE BO ACCESS...")
        try:
            if self.bo_creator:
                current_option = zoom.BOOption()
                get_result = self.bo_creator.GetBOOption(current_option)
                
                if get_result or current_option:
                    # Check IsAttendeeContained property
                    is_attendee_contained = getattr(current_option, 'IsAttendeeContained', None)
                    if is_attendee_contained is not None:
                        if is_attendee_contained:
                            print("✅ IsAttendeeContained = True")
                            print("   → Attendees/webinar participants CAN join breakout rooms")
                        else:
                            print("⚠️  IsAttendeeContained = False")
                            print("   → Attendees/webinar participants CANNOT join breakout rooms")
                    else:
                        print("ℹ️  IsAttendeeContained: Not available (may not be applicable for this meeting type)")
                    
                    # Also check participant choice setting
                    can_choose = getattr(current_option, 'IsParticipantCanChooseBO', None)
                    if can_choose is not None:
                        print(f"✅ IsParticipantCanChooseBO = {can_choose}")
                        print("   → Participants can select their own breakout rooms")
                else:
                    print("⚠️  Could not retrieve BO options for verification")
        except Exception as e:
            print(f"⚠️  Could not verify attendee configuration: {e}")
        
        print("")
        
    def setup_breakout_rooms(self):
        """Set up breakout room controller and create rooms"""
        try:
            # Get the breakout room controller
            self.bo_controller = self.meeting_service.GetMeetingBOController()
            if not self.bo_controller:
                print("❌ Failed to get BO controller")
                return False

            print("✅ Got BO controller")

            # Check if breakout rooms are enabled
            if not self.bo_controller.IsBOEnabled():
                print("❌ Breakout rooms are not enabled for this meeting")
                return False

            print("✅ Breakout rooms are enabled")

            # Completely avoid all BO callbacks due to std::bad_cast crashes
            # Set up BO event handler to listen for creator rights notification
            try:
                print("🔧 Setting up BO event handler for creator rights...")
                
                # Create BO event handler with the correct class name and callback
                self.bo_event = zoom.MeetingBOEventCallbacks(
                    onHasCreatorRightsNotificationCallback=self.on_has_creator_rights_notification
                )
                self.bo_controller.SetEvent(self.bo_event)
                print("✅ BO event handler set - listening for creator rights...")
                
                # Give some time for callbacks to trigger
                print("⏳ Waiting for creator rights notification...")
                GLib.timeout_add(5000, self.check_creator_rights_fallback)
                
            except Exception as e:
                print(f"⚠️  Could not set BO event handler: {e}")
                print("🔄 Falling back to direct approach...")
                GLib.timeout_add(3000, self.create_breakout_rooms_without_callbacks)

            return False  # Don't repeat this timeout

        except Exception as e:
            print(f"❌ Error setting up breakout rooms: {e}")
            return False

    def create_breakout_rooms_without_callbacks(self):
        """Create breakout rooms using direct API calls without callbacks"""
        
        # Prevent duplicate creation
        if self.breakout_rooms_created:
            print("✅ Breakout rooms already created - skipping duplicate creation")
            return False
            
        print("🏗️  Creating breakout rooms without callback dependencies...")
        
        try:
            # Try to access BO admin directly
            print("🔄 Attempting to get BO admin helper directly...")
            bo_admin = None
            
            if self.bo_controller:
                try:
                    # Try to get admin helper directly from controller
                    bo_admin = self.bo_controller.GetBOAdminHelper()
                    if bo_admin:
                        print("✅ Got BO admin helper directly!")
                        self.bo_admin = bo_admin
                    else:
                        print("⚠️  GetBOAdminHelper() returned None")
                except Exception as e:
                    print(f"⚠️  Could not get BO admin helper: {e}")
            
            # Try to get creator helper for room creation
            bo_creator = None
            if self.bo_controller:
                try:
                    bo_creator = self.bo_controller.GetBOCreatorHelper()
                    if bo_creator:
                        print("✅ Got BO creator helper directly!")
                        self.bo_creator = bo_creator
                    else:
                        print("⚠️  GetBOCreatorHelper() returned None")
                except Exception as e:
                    print(f"⚠️  Could not get BO creator helper: {e}")
            
            # If we have creator, try to create rooms
            if self.bo_creator:
                print("🏗️  Creating rooms with creator helper...")
                self.create_rooms_with_creator()
                
                # Don't try batch creation - individual creation was just attempted
                # Callbacks will fire asynchronously and populate self.created_rooms
                # Batch creation is only used as a fallback for failed individual creation
                
                # Don't try to assign users - just create rooms
                if len(self.created_rooms) > 0:
                    print("✅ Breakout rooms created successfully!")
                    print(f"🎯 {len(self.created_rooms)} rooms are ready for manual participant assignment")
                    print("💡 Participants can join rooms via Zoom client interface")
                    self.breakout_rooms_created = True
                else:
                    print("⏳ Waiting for room callbacks to complete...")
                    print("💡 Rooms should appear in Zoom UI shortly")
            else:
                print("❌ No creator helper available")
                print("💡 Breakout room creation requires:")
                print("   - Meeting host privileges")
                print("   - Account with BO creation permissions") 
                print("   - Pro/Business/Enterprise Zoom plan")
                print("   - BO feature enabled in account settings")
                print("📝 Manual creation required via Zoom client")
                
            # Don't try user assignment - just create rooms
            return False
            
        except Exception as e:
            print(f"❌ Error in callback-free room creation: {e}")
            
        return False
            
    def check_creator_rights_fallback(self):
        """Fallback to check creator rights if callback doesn't trigger"""
        try:
            if not self.has_creator_rights:
                print("⏰ Timeout waiting for creator rights notification - trying direct access...")
                
                # Try to get creator helper directly as fallback
                if self.bo_controller:
                    try:
                        creator_helper = self.bo_controller.GetBOCreatorHelper()
                        if creator_helper:
                            print("✅ Got creator helper directly - proceeding with room creation")
                            self.bo_creator = creator_helper
                            self.has_creator_rights = True
                            
                            # Setup event handler manually
                            self.setup_creator_event_handler()
                            
                            # Create rooms with creator method (non-blocking)
                            return self.create_rooms_with_creator()
                        else:
                            print("❌ Creator helper not available - falling back to callback-free approach")
                            # Only run fallback if we haven't already created rooms
                            if not self.breakout_rooms_created:
                                return self.create_breakout_rooms_without_callbacks()
                            else:
                                print("✅ Rooms already created via proper SDK - skipping fallback")
                                return False
                    except Exception as e:
                        print(f"❌ Error in direct creator access: {e}")
                        # Only run fallback if rooms haven't been created
                        if not self.breakout_rooms_created:
                            return self.create_breakout_rooms_without_callbacks()
                        else:
                            return False
                else:
                    print("❌ No BO controller available")
                    return False
            else:
                print("✅ Creator rights already obtained via callback")
                return False  # Don't repeat this timeout
        except Exception as e:
            print(f"❌ Error in creator rights fallback: {e}")
            return False
            
    def create_rooms_with_creator(self):
        """Create rooms using creator helper"""
        # Prevent duplicate room creation attempts
        if self.pending_room_creation:
            print("⚠️  Room creation already in progress - skipping duplicate call")
            return False
        
        try:
            self.pending_room_creation = True
            if self.debug_mode:
                print("\n[DEBUG] === STARTING ROOM CREATION ===")
            
            print("🏗️  Creating 5 breakout rooms...")
            
            # 🚨 CRITICAL: Verify we're at the right stage
            print("\n✅ FLOW VERIFICATION:")
            print("="*60)
            print("1️⃣  ✓ Waiting for MEETING_STATUS_INMEETING - DONE")
            print("2️⃣  ✓ Got BO Creator - READY")
            print("3️⃣  ⚠️  Now creating room...")
            print("4️⃣  ⏳ MUST call StartBO() after creation")
            print("5️⃣  ⏳ Room will then appear in Zoom UI")
            print("="*60 + "\n")
            
            # ⚠️  CRITICAL: Set up event handler FIRST, before any SDK calls!
            if self.debug_mode:
                print("[DEBUG] Setting up event handler before room creation")
            
            print("🔧 Setting up IBOCreatorEvent handler (MUST be done before room creation)...")
            if not self.setup_creator_event_handler():
                print("⚠️  Event handler setup failed, but continuing with room creation...")
            
            # === DIAGNOSTIC CHECKS ===
            print("\n🔍 DIAGNOSTIC CHECKS:")
            if self.debug_mode:
                print("[DEBUG] Running diagnostic checks")
            
            # Check 1: Is BO controller valid?
            if self.bo_controller:
                if self.debug_mode:
                    print(f"[DEBUG] BO Controller type: {type(self.bo_controller)}")
                print(f"✅ BO Controller is valid: {type(self.bo_controller)}")
            else:
                if self.debug_mode:
                    print("[DEBUG] BO Controller is None")
                print("❌ BO Controller is None!")
                return False
            
            # Check 2: Is BO enabled for this meeting?
            try:
                is_bo_enabled = self.bo_controller.IsBOEnabled()
                if self.debug_mode:
                    print(f"[DEBUG] IsBOEnabled() returned: {is_bo_enabled}")
                print(f"   BO Enabled for meeting: {is_bo_enabled}")
                if not is_bo_enabled:
                    print("   ⚠️  BREAKOUT ROOMS NOT ENABLED FOR THIS MEETING!")
                    print("   💡 You may need to enable it in Zoom meeting settings")
                    return False
            except Exception as e:
                if self.debug_mode:
                    print(f"[DEBUG] IsBOEnabled() exception: {e}")
                print(f"   ⚠️  Could not check if BO enabled: {e}")
            
            # Check 3: Is BO started?
            try:
                is_bo_started = self.bo_controller.IsBOStarted()
                if self.debug_mode:
                    print(f"[DEBUG] IsBOStarted() returned: {is_bo_started}")
                print(f"   BO Started: {is_bo_started}")
            except Exception as e:
                if self.debug_mode:
                    print(f"[DEBUG] IsBOStarted() exception: {e}")
                print(f"   ⚠️  Could not check if BO started: {e}")
            
            # Check 4: Is creator helper valid?
            if self.bo_creator:
                if self.debug_mode:
                    print(f"[DEBUG] BO Creator type: {type(self.bo_creator)}")
                print(f"✅ BO Creator is valid: {type(self.bo_creator)}")
            else:
                if self.debug_mode:
                    print("[DEBUG] BO Creator is None")
                print("❌ BO Creator is None!")
                return False
            
            # Check 5: Can we access creator methods?
            has_create_method = hasattr(self.bo_creator, 'CreateBreakoutRoom')
            if self.debug_mode:
                print(f"[DEBUG] CreateBreakoutRoom method available: {has_create_method}")
            print(f"   CreateBreakoutRoom method available: {has_create_method}")
            
            print("🔍 END DIAGNOSTIC CHECKS\n")
            if self.debug_mode:
                print("[DEBUG] Diagnostic checks complete\n")
            
            # === CONFIGURATION ===
            try:
                print("\n🔧 CONFIGURING BREAKOUT ROOM OPTIONS")
                print("="*60)
                
                bo_option = zoom.BOOption()
                
                # Core participant options
                bo_option.IsParticipantCanChooseBO = True
                if self.debug_mode:
                    print("[DEBUG] Set IsParticipantCanChooseBO = True")
                
                bo_option.IsParticipantCanReturnToMainSessionAtAnyTime = True
                if self.debug_mode:
                    print("[DEBUG] Set IsParticipantCanReturnToMainSessionAtAnyTime = True")
                
                # Timer configuration
                bo_option.IsBOTimerEnabled = True
                if self.debug_mode:
                    print("[DEBUG] Set IsBOTimerEnabled = True")
                
                bo_option.nTimerDurationMinutes = 10  # 10 minutes
                if self.debug_mode:
                    print("[DEBUG] Set nTimerDurationMinutes = 10")
                
                bo_option.countdown_seconds = zoom.BO_STOP_COUNTDOWN.BO_STOP_COUNTDOWN_SECONDS_30
                if self.debug_mode:
                    print("[DEBUG] Set countdown_seconds = 30 seconds")
                
                # Optional properties - set if they exist
                try:
                    if hasattr(bo_option, 'IsAutoMoveAllAssignedParticipantsEnabled'):
                        bo_option.IsAutoMoveAllAssignedParticipantsEnabled = False
                        if self.debug_mode:
                            print("[DEBUG] Set IsAutoMoveAllAssignedParticipantsEnabled = False")
                except:
                    pass
                
                try:
                    if hasattr(bo_option, 'IsAttendeeContained'):
                        bo_option.IsAttendeeContained = True  # Allow attendees to join BO rooms
                        if self.debug_mode:
                            print("[DEBUG] Set IsAttendeeContained = True")
                except:
                    pass
                
                try:
                    if hasattr(bo_option, 'IsTimerAutoStopBOEnabled'):
                        bo_option.IsTimerAutoStopBOEnabled = True
                        if self.debug_mode:
                            print("[DEBUG] Set IsTimerAutoStopBOEnabled = True")
                except:
                    pass
                
                try:
                    if hasattr(bo_option, 'IsUserConfigMaxRoomUserLimitsEnabled'):
                        bo_option.IsUserConfigMaxRoomUserLimitsEnabled = True
                        if self.debug_mode:
                            print("[DEBUG] Set IsUserConfigMaxRoomUserLimitsEnabled = True")
                    
                    if hasattr(bo_option, 'nUserConfigMaxRoomUserLimits'):
                        bo_option.nUserConfigMaxRoomUserLimits = 100
                        if self.debug_mode:
                            print("[DEBUG] Set nUserConfigMaxRoomUserLimits = 100")
                except:
                    pass
                
                if self.debug_mode:
                    print("[DEBUG] Calling SetBOOption with full configuration")
                
                # Apply the configuration
                set_result = self.bo_creator.SetBOOption(bo_option)
                if self.debug_mode:
                    print(f"[DEBUG] SetBOOption returned: {set_result}")
                print(f"✅ BO options configured (SetBOOption returned: {set_result})")
                
                # **CRITICAL**: Retrieve and verify the current BOOption was actually set
                print("\n🔍 VERIFYING BOOption Configuration...")
                try:
                    if hasattr(self.bo_creator, 'GetBOOption'):
                        # GetBOOption takes a BOOption reference and modifies it in-place
                        current_option = zoom.BOOption()
                        get_result = self.bo_creator.GetBOOption(current_option)
                        
                        if self.debug_mode:
                            print(f"[DEBUG] GetBOOption returned: {get_result}")
                        
                        if get_result or current_option:
                            print("📋 Current BO Configuration:")
                            print(f"   IsParticipantCanChooseBO: {getattr(current_option, 'IsParticipantCanChooseBO', 'N/A')}")
                            print(f"   IsParticipantCanReturnToMainSessionAtAnyTime: {getattr(current_option, 'IsParticipantCanReturnToMainSessionAtAnyTime', 'N/A')}")
                            print(f"   IsBOTimerEnabled: {getattr(current_option, 'IsBOTimerEnabled', 'N/A')}")
                            print(f"   nTimerDurationMinutes: {getattr(current_option, 'nTimerDurationMinutes', 'N/A')}")
                            print(f"   countdown_seconds: {getattr(current_option, 'countdown_seconds', 'N/A')}")
                            print(f"   IsAttendeeContained: {getattr(current_option, 'IsAttendeeContained', 'N/A')}")
                            
                            if self.debug_mode:
                                print("[DEBUG] GetBOOption retrieved configuration successfully")
                        else:
                            print("⚠️  GetBOOption returned false/none")
                            if self.debug_mode:
                                print("[DEBUG] GetBOOption returned false")
                    else:
                        print("⚠️  GetBOOption method not available")
                        if self.debug_mode:
                            print("[DEBUG] GetBOOption method not found on bo_creator")
                except Exception as e:
                    print(f"⚠️  Could not verify BOOption: {e}")
                    if self.debug_mode:
                        print(f"   [DEBUG] GetBOOption error: {e}")
                
                print("="*60)
                
            except Exception as e:
                print(f"❌ Error in BO options configuration: {e}")
                if self.debug_mode:
                    print(f"   [DEBUG] Configuration error: {e}")
                    import traceback
                    traceback.print_exc()
            
            # ⚠️  CRITICAL: BO must be STOPPED before creating/editing rooms!
            print("🔍 Checking if BO is started...")
            if self.debug_mode:
                print("[DEBUG] Checking BO started status before creation")
            try:
                is_bo_started = self.bo_controller.IsBOStarted()
                print(f"   BO Started: {is_bo_started}")
                
                if is_bo_started:
                    if self.debug_mode:
                        print("[DEBUG] BO is already started - need to stop it")
                    print("⚠️  BO is already started - need to stop it first to create rooms!")
                    print("🛑 Stopping BO to allow room creation...")
                    
                    # Get admin helper to stop BO
                    bo_admin = self.bo_controller.GetBOAdminHelper()
                    if bo_admin:
                        # Try to stop BO (event handler may not be available)
                        try:
                            if self.debug_mode:
                                print("[DEBUG] Calling StopBO()")
                            stop_result = bo_admin.StopBO()
                            if self.debug_mode:
                                print(f"[DEBUG] StopBO returned: {stop_result}")
                            print(f"   StopBO result: {stop_result}")
                            
                            # Give SDK time to process the stop
                            time.sleep(2)
                            print("   ✅ BO stopped (or stopping asynchronously)")
                        except Exception as e:
                            print(f"   ❌ StopBO error: {e}")
                            if self.debug_mode:
                                print(f"   [DEBUG] StopBO exception: {e}")
                    else:
                        print("   ❌ Could not get BO admin helper to stop BO")
                        if self.debug_mode:
                            print("   [DEBUG] GetBOAdminHelper returned None")
            except Exception as e:
                print(f"   ⚠️  Could not check BO status: {e}")
                if self.debug_mode:
                    print(f"   [DEBUG] IsBOStarted error: {e}")
            
            # Create rooms using individual async method (most reliable)
            # Use CreateBreakoutRoom - the current method (CreateBO is deprecated)
            # CreateBreakoutRoom returns bool: True=request sent, False=failed
            # The actual result is delivered via onCreateBOResponse callback
            room_names = ["Team Alpha", "Team Beta", "Team Gamma", "Team Delta", "Team Echo"]
            # Check if any of these rooms already exist to prevent duplicates
            existing_room_names = {room_info['name'] for room_info in self.created_rooms.values()}
            rooms_to_create = [name for name in room_names if name not in existing_room_names]
            
            if not rooms_to_create:
                print("ℹ️  All rooms already created - skipping duplicate room creation")
                print(f"🎯 Existing rooms: {list(existing_room_names)}")
                return False
            
            if len(rooms_to_create) < len(room_names):
                print(f"⚠️  Some rooms already exist, creating remaining: {rooms_to_create}")
                room_names = rooms_to_create
            
            successful_rooms = 0
            
            if self.debug_mode:
                print("[DEBUG] Creating rooms individually using CreateBreakoutRoom (async)")
            print("📝 Creating rooms individually using CreateBreakoutRoom (async)...")
            
            for i, room_name in enumerate(room_names, 1):
                try:
                    print(f"📝 Creating room {i}: '{room_name}'")
                    if self.debug_mode:
                        print(f"   [DEBUG] Calling CreateBreakoutRoom('{room_name}')")
                    
                    # Use CreateBreakoutRoom - the current (non-deprecated) method
                    # Returns: True = async request sent successfully
                    #          False = request failed
                    # Actual result comes via onCreateBOResponse callback
                    result = self.bo_creator.CreateBreakoutRoom(room_name)
                    
                    if self.debug_mode:
                        print(f"   [DEBUG] CreateBreakoutRoom returned: {result} (type: {type(result).__name__})")
                    print(f"   DEBUG: CreateBreakoutRoom returned: {result}")
                    
                    if result is True or result == True:
                        if self.debug_mode:
                            print(f"   [DEBUG] Room creation request sent successfully")
                        print(f"   ✅ CreateBreakoutRoom request sent successfully")
                        print(f"   📮 Waiting for onCreateBOResponse callback...")
                        successful_rooms += 1
                    else:
                        if self.debug_mode:
                            print(f"   [DEBUG] CreateBreakoutRoom returned False")
                        print(f"   ❌ CreateBreakoutRoom returned False")
                        print(f"   This could mean: Not host yet, no permissions, or BO feature disabled")
                    
                    # Small delay between creations to avoid overwhelming SDK
                    time.sleep(0.3)
                        
                except Exception as e:
                    print(f"❌ Error creating room {i} ({room_name}): {e}")
                    if self.debug_mode:
                        print(f"   [DEBUG] Exception: {e}")
                    import traceback
                    traceback.print_exc()
            
            if self.debug_mode:
                print(f"[DEBUG] Room creation request completed: {successful_rooms}/{len(room_names)} sent")
            print(f"🎉 Room creation completed! {successful_rooms}/{len(room_names)} room requests sent.")
            print("⏳ Request is being processed asynchronously by the SDK...")
            print("   StartBO() will be called automatically when onCreateBOResponse callback fires.\n")
            
            # NOTE: Don't call StartBO() here! The room doesn't exist yet.
            # StartBO() is now called from the onCreateBOResponse callback
            # after the room is confirmed created by the SDK.
            
            if successful_rooms == 0:
                print("❌ No rooms were created successfully")
                print("💡 This could indicate:")
                print("   - Bot is not the meeting host yet")
                print("   - Account lacks breakout room creation permissions")
                print("   - BO feature disabled for this account/plan")
                print("   - Need Pro/Business/Enterprise Zoom subscription")
            
        except Exception as e:
            print(f"❌ Error in room creation: {e}")
        finally:
            self.pending_room_creation = False

    def _start_breakout_rooms(self):
        """Called from onCreateBOResponse callback AFTER room is confirmed created.
        This is the ONLY place StartBO() should be called."""
        try:
            print("\n" + "="*60)
            print("🚀 STARTING BREAKOUT ROOMS (triggered by onCreateBOResponse)")
            print("="*60)
            
            # Get BO admin helper if not already available
            if not self.bo_admin:
                print("🔄 Getting BO admin helper...")
                try:
                    self.bo_admin = self.bo_controller.GetBOAdminHelper()
                    if self.bo_admin:
                        print("✅ Got BO admin helper")
                    else:
                        print("❌ Could not get BO admin helper - rooms won't be visible")
                        return False
                except Exception as e:
                    print(f"❌ Error getting admin helper: {e}")
                    return False
            
            # Check CanStartBO (informational)
            if hasattr(self.bo_admin, 'CanStartBO'):
                try:
                    can_start = self.bo_admin.CanStartBO()
                    print(f"   CanStartBO = {can_start}")
                    if self.debug_mode:
                        print(f"[DEBUG] CanStartBO() returned: {can_start}")
                except Exception as e:
                    print(f"   CanStartBO check skipped: {e}")
            
            # Call StartBO() - THIS makes rooms visible in Zoom UI
            if hasattr(self.bo_admin, 'StartBO'):
                print("🚀 Calling admin.StartBO()...")
                try:
                    result = self.bo_admin.StartBO()
                    
                    if self.debug_mode:
                        print(f"[DEBUG] StartBO returned: {result} (type: {type(result).__name__})")
                    
                    if result is True or result == True:
                        print("✅ StartBO() succeeded - rooms should now appear in Zoom UI!")
                    elif result is False or result == False:
                        print("⚠️  StartBO() returned False")
                        print("   Trying again in 2 seconds...")
                        GLib.timeout_add(2000, self._retry_start_bo)
                    else:
                        print(f"⚠️  StartBO() returned: {result}")
                    
                except Exception as e:
                    print(f"❌ Error calling StartBO: {e}")
                    if self.debug_mode:
                        import traceback
                        traceback.print_exc()
            else:
                print("❌ No StartBO method on admin helper!")
                if self.debug_mode:
                    print("[DEBUG] Available admin methods:")
                    for attr in dir(self.bo_admin):
                        if not attr.startswith('_'):
                            print(f"[DEBUG]   - {attr}")
            
            print("="*60)
            
        except Exception as e:
            print(f"❌ Error in _start_breakout_rooms: {e}")
            if self.debug_mode:
                import traceback
                traceback.print_exc()
        
        return False  # Don't repeat GLib timeout

    def _retry_start_bo(self):
        """Retry StartBO() if first attempt returned False"""
        try:
            print("🔄 Retrying StartBO()...")
            if self.bo_admin and hasattr(self.bo_admin, 'StartBO'):
                result = self.bo_admin.StartBO()
                print(f"   StartBO retry result: {result}")
                if result:
                    print("✅ StartBO() succeeded on retry!")
                else:
                    print("⚠️  StartBO() still returned False")
                    print("💡 Check: Zoom meeting settings, host permissions, account plan")
        except Exception as e:
            print(f"❌ StartBO retry error: {e}")
        return False  # Don't repeat


    def on_has_creator_rights_notification(self, *args):
        """Handle when we get creator rights notification - proper SDK callback"""
        try:
            print("🎛️  Received onHasCreatorRightsNotification callback!")
            self.has_creator_rights = True
            
            # Set up creator event handler for room creation callbacks
            self.setup_creator_event_handler()
            
            # Now we can safely create breakout rooms
            print("✅ Creator rights confirmed - ready to create breakout rooms")
            GLib.timeout_add(1000, self.create_rooms_with_creator)
            
        except Exception as e:
            print(f"❌ Error in creator rights notification: {e}")
    
    def setup_creator_event_handler(self):
        """Set up IBOCreatorEvent handler for room creation callbacks"""
        try:
            if self.debug_mode:
                print("🔧 [DEBUG] setup_creator_event_handler called")
            if not self.bo_creator:
                print("⚠️  No BO creator available to setup event handler")
                return False
            
            if self.debug_mode:
                print("🔧 [DEBUG] Creating callback functions...")
            
            # Create proper SDK binding event callbacks
            def on_create_bo_response(success, room_id):
                """Called when breakout room creation completes"""
                try:
                    if self.debug_mode:
                        print(f"🔧 [DEBUG] onCreateBOResponse callback fired: success={success}, room_id={room_id}")
                    
                    print(f"📮 onCreateBOResponse CALLBACK FIRED: success={success}, room_id={room_id}")
                    if success and room_id:
                        # Store the created room ID
                        room_count = len(self.created_rooms) + 1
                        room_names = ["Team Alpha", "Team Beta", "Team Gamma", "Team Delta", "Team Echo"]
                        room_name = room_names[room_count - 1] if room_count <= len(room_names) else f"Room {room_count}"
                        
                        self.created_rooms[room_count] = {
                            'id': str(room_id),
                            'name': room_name
                        }
                        
                        if self.debug_mode:
                            print(f"   [DEBUG] Room stored: {room_name} -> {room_id}")
                        
                        print(f"✅ Room '{room_name}' created successfully with ID: {room_id}")
                        
                        # Check if all 5 rooms are created
                        if len(self.created_rooms) >= 5:
                            self.breakout_rooms_created = True
                            print(f"🎉 All 5 breakout rooms created successfully!")
                            if self.debug_mode:
                                print("   [DEBUG] All rooms now tracked")
                            
                            # ✅ CRITICAL: NOW call StartBO() - all rooms confirmed to exist
                            print("🚀 All rooms confirmed - now calling StartBO() to make them visible in UI...")
                            GLib.timeout_add(500, self._start_breakout_rooms)
                        else:
                            print(f"   📊 {len(self.created_rooms)}/5 rooms created so far... waiting for more callbacks")
                    else:
                        print(f"   ⚠️  Failed: success={success}, room_id={room_id}")
                except Exception as e:
                    print(f"❌ Error in onCreateBOResponse: {e}")
                    import traceback
                    traceback.print_exc()
            
            def on_option_changed(option):
                """Handle BO option changes"""
                try:
                    if self.debug_mode:
                        print(f"🔧 [DEBUG] onBOOptionChanged callback fired: {option}")
                    print(f"⚙️  OnBOOptionChanged CALLBACK FIRED: {option}")
                except Exception as e:
                    print(f"❌ Error in onBOOptionChanged: {e}")
            
            def on_remove_bo_response(success, room_id):
                """Handle BO removal response"""
                try:
                    if self.debug_mode:
                        print(f"🔧 [DEBUG] onRemoveBOResponse callback fired: success={success}, room_id={room_id}")
                    print(f"📮 onRemoveBOResponse CALLBACK FIRED: success={success}, room_id={room_id}")
                except Exception as e:
                    print(f"❌ Error in onRemoveBOResponse: {e}")
            
            def on_update_name_response(success, room_id):
                """Handle BO name update response"""
                try:
                    if self.debug_mode:
                        print(f"🔧 [DEBUG] onUpdateBONameResponse callback fired: success={success}, room_id={room_id}")
                    print(f"📮 onUpdateBONameResponse CALLBACK FIRED: success={success}, room_id={room_id}")
                except Exception as e:
                    print(f"❌ Error in onUpdateBONameResponse: {e}")
            
            # Create the SDK binding event handler with callbacks
            if self.debug_mode:
                print("🔧 [DEBUG] Creating BOCreatorEventCallbacks object...")
            
            self.creator_event_handler = zoom.BOCreatorEventCallbacks(
                onCreateBOResponseCallback=on_create_bo_response,
                onBOOptionChangedCallback=on_option_changed,
                onRemoveBOResponseCallback=on_remove_bo_response,
                onUpdateBONameResponseCallback=on_update_name_response
            )
            
            if self.debug_mode:
                print(f"🔧 [DEBUG] Handler object created: {self.creator_event_handler}")
            
            # Set the event handler on the creator
            try:
                if self.debug_mode:
                    print("🔧 [DEBUG] Calling SetEvent()...")
                
                self.bo_creator.SetEvent(self.creator_event_handler)
                
                if self.debug_mode:
                    print("🔧 [DEBUG] SetEvent() call completed")
                print("✅ IBOCreatorEvent handler set successfully - now ready to create rooms!")
                return True
            except Exception as e:
                print(f"⚠️  Could not set creator event handler: {e}")
                if self.debug_mode:
                    print(f"   [DEBUG] SetEvent error: {e}")
                print("⚠️  Attempting room creation without event handler...")
                return False
            
        except Exception as e:
            print(f"❌ Error setting up creator event handler: {e}")
            import traceback
            traceback.print_exc()
            return False


    def on_has_admin_rights(self, bo_admin=None):
        """Handle when we get admin rights for breakout rooms"""
        try:
            print("🎛️  Got breakout room admin rights!")
            self.bo_admin = bo_admin
            
            print("✅ Admin rights available for user assignment")
            print("🏗️  Now creating breakout rooms and assigning users...")
            
            # First create rooms, then assign users
            GLib.timeout_add(1000, self.create_rooms_with_creator)
            GLib.timeout_add(3000, self.assign_users_with_admin_rights)
            
        except Exception as e:
            print(f"❌ Error in admin rights callback: {e}")





    def fetch_bo_details_and_assign(self):
        """Fetch current breakout room details from SDK and then assign participants"""
        # First try to get BOData helper if not available
        if not self.bo_data_helper and self.bo_controller:
            try:
                self.bo_data_helper = self.bo_controller.GetBODataHelper()
                if self.bo_data_helper:
                    print("✅ Got BOData helper from controller")
            except:
                pass
        
        if not self.bo_data_helper:
            print("❌ No BOData helper available, using created rooms only")
            return self.assign_all_participants_to_rooms()
            
        try:
            print("\n🔍 FETCHING CURRENT BREAKOUT ROOM DETAILS FROM SDK")
            print("="*60)
            
            # Get current breakout room list from SDK using new BOData interface
            bo_list = None
            try:
                if hasattr(self.bo_data_helper, 'GetBOList'):
                    bo_list = self.bo_data_helper.GetBOList()
                elif hasattr(self.bo_data_helper, 'GetBOIDList'):
                    bo_list = self.bo_data_helper.GetBOIDList()
            except Exception as e:
                print(f"⚠️  Could not get BO list: {e}")
            
            sdk_rooms = {}
            
            if bo_list:
                print(f"📊 SDK reports {len(bo_list)} breakout rooms:")
                for i, bo_id in enumerate(bo_list, 1):
                    try:
                        # Get room name using new BOData interface
                        bo_name = None
                        try:
                            if hasattr(self.bo_data_helper, 'GetBONameById'):
                                bo_name = self.bo_data_helper.GetBONameById(bo_id)
                            elif hasattr(self.bo_data_helper, 'GetBOName'):
                                bo_name = self.bo_data_helper.GetBOName(bo_id)
                        except:
                            bo_name = f"Room {i}"
                        
                        sdk_rooms[i] = {'id': bo_id, 'name': bo_name}
                        print(f"🏠 Room {i}: {bo_name} (ID: {bo_id})")
                        
                        # Get participants in this room
                        try:
                            participants_in_room = self.bo_data_helper.GetBOUserList(bo_id)
                            if participants_in_room:
                                print(f"   👥 Participants: {len(participants_in_room)}")
                                for participant_id in participants_in_room:
                                    try:
                                        user_info = self.participants_ctrl.GetUserByUserID(participant_id)
                                        user_name = user_info.GetUserName()
                                        print(f"      👤 {user_name}")
                                    except:
                                        print(f"      👤 Unknown user (ID: {participant_id})")
                            else:
                                print(f"   👥 No participants assigned")
                        except Exception as e:
                            print(f"   ⚠️  Could not get participant list: {e}")
                            
                    except Exception as e:
                        print(f"⚠️  Could not get details for BO {bo_id}: {e}")
                        
                # Update our created_rooms with SDK data
                self.created_rooms.update(sdk_rooms)
                print(f"\n✅ Updated room database with {len(sdk_rooms)} SDK rooms")
                
            else:
                print("❌ No breakout rooms found in SDK!")
                
            print("="*60 + "\n")
            
        except Exception as e:
            print(f"❌ Error fetching BO details from SDK: {e}")
            
        # Now proceed with assignment
        return self.assign_all_participants_to_rooms()

    def on_bo_status_changed(self, status=None):
        """Handle breakout room status changes"""
        try:
            print(f"📊 BO Status changed to: {status}")
        except Exception as e:
            print(f"❌ Error in BO status callback: {e}")

    def leave(self):
        """Leave the meeting"""
        if self.meeting_service:
            leave_result = self.meeting_service.Leave(zoom.LEAVE_MEETING)
            print(f"📤 Leave meeting result: {leave_result}")

    def assign_users_with_admin_rights(self):
        """Assign users to breakout rooms using admin rights from new BOAdmin interface"""
        print("\n🎯 ASSIGNING USERS WITH ADMIN RIGHTS")
        print("="*50)
        
        try:
            if not self.bo_admin:
                print("❌ No admin rights available")
                return False
                
            # Try to get BO data helper for room information
            if not self.bo_data_helper and self.bo_controller:
                try:
                    self.bo_data_helper = self.bo_controller.GetBODataHelper()
                    if self.bo_data_helper:
                        print("✅ Got BOData helper for room info")
                    else:
                        print("⚠️  No BOData helper - using created room data")
                except Exception as e:
                    print(f"⚠️  Could not get BOData helper: {e}")
                
            # Get list of breakout rooms using new BOData interface
            room_list = []
            if self.bo_data_helper:
                try:
                    if hasattr(self.bo_data_helper, 'GetBOList'):
                        room_list = self.bo_data_helper.GetBOList()
                    elif hasattr(self.bo_data_helper, 'GetBOIDList'):
                        room_list = self.bo_data_helper.GetBOIDList()
                    print(f"📊 Found {len(room_list)} rooms via BOData helper")
                except Exception as e:
                    print(f"⚠️  Could not get room list: {e}")
            
            # Fallback to created rooms if data helper fails
            if not room_list and self.created_rooms:
                room_list = [room['id'] for room in self.created_rooms.values()]
                print(f"📊 Using {len(room_list)} rooms from creation data")
            
            if not room_list:
                print("❌ No breakout rooms available for assignment")
                return False
                
            # Get current participants
            if not self.participants_list:
                print("❌ No participants to assign")
                return False
                
            print(f"👥 Assigning {len(self.participants_list)} participants to {len(room_list)} rooms")
            
            successful_assignments = 0
            
            # Round-robin assignment using admin rights from new BOAdmin interface
            for i, participant in enumerate(self.participants_list):
                try:
                    # Select room using round-robin
                    room_index = i % len(room_list)
                    breakout_id = room_list[room_index]
                    user_id = str(participant['id'])
                    
                    print(f"🔄 Assigning {participant['name']} to room {room_index + 1}")
                    print(f"   User ID: {user_id}, Breakout ID: {breakout_id}")
                    
                    # Try new BOAdmin methods for assignment
                    result = False
                    if hasattr(self.bo_admin, 'AssignNewUserToRunningBO'):
                        result = self.bo_admin.AssignNewUserToRunningBO(user_id, breakout_id)
                    elif hasattr(self.bo_admin, 'SwitchAssignedUserToRunningBO'):
                        result = self.bo_admin.SwitchAssignedUserToRunningBO(user_id, breakout_id)
                    
                    if result:
                        print(f"✅ {participant['name']} assigned successfully")
                        successful_assignments += 1
                    else:
                        print(f"❌ Failed to assign {participant['name']}")
                        
                except Exception as e:
                    print(f"❌ Error assigning {participant['name']}: {e}")
                    
            print("="*50)
            print(f"✅ Assignment completed: {successful_assignments}/{len(self.participants_list)} users assigned")
            
            if successful_assignments > 0:
                print("🎉 Users should now see breakout room assignments in their Zoom client!")
            else:
                print("💡 If assignments failed, check:")
                print("   - Account has breakout room admin permissions")
                print("   - Breakout rooms are properly created and running")
                print("   - Users are in the main meeting room")
            
        except Exception as e:
            print(f"❌ Error in admin assignment: {e}")
            
        return False

    def cleanup(self):
        """Clean up resources"""
        print("🧹 Cleaning up...")
        
        # Set flags to prevent callbacks during cleanup
        self.is_meeting_started = False
        self.breakout_rooms_created = False
        
        try:
            if self.meeting_service:
                zoom.DestroyMeetingService(self.meeting_service)
                print("✅ Destroyed meeting service")
        except Exception as e:
            print(f"⚠️  Error destroying meeting service: {e}")

        try:
            if self.setting_service:
                zoom.DestroySettingService(self.setting_service)
                print("✅ Destroyed setting service")
        except Exception as e:
            print(f"⚠️  Error destroying setting service: {e}")

        try:
            if self.auth_service:
                zoom.DestroyAuthService(self.auth_service)
                print("✅ Destroyed auth service")
        except Exception as e:
            print(f"⚠️  Error destroying auth service: {e}")

        try:
            zoom.CleanUPSDK()
            print("✅ SDK cleanup completed")
        except Exception as e:
            print(f"⚠️  Error during SDK cleanup: {e}")

    def shutdown(self):
        """Shutdown the bot gracefully"""
        print("🛑 Shutting down...")
        if self.main_loop and self.main_loop.is_running():
            self.main_loop.quit()

    def on_signal(self, signum, frame):
        """Handle interrupt signals"""
        print(f"\n🛑 Received signal {signum}, shutting down...")
        self.leave()
        GLib.timeout_add(1000, self.shutdown)

    def run(self):
        """Main run loop"""
        try:
            print("🚀 Starting Breakout Room Host Bot...")
            
            # Initialize SDK
            self.init()
            
            # Authenticate
            self.authenticate()
            
            # Set up signal handlers
            signal.signal(signal.SIGINT, self.on_signal)
            signal.signal(signal.SIGTERM, self.on_signal)
            
            # Create and run main loop
            self.main_loop = GLib.MainLoop()
            print("🔄 Starting main event loop...")
            self.main_loop.run()
            
        except Exception as e:
            print(f"❌ Error in main run loop: {e}")
        finally:
            self.cleanup()

def main():
    """Main entry point"""
    # Load environment variables
    load_dotenv()
    
    # Create and run the bot
    bot = BreakoutRoomHostBot()
    bot.run()

if __name__ == "__main__":
    main()