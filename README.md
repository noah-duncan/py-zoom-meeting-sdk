# PyZoomMeetingSDK: Create Zoom Meeting Bots in Python
![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/noah-duncan/py-zoom-meeting-sdk/ci.yml?label=tests)
[![](https://img.shields.io/pypi/v/zoom-meeting-sdk.svg?color=brightgreen)](https://pypi.org/pypi/zoom-meeting-sdk/)
![PyPI - License](https://img.shields.io/pypi/l/zoom-meeting-sdk)

This library creates Python bindings for the Zoom Meeting SDK for Linux. The Zoom Meeting SDK lets you create bots that can join Zoom meetings and record real-time audio. It powers applications like Gong or Otter.ai. Some of the more powerful features of the Meeting SDK, such as separate audio streams per participant were only supported in the C++ versions of the SDK. 

## Requirements

- Docker (You can also use natively if you're running Linux)
- Python 3.10+ (Included in the DockerFile)
- Zoom App Client ID and Client Secret (Instructions for obtaining them are below)
- Deepgram API Key (For running real-time transcription in the sample program)

## Installation

1. cd to repository root
2. `docker compose run --rm develop`
3. `pip install zoom-meeting-sdk`
4. `import zoom_meeting_sdk`

## Running the sample program

### 1. Get your Zoom App Credentials

- Navigate to [Zoom Marketplace](https://marketplace.zoom.us/) and register/log into your
developer account.
- Click the "Develop" button at the top-right, then click 'Build App' and choose "General App".
- Copy the Client ID and Client Secret from the 'App Credentials' section
- Go to the Embed tab on the left navigation bar under Features, then select the Meeting SDK toggle.

For more information, you can follow [this guide](https://developers.zoom.us/docs/meeting-sdk/developer-accounts/)

### 2. Create your .env file
- Create a plaintext file called `.env` in the repository root
- Fill it out like this
```
ZOOM_APP_CLIENT_ID=<your zoom app's client id>
ZOOM_APP_CLIENT_SECRET=<your zoom app's client secret>
MEETING_ID=<id of meeting on your developer account>
MEETING_PWD=<password of meeting on your developer account, taken from URL> 
DEEPGRAM_API_KEY=<your deepgram API key (optional)>
```
### 3. Run the sample program
- Open Zoom and start the meeting you listed in the `.env` file
- Run `docker compose run --rm develop` to enter the docker container
- Run `python sample_program/sample.py`
- The bot should request to join the meeting, then request to record the meeting. Once you accept, it will start playing pre-recorded audio in the meeting and print out live transcripts of your speech in the terminal if you provided a deepgram api key. If you didn't it will print out the volume.

## Modifying the binds

1. Download the latest version of the Zoom Meeting SDK for Linux from the Zoom Marketplace and and extract it into
the [src/zoomsdk](src/zoomsdk) folder of this repository. To download the SDK, open your Zoom App in the developer portal, go to the Embed tab on the left navigation bar under Features, then select the Meeting SDK toggle. Then download the Linux SDK.
2. cd to respository root
3. `docker compose run --rm develop`
4. `source scripts/build.sh`
5. `import zoom_meeting_sdk`

To recompile the bindings run `source scripts/build.sh` again.

The bindings are in the src folder. They are written in C++ and use the [nanobind](https://github.com/wjakob/nanobind) library.
The file structure of the bindings mirrors the file structure of the Zoom Meeting SDK headers. IE the bindings for `zoom_sdk_def.h` are in `zoom_sdf_def_bindings.cpp`
