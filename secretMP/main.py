import stt
#from stt import listen_for_wake_word
import tts
from tts import convert_to_good_audio, get_gpt4_response
import time

def main():
    while True: 
        #stt.listen_for_wake_word()
        spoken_text = stt.capture_and_transcribe() # transcribe the text (working well)
        tts.convert_to_good_audio(spoken_text)

if __name__ == "__main__":
    main()