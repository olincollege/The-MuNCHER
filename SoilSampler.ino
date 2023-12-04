#include <ezButton.h>

ezButton limitSwitch(17);  // use 17 for testing
int initial_check = 0;
bool moving = false;
void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  limitSwitch.setDebounceTime(50);  // set debounce time to 50 milliseconds
}

void loop() {
  //call check to see if drill is zeroed for the first time
  outputVal[0] = 0;
  outputVal[1] = 0;
  test = "";
  newString = "";
  while (Serial.available() > 0) {
    // read the incoming byte:
    test = test + String(char(Serial.read()));
  }
  //    Serial.println(test[1], DEC);
  if (test.charAt(0) == 'a' && test.charAt(test.length() - 1) == 'a') {
    i = 0;
    newString = test.substring(1, test.length() - 1);

    while (newString.indexOf(',') > 0) {
      outputVal[i] = int(min(max(newString.substring(0, newString.indexOf(',')).toInt(), -255L), 255L));
      newString = newString.substring(newString.indexOf(',') + 1, newString.length());
      i++;
    }
  }
  if (initial_check < 1 || outputVal[1] = 1) {
    moving = true;
    drillZero(initial_check);
    plateZero(); // set plate 0 to over cartridge 1
  }
  if ((moving == false) && (initial_check > 0)) {

    if (outputVal[0] == 1) {
      moving = true;
      loadCartridge(); 
    }
    if (outputVal[0] == 2) {
      moving = true;
      drillCartridge();
    }

  } 
  else {
    Serial.println("Busy");
  }
  delay(50);
}
//have functions include while loop till action is done
void drillZero(inital_check) {
  limitSwitch.loop();  // MUST call the loop() function first
  //call servo function
  drillServo();          //use to bring servo up constantly library
  check = drillLimit();  //check limit switch library, expect a return and if return a 1 (if switch is triggered) or a 0
  initial_check = initial_check + check;
  if (check == 1) {
    //set drill servo point to 0 code?
  }
  // if(limitSwitch.isPressed())
  //   Serial.println("The limit switch: UNTOUCHED -> TOUCHED");

  // if(limitSwitch.isReleased())
  //   Serial.println("The limit switch: TOUCHED -> UNTOUCHED");

  // int state = limitSwitch.getState();
  // if(state == HIGH)
  //   // Serial.println("The limit switch: UNTOUCHED");
  // else
  //   Serial.println("The limit switch: TOUCHED");
  //   inital_check ++;
  //   return("s");
}

void drillServo(){
  //use ian's drill servo code
}

void loadCartridge() {

  moving = false;
}

void drillCatridge(){

  moving = false;
}



