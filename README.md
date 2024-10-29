# PyZoomMeetingSDK
![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/noah-duncan/py-zoom-meeting-sdk/ci.yml?label=tests)
[![](https://img.shields.io/pypi/v/zoom-meeting-sdk.svg?color=brightgreen)](https://pypi.org/pypi/zoom-meeting-sdk/)
![PyPI - License](https://img.shields.io/pypi/l/zoom-meeting-sdk)


Python bindings for the Zoom Meeting SDK for Linux.

## Requirements

Only runs on Linux. Requires Python 3.10+

A Dockerfile to run a suitable Linux machine is provided in the repository. Run `docker compose run --rm develop` from the repository root to start it.

## Installation

### Simple

1. cd to repository root
2. `docker compose run --rm develop`
3. `pip install zoom-meeting-sdk`
4. `import zoom_meeting_sdk`

### Complex (For modifying the bindings)
1. cd to repository root
2. `docker compose run --rm develop`
3. Get your 
