#define FRONT_LEFT 0
#define FRONT_LEFT_PWM      4
#define FRONT_LEFT_DIR      16

#define FRONT_RIGHT 1
#define FRONT_RIGHT_PWM     17
#define FRONT_RIGHT_DIR     18

#define BACK_LEFT 2
#define BACK_LEFT_PWM       19
#define BACK_LEFT_DIR       21

#define BACK_RIGHT 3
#define BACK_RIGHT_PWM      22
#define BACK_RIGHT_DIR      23

void setup() {
  
  Serial.begin(115200);

  //Set up motor drives
  setupMotors();
//  Serial.print("init");
}

String test = "";
String newString = "";
int i = 0;

int outputVal[4]  = {0,0,0,0};

void loop() {
  outputVal[0] = 0;
  outputVal[1] = 0;
  outputVal[2] = 0;
  outputVal[3] = 0;
    test = "";
    newString = "";
    while(Serial.available() > 0) {
      // read the incoming byte:
      test = test + String(char(Serial.read()));
    }
//    Serial.println(test[1], DEC);
  if(test.charAt(0) == 'a' && test.charAt(test.length()-2) == 'a') {
    i=0;
    newString = test.substring(1, test.length()-1);
    
    
    while(newString.indexOf(',') > 0) {
      outputVal[i] = int(min(max(newString.substring(0, newString.indexOf(',')).toInt(),-255L), 255L));
        newString = newString.substring(newString.indexOf(',') + 1, newString.length());
        i++;
    }

//    writeMotor(FRONT_LEFT, outputVal[0]);
//    writeMotor(FRONT_RIGHT, outputVal[1]);
//    writeMotor(BACK_LEFT, outputVal[2]);
//    writeMotor(BACK_RIGHT, outputVal[3]);
//
//    delay(50);
  }

//  Serial.print(outputVal[0]);
//  Serial.print(" ");
//  Serial.print(outputVal[1]);
//  Serial.print(" ");
//  Serial.print(outputVal[2]);
//  Serial.print(" ");
//  Serial.println(outputVal[3]);
  
  writeMotor(FRONT_LEFT, outputVal[0]);
  writeMotor(FRONT_RIGHT, outputVal[1]);
  writeMotor(BACK_LEFT, outputVal[2]);
  writeMotor(BACK_RIGHT, outputVal[3]);

  delay(50);


}


void setupMotors() {
  //Init PWM interrupts
  ledcSetup(0, 5000, 8);
  ledcSetup(1, 5000, 8);
  ledcSetup(2, 5000, 8);
  ledcSetup(3, 5000, 8);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(FRONT_LEFT_PWM,  0);
  ledcAttachPin(FRONT_RIGHT_PWM, 0);
  ledcAttachPin(BACK_LEFT_PWM,   0);
  ledcAttachPin(BACK_RIGHT_PWM,  0);

  //Init all motors to speed 0
  ledcWrite(FRONT_LEFT,  0);
  ledcWrite(FRONT_RIGHT, 0);
  ledcWrite(BACK_LEFT,   0);
  ledcWrite(BACK_RIGHT,  0);

  //Set motor direction pins to output
  pinMode(FRONT_LEFT_DIR,  OUTPUT);
  pinMode(FRONT_RIGHT_DIR, OUTPUT);
  pinMode(BACK_LEFT_DIR,   OUTPUT);
  pinMode(BACK_RIGHT_DIR,  OUTPUT);

  //Set motor direction to forwards
  digitalWrite(FRONT_LEFT_DIR, LOW);
  digitalWrite(FRONT_RIGHT_DIR, LOW);
  digitalWrite(BACK_LEFT_DIR, LOW);
  digitalWrite(BACK_RIGHT_DIR, LOW);
}


// Updated writeMotor function to work with PID outputs
void writeMotor(uint8_t motor, int output) {
  int speed = abs(output); // Get the absolute value for speed
  if (speed > 255) speed = 255; // Constrain the speed to 255

  bool direction = output >= 0; // Determine the direction based on the sign of the output

  switch(motor) {
    case FRONT_LEFT:
      digitalWrite(FRONT_LEFT_DIR, direction ? HIGH : LOW); // Set direction
      ledcWrite(FRONT_LEFT, speed); // Set speed
      return;
    case FRONT_RIGHT:
      digitalWrite(FRONT_RIGHT_DIR, direction ? HIGH : LOW);
      ledcWrite(FRONT_RIGHT, speed);
      return;
    case BACK_LEFT:
      digitalWrite(BACK_LEFT_DIR, direction ? HIGH : LOW);
      ledcWrite(BACK_LEFT, speed);
      return;
    case BACK_RIGHT:
      digitalWrite(BACK_RIGHT_DIR, direction ? HIGH : LOW);
      ledcWrite(BACK_RIGHT, speed);
      return;
    default:
      // Handle invalid motor index, if necessary
      return;
  }
}
