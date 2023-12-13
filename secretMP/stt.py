import sounddevice as sd
from google.cloud import speech_v1p1beta1 as speech
import numpy as np
import os
from tts import get_gpt4_response

# Add audio feedback since I realized we're going to run this headless so the user will need a way to know when the code is listening/not
# MHz/sec, how long wave, how many samples per duration
def give_audio_feedback(frequency=1000, duration=0.1, sample_rate=44100):
    # .arange() returns evenly spaced values for a given region
    samples = np.arange(duration * sample_rate)
    wave = 10000 * np.sin(2 * np.pi * frequency * samples / sample_rate)
    # convert to 16-bit audio
    wave = wave.astype(np.int16)
    sd.play(wave, sample_rate)
    sd.wait()

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
    give_audio_feedback() # pass function w/out args b/c of kwargs in definition

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