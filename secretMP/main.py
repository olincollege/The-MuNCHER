import stt
import tts
from tts import convert_to_good_audio, get_gpt4_response

def main():
    spoken_text = stt.capture_and_transcribe() # transcribe the text (working well)
    #save_this = tts.get_gpt4_response(spoken_text) # take the transcribed text and pass it to gpt
    #msave_text_output = tts.get_gpt4_response(save_this)
    tts.convert_to_good_audio(spoken_text)

if __name__ == "__main__":
    main()