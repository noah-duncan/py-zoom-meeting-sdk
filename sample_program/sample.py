import zoom_meeting_sdk as zoom
import time
import jwt
from datetime import datetime, timedelta
from typing import Callable, Optional
import asyncio
from meeting_bot import MeetingBot
from dotenv import load_dotenv
import signal
import sys
import gi
gi.require_version('GLib', '2.0')
from gi.repository import GLib
import os

class ZoomBotRunner:
    def __init__(self):
        self.bot = None
        self.main_loop = None
        self.shutdown_requested = False

    def exit_process(self):
        """Clean shutdown of the bot and main loop"""
        print("Starting cleanup process...")
        
        # Set flag to prevent re-entry
        if self.shutdown_requested:
            return False
        self.shutdown_requested = True
        
        try:
            if self.bot:
                print("Leaving meeting...")
                self.bot.leave()
                print("Cleaning up bot...")
                self.bot.cleanup()
            
                self.force_exit()
             
        except Exception as e:
            print(f"Error during cleanup: {e}")
            self.force_exit()
        
        return False

    def force_exit(self):
        """Force the process to exit"""
        print("Forcing exit...")
        os._exit(0)  # Use os._exit() to force immediate termination
        return False

    def on_signal(self, signum, frame):
        """Signal handler for SIGINT and SIGTERM"""
        print(f"\nReceived signal {signum}")
        # Schedule the exit process to run soon, but not immediately
        if self.main_loop:
            GLib.timeout_add(100, self.exit_process)
        else:
            self.exit_process()

    def on_timeout(self):
        """Regular timeout callback"""
        if self.shutdown_requested:
            return False
        return True

    def run(self):
        """Main run method"""
        self.bot = MeetingBot()
        try:
            self.bot.init()
        except Exception as e:
            print(e)
            self.exit_process()
        

        # Create a GLib main loop
        self.main_loop = GLib.MainLoop()

        # Add a timeout function that will be called every 100ms
        GLib.timeout_add(100, self.on_timeout)

        try:
            print("Starting main event loop")
            self.main_loop.run()
        except KeyboardInterrupt:
            print("Interrupted by user, shutting down...")
        except Exception as e:
            print(f"Error in main loop: {e}")
        finally:
            self.exit_process()

def main():
    load_dotenv()
    
    runner = ZoomBotRunner()
    
    # Set up signal handlers
    signal.signal(signal.SIGINT, runner.on_signal)
    signal.signal(signal.SIGTERM, runner.on_signal)
    
    # Run the Meeting Bot
    runner.run()

if __name__ == "__main__":
    main()