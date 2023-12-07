
#ifndef MUNCHER_CPP
	#define MUNCHER_CPP

	#include "MuNCHER.h"

	#ifndef ESP_A
		NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> underglow(LED_UNDERGLOW_CT, LED_UNDERGLOW_PIN);
		NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> b(LED_2_CT, LED_2_PIN);
	#endif

	TaskHandle_t SerialTask;
	s_data serialData;

	MuncherIO::MuncherIO(){}

	/////////////////////////////////////////////////////////////////////////////
	////////////////////////////////      INIT      /////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	void MuncherIO::init() { 
		#ifdef ESP_A
		//Z HEIGHT INIT
		pinMode(Z_STEP_LEFT, OUTPUT);
		pinMode(Z_DIR_LEFT, OUTPUT);
		pinMode(Z_STEP_RIGHT, OUTPUT);
		pinMode(Z_DIR_RIGHT, OUTPUT);
		digitalWrite(Z_STEP_LEFT, LOW);
		digitalWrite(Z_DIR_LEFT, LOW);
		digitalWrite(Z_STEP_RIGHT, LOW);
		digitalWrite(Z_DIR_RIGHT, LOW);

		//TURRET INIT
		pinMode(TURRET_STEP, OUTPUT);
		pinMode(TURRET_DIR, OUTPUT);
		digitalWrite(TURRET_STEP, LOW);
		digitalWrite(TURRET_DIR, LOW);

		//PLATTER INIT
		pinMode(PLATTER_STEP, OUTPUT);
		pinMode(PLATTER_DIR, OUTPUT);
		digitalWrite(PLATTER_STEP, LOW);
		digitalWrite(PLATTER_DIR, LOW);

		//Drill Init
		pinMode(DRILL_DIR, OUTPUT);
		pinMode(DRILL_PWM, OUTPUT);

		//Init Servo Timers
		// ESP32PWM::allocateTimer(0);
		// ESP32PWM::allocateTimer(1);
		// ESP32PWM::allocateTimer(2);
		// ESP32PWM::allocateTimer(3);

		//Init Servos
		// servo_1.setPeriodHertz(50);
		// servo_2.setPeriodHertz(50);
		// servo_3.setPeriodHertz(50);

		//Attach Servos to Pins with default timings (1000-2000uS)
		// servo_1.attach(SERVO_1, 500, 2400);
		// servo_2.attach(SERVO_2, 500, 2400);
		// servo_3.attach(SERVO_3, 500, 2400);

		//Set pins for switches
		pinMode(SW_P_PIN, INPUT_PULLUP);
		pinMode(SW_Z_PIN, INPUT_PULLUP);
		pinMode(SW_T_PIN, INPUT_PULLUP);

		//Set  voltage read pin
		pinMode(VOLTAGE_PIN, INPUT);

		#else
		// attach the channel to the GPIO to be controlled
		pinMode(FRONT_LEFT_PWM, OUTPUT);
		pinMode(FRONT_RIGHT_PWM, OUTPUT);
		pinMode(BACK_LEFT_PWM, OUTPUT);
		pinMode(BACK_RIGHT_PWM, OUTPUT);

		//Set motor direction to forwards
		analogWrite(FRONT_LEFT_PWM, 0);
		analogWrite(FRONT_RIGHT_PWM, 0);
		analogWrite(BACK_LEFT_PWM, 0);
		analogWrite(BACK_RIGHT_PWM, 0);

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

		//Setup LEDs
		underglow.Begin();
		underglow.Show();

		#endif

		//Create Serial Core
		xTaskCreatePinnedToCore(
				serialThread, /* Function to implement the task */
				"SerialData", /* Name of the task */
				10000,  /* Stack size in words */
				(void*)&serialData,  /* Task input parameter */
				0,  /* Priority of the task */
				&SerialTask,  /* Task handle. */
				0); /* Core where the task should run */
	}

	#ifdef ESP_A

		void MuncherIO::initSteppers() {
			// calibrateZ();
			// calibratePlatter();
			calibrateTurret();
		}

		/////////////////////////////////////////////////////////////////////////////
		////////////////////////      Z Height Steppers      ////////////////////////
		/////////////////////////////////////////////////////////////////////////////
		
		//NOTE: USE THESE TOP 3 WHENEVER POSSIBLE

		//Called once at startup to calibrate the zero position (BLOCKING METHOD)
		void MuncherIO::calibrateZ() {
			//Raising
			setZDirection(DOWN);
			while(!getHeightLimit()) {
				digitalWrite(Z_STEP_LEFT,HIGH);
				digitalWrite(Z_STEP_RIGHT,HIGH);
				delayMicroseconds(1000);
				digitalWrite(Z_STEP_LEFT,LOW);
				digitalWrite(Z_STEP_RIGHT,LOW);
				delayMicroseconds(1000);

			}
			//Back off
			setZDirection(UP);
			for(int i = 0; i < 100; i++) {
				digitalWrite(Z_STEP_LEFT,HIGH);
				digitalWrite(Z_STEP_RIGHT,HIGH);
				delayMicroseconds(1000);
				digitalWrite(Z_STEP_LEFT,LOW);
				digitalWrite(Z_STEP_RIGHT,LOW);
				delayMicroseconds(1000);
			}
			_heightZ = 0;
		}

		//Set target position (between 0 (top of structure) and ~12k)
		void MuncherIO::setZHeight(short height) {
			if(height >= 0) {
				_heightZ = min(height, (short)MAX_HEIGHT);
			}
			else {
				_heightZ = 0;
			}
		}

		//True if movement is complete (safe to move to next step)
		bool MuncherIO::zMoveComplete() {
			return _heightZ == _currentHeightZ;
		}

		///////////////////////////////////////////////////////

		unsigned short MuncherIO::getZTarget() {
			return _heightZ;
		}

		unsigned short MuncherIO::getZHeight() {
			return _currentHeightZ;
		}

		void MuncherIO::setZSpeed(unsigned short speed) {
			_speedZ = speed;
		}

		unsigned short MuncherIO::getZSpeed() {
			return _speedZ;
		}

		bool MuncherIO::getHeightLimit() {
			return !digitalRead(SW_Z_PIN);
		}

		//Unexposed Methods
		void MuncherIO::setZDirection(bool direction) {
			_directionZ = direction;
			digitalWrite(Z_DIR_LEFT, _directionZ);
			digitalWrite(Z_DIR_RIGHT, _directionZ);
		}



		/////////////////////////////////////////////////////////////////////////////
		//////////////////////////////      Drill      //////////////////////////////
		/////////////////////////////////////////////////////////////////////////////
		//Positive value for forward, negative for backwards
		void MuncherIO::setDrillSpeed(short speed) {
			_drillSpeed = speed;

			//Update drill direction
			if(_drillSpeed < 0) {digitalWrite(DRILL_DIR, LOW);Serial.println("LOW");}
			else {digitalWrite(DRILL_DIR, HIGH);Serial.println("HIGH");}

			analogWrite(DRILL_PWM, abs(_drillSpeed));
		}
		short MuncherIO::getDrillSpeed() {
			return _drillSpeed;
		}



		/////////////////////////////////////////////////////////////////////////////
		/////////////////////////////      Platter      /////////////////////////////
		/////////////////////////////////////////////////////////////////////////////
			void MuncherIO::calibratePlatter() {
				//Raising
				setPlatterDirection(COUNTER_CLOCKWISE);
				while(!getPlatterLimit()) {
					digitalWrite(PLATTER_STEP,HIGH);
					delayMicroseconds(12000);
					digitalWrite(PLATTER_STEP,LOW);
					delayMicroseconds(12000);
				}

				for(int i = 0; i < 100; i++) {
					digitalWrite(PLATTER_STEP,HIGH);
					delayMicroseconds(12000);
					digitalWrite(PLATTER_STEP,LOW);
					delayMicroseconds(12000);
				}
				_positionPlatter = 0;
			}

			void MuncherIO::setPlatterPosition(short position) {
				_positionPlatter = position;
			}

			//True if movement is complete (safe to move to next step)
			bool MuncherIO::platterMoveComplete() {
				return _positionPlatter == _currentPositionPlatter;
			}


		bool MuncherIO::getPlatterLimit() {
			return !digitalRead(SW_P_PIN);
		}


		unsigned short MuncherIO::getPlatterPosition() {
			return _currentPositionPlatter;
		}

		unsigned short MuncherIO::getPlatterTarget() {
			return _positionPlatter;
		}

		void MuncherIO::setPlatterDirection(bool direction) {
			_directionPlatter = direction;
			digitalWrite(PLATTER_DIR, _directionPlatter);
		}

		/////////////////////////////////////////////////////////////////////////////
		//////////////////////////////      Turret      /////////////////////////////
		/////////////////////////////////////////////////////////////////////////////
		void MuncherIO::calibrateTurret() {
			//Raising
			setTurretDirection(CLOCKWISE);
			while(!getTurretLimit()) {
				digitalWrite(TURRET_STEP,HIGH);
				delayMicroseconds(10000);
				digitalWrite(TURRET_STEP,LOW);
				delayMicroseconds(10000);
			}
			_positionTurret = 0;
		}

		void MuncherIO::setTurretPosition(short position) {
			_positionTurret = position;
		}

		//True if movement is complete (safe to move to next step)
		bool MuncherIO::turretMoveComplete() {
			return _positionTurret == _currentPositionTurret;
		}





		bool MuncherIO::getTurretLimit() {
			return !digitalRead(SW_T_PIN);
		}

		unsigned short MuncherIO::getTurretPosition() {
			return _currentPositionTurret;
		}

		unsigned short MuncherIO::getTurretTarget() {
			return _positionTurret;
		}

		void MuncherIO::setTurretDirection(bool direction) {
			_directionTurret = direction;
			digitalWrite(TURRET_DIR, _directionTurret);
		}



		/////////////////////////////////////////////////////////////////////////////
		///////////////////////////      Servo Motors     ///////////////////////////
		/////////////////////////////////////////////////////////////////////////////
		void MuncherIO::writeServoPosition(uint8_t servo, int position) {
			switch(servo) {
			case 1:
				// servo_1.write(position);
				return;
			case 2:
			// 	servo_2.write(position);
				return;
			case 3:
			// 	servo_2.write(position);
				return;
			default:
				return;
			}
		}

		/////////////////////////////////////////////////////////////////////////////
		/////////////////////////      Update & Helpers     /////////////////////////
		/////////////////////////////////////////////////////////////////////////////
		void MuncherIO::updateAsync() {
			//First, handle steppers

			//Check to see if a change in Z height is needed
			if(_heightZ - _currentHeightZ != 0) {

				//If the stepper delay time has passed, step the servos
				if(esp_timer_get_time() - _zTimeLastStep > _speedZ){
					
					//Set Z direction
					setZDirection(_heightZ - _currentHeightZ > 0);

			   		_zTimeLastStep = esp_timer_get_time();

			    	//Change polarity of motors at upcoming step
			    	_zStepStatus = _zStepStatus ^ true;
		            // Serial.println(_zStepStatus);
			    	digitalWrite(Z_STEP_LEFT, _zStepStatus);
			    	digitalWrite(Z_STEP_RIGHT, _zStepStatus);
			    	
			    	if(_directionZ) {_currentHeightZ++;}
			    	else {_currentHeightZ--;}
				}
			}

			//Check to see if a change in platter position is needed
			if(_positionPlatter - _currentPositionPlatter != 0) {

				//If the stepper delay time has passed, step the servos
				if(esp_timer_get_time() - _platterTimeLastStep > PLATTER_SPEED){
					//Set platter height 
					setPlatterDirection(_positionPlatter - _currentPositionPlatter > 0);

			   		_platterTimeLastStep = esp_timer_get_time();

			    	//Change polarity of motors at upcoming step
			    	_platterStepStatus = _platterStepStatus ^ true;
			    	digitalWrite(PLATTER_STEP, _platterStepStatus);

			    	if(_directionPlatter) {_currentPositionPlatter++;}
			    	else {_currentPositionPlatter--;}
				}
			}

			//Check to see if a change in turret is needed
			if(_positionTurret - _currentPositionTurret != 0) {

				//If the stepper delay time has passed, step the servos
				if(esp_timer_get_time() - _turretTimeLastStep > TURRET_SPEED){
					//Set turret direction
					setTurretDirection(_positionTurret - _currentPositionTurret > 0);

			   		_turretTimeLastStep = esp_timer_get_time();

			    	//Change polarity of motors at upcoming step
			    	_turretStepStatus = _turretStepStatus ^ true;
			    	digitalWrite(TURRET_STEP, _turretStepStatus);

			    	if(_directionTurret) {_currentPositionTurret++;}
			    	else {_currentPositionTurret--;}
				}
			}
		}

		float MuncherIO::getBatteryVoltage() {
			return (float)analogRead(VOLTAGE_PIN) * 0.00459341 + 0.12;
		}

	#else

		void MuncherIO::setUnderglowSolidColor(uint8_t red, uint8_t green, uint8_t blue) {
			RgbColor color(red, green, blue);

			for(uint8_t ledNum = 0; ledNum < LED_UNDERGLOW_CT; ledNum++) {
				underglow.SetPixelColor(ledNum, color);
			}
			underglow.Show();
		}


		void MuncherIO::setFrontLeftSpeed(short speed) {
			digitalWrite(FRONT_LEFT_DIR, speed < 0);
			analogWrite(FRONT_LEFT_PWM, abs(speed));
		}
		void MuncherIO::setFrontRightSpeed(short speed) {
			digitalWrite(FRONT_RIGHT_DIR, speed < 0);
			analogWrite(FRONT_RIGHT_PWM, abs(speed));
		}
		void MuncherIO::setBackLeftSpeed(short speed) {
			digitalWrite(BACK_LEFT_DIR, speed > 0);
			analogWrite(BACK_LEFT_PWM, abs(speed));
		}
		void MuncherIO::setBackRightSpeed(short speed) {
			digitalWrite(BACK_RIGHT_DIR, speed > 0);
			analogWrite(BACK_RIGHT_PWM, abs(speed));
		}
		

		//Set motor target speeds
		void MuncherIO::diffDrive(short turnAngle, uint8_t speed) {
			short left   = speed / 6 + (((long)speed * 3) * (180 - abs(turnAngle)) / 180 * (turnAngle < 0)) / 6 + (long)speed * 5 / 6 * (turnAngle >= 0); 
	    	short right  = speed / 6 + (((long)speed * 3) * (180 - abs(turnAngle)) / 180 * (turnAngle > 0)) / 6 + (long)speed * 5 / 6 * (turnAngle <= 0);		

			_frontLeftSpeed  = left;
			_backLeftSpeed   = right;
			_frontRightSpeed = left;
			_backRightSpeed  = right;		
		}

		void MuncherIO::updateAsync() {
			//If the motor delay time has passed, update speeds
			if(esp_timer_get_time() - _motorMillisTimer > MOTOR_UPDATE_RATE){

				diffDrive(serialData.turnAngle, serialData.speed);

				//Front left calc
				short frontLeftDiff = _frontLeftSpeed - _currentFrontLeftSpeed;
				_currentFrontLeftSpeed = _currentFrontLeftSpeed + min(abs(frontLeftDiff), MOTOR_RESPONSE_RATE) * (frontLeftDiff > 1) + min(abs(frontLeftDiff), MOTOR_RESPONSE_RATE) * (frontLeftDiff < 1) * -1;
				setFrontLeftSpeed(_currentFrontLeftSpeed);
				
				//Front right calc
				short frontRightDiff = _frontRightSpeed - _currentFrontRightSpeed;
				_currentFrontRightSpeed = _currentFrontRightSpeed + min(abs(frontRightDiff), MOTOR_RESPONSE_RATE) * (frontRightDiff > 1) + min(abs(frontRightDiff), MOTOR_RESPONSE_RATE) * (frontRightDiff < 1) * -1;
				setFrontRightSpeed(_currentFrontRightSpeed);
				
				//Back left calc
				short backLeftDiff = _backLeftSpeed - _currentBackLeftSpeed;
				_currentBackLeftSpeed = _currentBackLeftSpeed + min(abs(backLeftDiff), MOTOR_RESPONSE_RATE) * (backLeftDiff > 1) + min(abs(backLeftDiff), MOTOR_RESPONSE_RATE) * (backLeftDiff < 1) * -1;
				setBackLeftSpeed(_currentBackLeftSpeed);
				
				//Back right calc
				short backRightDiff = _backRightSpeed - _currentBackRightSpeed;
				_currentBackRightSpeed = _currentBackRightSpeed + min(abs(backRightDiff), MOTOR_RESPONSE_RATE) * (backRightDiff > 1) + min(abs(backRightDiff), MOTOR_RESPONSE_RATE) * (backRightDiff < 1) * -1;
				setBackRightSpeed(_currentBackRightSpeed);

				//Update Timer
		   		_motorMillisTimer = esp_timer_get_time();
		   	}
		}
	#endif

	void serialThread(void * pvParameters)
	{
		s_data &serialData = *(s_data*)(pvParameters);

		String inputBuffer = "";
		String incomingCommandParsed = "";

		long lastUpdateTime = 0UL;
		uint8_t commandElementsParsed = 0;

		int outputVal[4]  = {0,0,0,0};

		while(true) {

			//Set values to zero and reset if no input for 200ms
			if(esp_timer_get_time() - lastUpdateTime > SERIAL_TIMEOUT) {
				serialData.turnAngle = 0;
				serialData.speed = 0;
				serialData.sSAction = 0;
				serialData.sSAbort = false;

				commandElementsParsed = 0;

				inputBuffer = "";
				incomingCommandParsed = "";

				for(uint8_t iterator = 0; iterator < 4; iterator++) {
					outputVal[iterator] = 0;
				}
			}
			

			//If there is a new serial command (or part of one)
			while(Serial.available() > 0) {
				inputBuffer = inputBuffer + String(char(Serial.read()));
			}

			//There is a whole and valid serial command in the buffer 
			if(inputBuffer.charAt(0) == 'a' && inputBuffer.charAt(inputBuffer.length()-2) == 'a') {
				
				//First, remove validation chars
				incomingCommandParsed = inputBuffer.substring(1, inputBuffer.length()-2);

				//Start at the beginning of the string
				commandElementsParsed = 0;

				//While there are command elements left to parse, continue to parse them
				while(incomingCommandParsed.indexOf(',') > 0 && commandElementsParsed < 4) {

					//Set valid int values to variables 
					outputVal[commandElementsParsed] = int(min(max(incomingCommandParsed.substring(0, incomingCommandParsed.indexOf(',')).toInt(),-255L), 255L));
					incomingCommandParsed = incomingCommandParsed.substring(incomingCommandParsed.indexOf(',') + 1, incomingCommandParsed.length());
					commandElementsParsed++;
				}

				if(commandElementsParsed == 4) {
					serialData.turnAngle = outputVal[0];
					serialData.speed = outputVal[1];
					serialData.sSAction = (uint8_t)outputVal[2];
					serialData.sSAbort = (bool)outputVal[3];

					lastUpdateTime = esp_timer_get_time();
					inputBuffer = "";
				}
			}
		}
	}



#endif
