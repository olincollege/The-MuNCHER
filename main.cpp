#include <iostream>
#include "get_set.h" // include header file as "import" so I have access to these functions
using namespace std;

int main() {
    DrillControl drill; // instance of class
    unsigned int pwmValue; // store the pwmValue
    // I want to get the current value for drillPWM
    std::cout << "Initial drill PWM is: " << drill.getDrillPWM() << std::endl;

    // then I want to taken in a value and set it
    std::cout << "Enter desired PWM value for the drill: ";
    std::cin >> pwmValue;
    drill.setDrillPWM(pwmValue);

    std::cout << "Drill PWM is now set to: " << drill.getDrillPWM() << std::endl;

    return 0;
}