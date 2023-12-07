import sounddevice as sd
from google.cloud import speech_v1p1beta1 as speech
import numpy as np
import os
from tts import get_gpt4_response

os.environ["GOOGLE_APPLICATION_CREDENTIALS"] = "/Users/zaynpatel/muncher/The-MuNCHER/credentialsGCloud.json"

# Initialize the client
client = speech.SpeechClient()

# Audio capture settings
sample_rate = 16000
duration = 10  # seconds
channels = 1

# Initialize speech config
config = speech.RecognitionConfig(
    encoding=speech.RecognitionConfig.AudioEncoding.LINEAR16,
    sample_rate_hertz=sample_rate,
    language_code="en-US",
    enable_automatic_punctuation = True
)
streaming_config = speech.StreamingRecognitionConfig(
    config=config,
    interim_results=False, # I set this to False so we're not sending interim transcripts, only the final one
)

def capture_and_transcribe():
    # Capture audio
    print("Listening...")
    audio_data = sd.rec(
        int(sample_rate * duration),
        samplerate=sample_rate,
        channels=channels,
        dtype=np.int16,
    )
    sd.wait()

    # Prepare requests generator
    def requests_gen(audio_data):
        yield speech.StreamingRecognizeRequest(audio_content=audio_data.tobytes())

    # Transcribe
    responses = client.streaming_recognize(streaming_config, requests_gen(audio_data))

    for response in responses:
        for result in response.results:
            spoken_text = result.alternatives[0].transcript
            gpt_response = get_gpt4_response(spoken_text)
            return gpt_response
