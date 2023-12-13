#ifndef MUNCHER_H
#define MUNCHER_H

#define ESP_A

/* ESP32_A contains the following IO:
 * - Two steppers for drill Z-height
 * - A stepper for the platter spin
 * - A stepper for the turret 
 * - Three servo motor outputs
 * - A UART GPS
 * - Two limit switches
 * - A PWM motor controller
*/

//STEPPERS
#define Z_STEP_LEFT          4
#define Z_DIR_LEFT          16

#define Z_STEP_RIGHT        17
#define Z_DIR_RIGHT         5

#define MAX_HEIGHT       11900

#define PLATTER_STEP        15
#define PLATTER_DIR          2
#define PLATTER_SPEED    10000

#define TURRET_STEP         18
#define TURRET_DIR          19
#define TURRET_SPEED     10000

//DRILL MOTOR
#define DRILL_PWM           25
#define DRILL_DIR           12

//SERVOS
#define SERVO_1_PIN         21
#define SERVO_1 			 1
#define SERVO_2_PIN         22
#define SERVO_2 			 2
#define SERVO_3_PIN         23
#define SERVO_3 			 3

//GPS
#define RX_PIN              26
#define TX_PIN              27

//SWITCHES
#define SW_P_PIN            13
#define SW_Z_PIN            14
#define SW_T_PIN            33

//VOLTAGE READ
#define VOLTAGE_PIN         32

/* ESP32_B contains the following IO:
 * - Four PWM motor controllers
 * - Two WS2812B LED Drive pins
*/

//DRIVE MOTORS
#define FRONT_LEFT           0
#define FRONT_LEFT_PWM       4
#define FRONT_LEFT_DIR      16

#define FRONT_RIGHT          1
#define FRONT_RIGHT_PWM     17
#define FRONT_RIGHT_DIR     18

#define BACK_LEFT            2
#define BACK_LEFT_PWM       19
#define BACK_LEFT_DIR       21

#define BACK_RIGHT           3
#define BACK_RIGHT_PWM      22
#define BACK_RIGHT_DIR      23

//DIGITAL LED PINS
#define LED_1_PIN           13
#define LED_2_PIN           12

#define UP                   true
#define DOWN                 false

#define CLOCKWISE            true
#define COUNTER_CLOCKWISE    false

#include "Arduino.h"
#include "esp_timer.h"

#ifdef ESP_A
	#include <ESP32Servo.h>
	// Servo servo_1;
	// Servo servo_2;
	// Servo servo_3;
#else
	#include <NeoPixelBus.h>
#endif

class MuncherIO
{
	public:
		MuncherIO(void);

		void init();
		void initSteppers();
		
		#ifdef ESP_A

			/////////////////////////////////////////////////////////////////////////////
			////////////////////////      Z Height Steppers      ////////////////////////
			/////////////////////////////////////////////////////////////////////////////
			
			//Should only need the first three of these:
			void calibrateZ();

			//Set target position (between 0 (top of structure) and ~12k)
			void setZHeight(short height);

			//True if movement is complete (safe to move to next step)
			bool zMoveComplete();

			///////////////////////////////////////////////////////

			//Get the target position (between 0 (top of structure) and ~12k)
			unsigned short getZTarget();

			//Actual height the motor is at
			unsigned short getZHeight();

			//Get/set delay between stepper step pins
			//Should be a number between 800-10000 (default 1000)
			void setZSpeed(unsigned short speed);
			unsigned short getZSpeed();

			//True if height limit switch is pressed
			bool getHeightLimit();

			/////////////////////////////////////////////////////////////////////////////
			//////////////////////////////      Drill      //////////////////////////////
			/////////////////////////////////////////////////////////////////////////////
			//Positive value for forward, negative for backwards
			void setDrillSpeed(short speed);
			short getDrillSpeed();

			/////////////////////////////////////////////////////////////////////////////
			/////////////////////////////      Platter      /////////////////////////////
			/////////////////////////////////////////////////////////////////////////////
			void setPlatterPosition(short position);
			unsigned short getPlatterPosition();

			unsigned short getPlatterTarget();

			void setPlatterPositionSync(short position);

			void calibratePlatter();

			//True if height limit switch is pressed
			bool getPlatterLimit();

			void setZDirection(bool direction);
			void setPlatterDirection(bool direction);
			void setTurretDirection(bool direction);

			bool platterComplete();

			/////////////////////////////////////////////////////////////////////////////
			//////////////////////////////      Turret      /////////////////////////////
			/////////////////////////////////////////////////////////////////////////////
			void setTurretPosition(short position);
			unsigned short getTurretPosition();

			unsigned short getTurretTarget();

			void setTurretPositionSync(short position);

			void calibrateTurret();

			//True if height limit switch is pressed
			bool getTurretLimit();

			/////////////////////////////////////////////////////////////////////////////
			///////////////////////////      Servo Motors     ///////////////////////////
			/////////////////////////////////////////////////////////////////////////////
			void writeServoPosition(uint8_t servo, int position);
		#else

		#endif

			/////////////////////////////////////////////////////////////////////////////
			/////////////////////////      Update & Helpers     /////////////////////////
			/////////////////////////////////////////////////////////////////////////////
			void updateAsync();

			float getBatteryVoltage();


	private:
		#ifdef ESP_A

			short _heightZ = 0;
			short _currentHeightZ = 0;
			unsigned long _speedZ = 1000UL;
			bool _directionZ = false;
				bool _zStepStatus = false;
				unsigned long _zTimeLastStep = 0UL;

			short _drillSpeed = 0;

			short _positionPlatter = 0;
			short _currentPositionPlatter = 0;
			bool _directionPlatter = false;
				bool _platterStepStatus = false;
				unsigned long _platterTimeLastStep = 0UL;

			short _positionTurret = 0;
			short _currentPositionTurret = 0;
			bool _directionTurret = false;
				bool _turretStepStatus = false;
				unsigned long _turretTimeLastStep = 0UL;

			unsigned long _microsecondUpdateTimer = 0UL;
		#else

		#endif

};

#endif