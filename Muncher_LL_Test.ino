#include "MuNCHER.h"
MuncherIO m;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(250000);
  delay(800);
  m.init();
  m.initSteppers();
  m.calibratePlatter();
  delay(1000);
  // m.setZSpeed(3000);
  //  m.setZHeight(3000);
  
 
//  m.setTurretPosition(800);


//  delay(1000);
  Serial.print("beginning");
  // m.setDrillSpeed(50);
//    delay(2000);
//    m.setDrillSpeed(0);
//    delay(2000);
//      m.setDrillSpeed(-50);
//    delay(2000);
}

int state = 0;
int slot = 1;
int slot_pos = 0;



void loop() {
    // m.setDrillSpeed(20);
    // delay(2000);
    // m.setDrillSpeed(0);
    // delay(2000);
    
      // m.setDrillSpeed(-50); //for drill -255 to 255. 28 is slowest. 
//     delay(2000);
// m.setDrillSpeed(0);
//     delay(2000);
  // put your main code here, to run repeatedly:

if(slot == 0) {slot_pos = 0;}
else if(slot == 1) {slot_pos = 320;}
else if(slot == 2) {slot_pos = 240;}
else if(slot == 3) {slot_pos = 160;}



if(state == 0 && m.zMoveComplete()) {m.setPlatterPosition(slot_pos); state = 1;}
else if(state == 1 && m.platterComplete()) {m.setZSpeed(3000); m.setZHeight(3000); m.setDrillSpeed(18); state = 2;}
else if(state == 2 && m.zMoveComplete()){m.setDrillSpeed(-23); state = 3;}
else if(state == 3 && m.zMoveComplete()){delay(6000);state = 4;}
else if(state == 4 && m.zMoveComplete()) {m.setZSpeed(1000); m.setZHeight(0); m.setDrillSpeed(0);state = 5;}
else if(state == 5 && m.zMoveComplete()) {m.setPlatterPosition(80); state = 6;}
else if(state == 6 && m.platterComplete()) {m.setZSpeed(2000); m.setZHeight(11200); state = 7;}
else if(state == 7 && m.zMoveComplete()){m.setDrillSpeed(30); state = 8;}
else if(state == 8 && m.zMoveComplete()){delay(4000); state = 9;}
else if(state == 9 && m.zMoveComplete()) {m.setZSpeed(1000); m.setZHeight(0); m.setDrillSpeed(0); state = 10;}
else if(state == 10 && m.zMoveComplete()) {m.setPlatterPosition(slot_pos); state = 11;}
else if(state == 11 && m.platterComplete()) {m.setZSpeed(3000); m.setZHeight(3000); state = 12;}
else if(state == 12 && m.zMoveComplete()) {m.setDrillSpeed(23); state = 13;}
else if(state == 13 && m.zMoveComplete()){delay(6000);state = 14;}
else if(state == 14 && m.zMoveComplete()) {m.setZSpeed(1000); m.setZHeight(0); m.setDrillSpeed(0); state = 15;}




// if(state == 0 && m.zMoveComplete()){m.setPlatterPosition(320);state = 1;}
// else if(state == 1 && m.zMoveComplete()){delay(1000);state = 2;}
// else if(state == 2 && m.zMoveComplete()){m.setPlatterPosition(160);state = 3;}

// m.setPlatterDirection(true);
//  if(state == 2 && m.zMoveComplete()){m.setDrillSpeed(0);state = 2;}
m.updateAsync(); //steps stepper motor once
//   Serial.println(m.getBatteryVoltage());
//   delay(500);
 
}
