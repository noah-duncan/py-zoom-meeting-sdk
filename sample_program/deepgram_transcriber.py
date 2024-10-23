from deepgram.utils import verboselogs
import os

from deepgram import (
    DeepgramClient,
    DeepgramClientOptions,
    LiveTranscriptionEvents,
    LiveOptions,
    Microphone,
)

import asyncio

class DeepgramTranscriber:
    def __init__(self):
        # Configure the DeepgramClientOptions to enable KeepAlive for maintaining the WebSocket connection (only if necessary to your scenario)
        config = DeepgramClientOptions(
            options={"keepalive": "true"}
        )

        # Create a websocket connection using the DEEPGRAM_API_KEY from environment variables
        self.deepgram = DeepgramClient(os.environ.get('DEEPGRAM_API_KEY'), config)

        # Use the listen.live class to create the websocket connection
        self.dg_connection = self.deepgram.listen.websocket.v("1") 

        def on_message(self, result, **kwargs):
            #print("got")
            #print(result)
            #print(result.channel.alternatives[0])
            sentence = result.channel.alternatives[0].transcript
            if len(sentence) == 0:
                return
            print(f"Transcription: {sentence}")

        self.dg_connection.on(LiveTranscriptionEvents.Transcript, on_message)

        def on_error(self, error, **kwargs):
            print(f"Error: {error}")

        self.dg_connection.on(LiveTranscriptionEvents.Error, on_error)

        options = LiveOptions(
            model="nova-2-conversationalai",
            punctuate=True,
            interim_results=True,
            language='en-GB',
            encoding= "linear16",
            sample_rate=32000
            )

        self.dg_connection.start(options)

    def send(self, data):
        self.dg_connection.send(data)

    def finish(self):
        self.dg_connection.finish()

PCM_FILE_PATH = 'sample_program/out/test_audio_16778240.pcm'
CHUNK_SIZE = 64000*10
async def send_pcm(transcriber):
    with open(PCM_FILE_PATH, 'rb') as pcm_file:
        while True:
             chunk = pcm_file.read(CHUNK_SIZE)
             if not chunk:
                 break
             transcriber.send(chunk)
             await asyncio.sleep(0.1)

def get_pcm_chunk():
    with open(PCM_FILE_PATH, 'rb') as pcm_file:
        while True:
             chunk = pcm_file.read(CHUNK_SIZE)
             return chunk