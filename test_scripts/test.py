import subprocess
import time
import signal
import sys
import os

def run_and_terminate(program_path):
    # Start the process
    process = subprocess.Popen(['python3', program_path])
    
    print(f"Started process with PID: {process.pid}")
    
    # Wait for 25 seconds or until the process terminates
    try:
        process.wait(timeout=25)
        print("Process terminated on its own.")
        return
    except subprocess.TimeoutExpired:
        print("Process did not terminate on its own. Attempting to terminate.")
    
    # Try to terminate the process gracefully
    process.terminate()
    
    # Wait for up to 5 seconds for the process to terminate
    try:
        process.wait(timeout=5)
    except subprocess.TimeoutExpired:
        print("Process didn't terminate after 5 seconds. Sending SIGINT repeatedly.")
        
        # If the process hasn't terminated, send SIGINT repeatedly
        for _ in range(10):  # Try 10 times
            process.send_signal(signal.SIGINT)  # signal 2 is SIGINT
            time.sleep(0.5)  # Wait half a second between signals
            
            if process.poll() is not None:
                print("Process terminated.")
                return
        
        print("Process still running. Killing forcefully.")
        process.kill()  # If it's still running, forcefully kill it

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python3 test.py <path_to_program_to_run>")
        sys.exit(1)
    
    program_to_run = sys.argv[1]
    run_and_terminate(program_to_run)

    if not os.path.exists("/tmp/test_passed"):
        raise Exception("Test failed")