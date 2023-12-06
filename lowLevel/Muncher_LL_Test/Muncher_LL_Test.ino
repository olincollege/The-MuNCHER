#include "MuNCHER.h"
MuncherIO m;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(250000);
  delay(800);
  m.init();
  m.initSteppers();
  
//  m.setHeightZ(500);
//  m.setPlatterPosition(800);
//  m.setTurretPosition(800);


//  delay(1000);
  Serial.print("beginning");
  m.setDrillSpeed(50);
//    delay(2000);
//    m.setDrillSpeed(0);
//    delay(2000);
//      m.setDrillSpeed(-50);
//    delay(2000);
}

int state = 0;

void loop() {
    m.setDrillSpeed(50);
    delay(2000);
    m.setDrillSpeed(0);
    delay(2000);
    
      m.setDrillSpeed(-50);
    delay(2000);
m.setDrillSpeed(0);
    delay(2000);
  // put your main code here, to run repeatedly:
//  if(state == 0 && m.zMoveComplete()) {m.setZHeight(3000);state = 1;}
//  else if(state == 1 && m.zMoveComplete()){m.setZHeight(0);state = 0;}
  
  m.updateAsync();
//   Serial.println(m.getBatteryVoltage());
//   delay(500);
 
}
