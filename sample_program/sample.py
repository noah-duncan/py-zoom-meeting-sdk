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

def on_signal(signum, frame):
    print(f"Received signal {signum}")
    sys.exit(0)

def on_exit():
    print("Exiting...")
    bot.leave()
    bot.cleanup()

def on_timeout():
    return True  # Returning True keeps the timeout active

bot=None
def run():
    global bot
    bot = MeetingBot()
    bot.init()   
    
    # Create a GLib main loop
    main_loop = GLib.MainLoop()

    # Add a timeout function that will be called every 100ms
    GLib.timeout_add(100, on_timeout)

    # Run the main loop
    try:
        print("Start main event loop")
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





