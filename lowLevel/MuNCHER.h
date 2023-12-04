#ifndef MUNCHER_H
#define MUNCHER_H

#include "Arduino.h"

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

#define TURRET_STEP         17
#define TURRET_DIR          19

#define Z_STEP_RIGHT        18
#define Z_DIR_RIGHT          5

#define PLATTER_STEP        15
#define PLATTER_DIR          2

//SERVOS
#define SERVO_1             21
#define SERVO_2             22
#define SERVO_3             23

//SWITCHES
#define SW_P                13
#define SW_Z                14

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
#define LED_1               13
#define LED_2               12

#define ESP_A

class MuncherIO
{
	public:
		MuncherIO(void);

		void init();
		
		#ifdef ESP_A

			//Z Height
			void calibrateZ();

			void setHeightZ(unsigned short height);
			unsigned short getHeightZ();

			setSpeedZ(unsigned short height);
			unsigned short getSpeedZ();

			bool getZLimit();

			//Drill 
			setDrillSpeed();
			getDrillSpeed();

			//Platter
			getPlatterPosition();
			setPlatterPosition();


			getPlatterLimit();


		#endif


	private:
		unsigned short _heightZ = 0UL;
		unsigned short _heightZ = 0UL;

};

#endif
