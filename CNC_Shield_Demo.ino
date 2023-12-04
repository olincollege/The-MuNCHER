// // CNC Shield Stepper  Control Demo

// const int StepX = 2;
// const int DirX = 5;

// void setup() {
//   pinMode(StepX,OUTPUT);
//   pinMode(DirX, OUTPUT);
// }

// void loop() {
//  digitalWrite(DirX, HIGH); // set direction, HIGH for clockwise, LOW for anticlockwise
 
//  for(int x = 0; x<200; x++) { // loop for 200 steps
//   digitalWrite(StepX,HIGH);
//   delayMicroseconds(500);
//   digitalWrite(StepX,LOW); 
//   delayMicroseconds(500);
//  }
// delay(1000); // delay for 1 second

// }
void setup() {
  // put your setup code here, to run once:
pinMode(15, OUTPUT);
for(int i = 0; i < 200; i++) {
  digitalWrite(15,LOW);
  delayMicroseconds(500);
  digitalWrite(15,HIGH);
  delayMicroseconds(500);
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:
}
