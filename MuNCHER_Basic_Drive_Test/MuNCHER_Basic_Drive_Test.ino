#include <ESP32Encoder.h>
#include <PID_v1.h>

// to-do
// comment out the PID controllers we're not going to use because there are no quadrature encoders on this
// figure out what the implication of not having all 4 quadrature encoders is

// define PID variables for all the motors
double setPointFrontLeft, inputFrontLeft, outputFrontLeft; // PointFrontLeft is the PID target value of the controller, inputFrontLeft is the current actual value from process, outputFrontLeft used to control motor (this should be PWM signal value to control speed)
double setPointFrontRight, inputFrontRight, outputFrontRight;
double setPointBackRight, inputBackRight, outputBackRight;
double setPointBackLeft, inputBackLeft, outputBackLeft;


// these gains are crucial to match the dynamics of our system
// kp (how aggressively the PID reacts to the current error)
// ki (influence of the sum of past errors)
// kd (influence of rate of change of error - helps predict future error)
double KpFrontLeft = 2.0,  KiFrontLeft = 5.0,   KdFrontLeft = 2.0;
double KpFrontRight = 2.0, KiFrontRight = 5.0,  KdFrontRight = 2.0;
double KpBackRight = 2.0,  KiBackRight = 5.0,   KdBackRight = 2.0;
double KpBackLeft = 2.0,   KiBackLeft = 5.0,    KdBackLeft = 2.0;

// Instantiate objects for each PIE
PID pidFrontLeft(&inputFrontLeft, &outputFrontLeft, &setPointFrontLeft, KpFrontLeft, KiFrontLeft, KdFrontLeft, DIRECT);
PID pidFrontRight(&inputFrontRight, &outputFrontRight, &setPointFrontRight, KpFrontRight, KiFrontRight, KdFrontRight, DIRECT);
PID pidBackLeft(&inputBackLeft, &outputBackLeft, &setPointBackLeft, KpBackLeft, KiBackLeft, KdBackLeft, DIRECT);
PID pidBackRight(&inputBackRight, &outputBackRight, &setPointBackRight, KpBackRight, KiBackRight, KdBackRight, DIRECT);

ESP32Encoder frontL;
ESP32Encoder frontR;
ESP32Encoder rearL;
ESP32Encoder rearR;

#define FRONT_LEFT 0
#define FRONT_LEFT_QUAD_A   13
#define FRONT_LEFT_QUAD_B   12
#define FRONT_LEFT_PWM      16
#define FRONT_LEFT_DIR      17

#define FRONT_RIGHT 1
#define FRONT_RIGHT_QUAD_A  14
#define FRONT_RIGHT_QUAD_B  27
#define FRONT_RIGHT_PWM     5
#define FRONT_RIGHT_DIR     18

#define BACK_LEFT 2
#define BACK_LEFT_QUAD_A    26
#define BACK_LEFT_QUAD_B    25
#define BACK_LEFT_PWM       19
#define BACK_LEFT_DIR       21

#define BACK_RIGHT 3
#define BACK_RIGHT_QUAD_A   33
#define BACK_RIGHT_QUAD_B   32
#define BACK_RIGHT_PWM      3
#define BACK_RIGHT_DIR      1

void setup(){
  
  Serial.begin(115200);

  //Set up motor encoders
  setupEncoders();

  //Set up motor drives
  setupMotors();

  // Initialize all PID controllers
  pidFrontLeft.SetMode(AUTOMATIC);
  pidFrontLeft.SetOutputLimits(-255, 255);

  pidFrontRight.SetMode(AUTOMATIC);
  pidFrontRight.SetOutputLimits(-255, 255);

  pidBackLeft.SetMode(AUTOMATIC);
  pidBackLeft.SetOutputLimits(-255, 255);

  pidBackRight.SetMode(AUTOMATIC);
  pidBackRight.SetOutputLimits(-255, 255);
  
}

void loop(){
  // Loop and read the count
  Serial.println("Encoder count = " + String((int32_t)frontL.getCount()) + " " + String((int32_t)frontR.getCount()) + " " + String((int32_t)rearL.getCount()) + " " + String((int32_t)rearR.getCount()));
  delay(100);

  // Update the input for each PID controller
  inputFrontLeft = frontL.getCount();
  inputFrontRight = frontR.getCount();
  inputBackLeft = rearL.getCount();
  inputBackRight = rearR.getCount();

  // Compute the new PID output value for each motor
  pidFrontLeft.Compute();
  pidFrontRight.Compute();
  pidBackLeft.Compute();
  pidBackRight.Compute();

  // Use the output to control the motor speed and direction
  writeMotor(FRONT_LEFT, outputFrontLeft);
  writeMotor(FRONT_RIGHT, outputFrontRight);
  writeMotor(BACK_LEFT, outputBackLeft);
  writeMotor(BACK_RIGHT, outputBackRight);
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

void setupEncoders() {
  // Enable the weak pull down resistors

  //ESP32Encoder::useInternalWeakPullResistors=DOWN;
  // Enable the weak pull up resistors
  ESP32Encoder::useInternalWeakPullResistors=UP;

  //Attach quadrature encoder pin inputs
  frontL.attachHalfQuad(FRONT_LEFT_QUAD_A, FRONT_LEFT_QUAD_B);
  frontR.attachHalfQuad(FRONT_RIGHT_QUAD_A, FRONT_RIGHT_QUAD_B);
  rearL.attachHalfQuad(BACK_LEFT_QUAD_A, BACK_LEFT_QUAD_B);
  rearR.attachHalfQuad(BACK_RIGHT_QUAD_A, BACK_RIGHT_QUAD_B);

  //Reset tick counts for all encoders
  frontL.clearCount();
  frontR.clearCount();
  rearL.clearCount();
  rearR.clearCount();
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

void differentialDrive(int speed, int distance, int turnPercent) {
//  writeMotor(uint8_t motor, int speedVector)
} 

void parseAndAssignMotorValues(const std::string& val) {
  // First step is to handle the comma delimiter and include exceptions if first and last index are not 'a'
  if (val.front() == 'a' && val.back() == 'a') {
    std::stringstream ss(val.substr(1, val.length() - 2)); // remove the first and last
    std::string token; // temp variable that when looping through ss, we store each value

    int counter = 0;
    while (getline(ss, token, ',')) {
      int value = std::stoi(token); 
      if (counter == 0) {
        setPointFrontLeft = value;

      } else if (counter == 1) {
        setPointFrontRight = value;

      } else if (counter == 2) {
        setPointBackLeft = value;

      } else if (counter == 3) {
        setPointBackRight = value;
      }

      counter ++;
    }
      // convert the token to an integer 
       // this is a temporary variable that stores int of current token
    
    if (counter != 4) {
      std::cerr << "Incorrect number of values provided." << std::endl;

} else {
      std::cerr << "Input string format is incorrect." << std::endl;
    }
  }
}