#include "get_set.h"
#include <boost/asio.hpp>
#include <iostream>
#include <string>

using namespace boost::asio;

int main() {
    io_service io;
    serial_port serial(io,"/dev/cu.usbserial-0001");
    serial.set_option(serial_port_base::baud_rate(9600));

    // Use a string stream to accumulate characters
    std::string data;
    char c;
    boost::system::error_code error;

    LimitSwitches limitSwitches; // create an instance of the class

    while (true) {
        // Read a single character
        // & ampersand is to pass variable into another
        boost::asio::read(serial, boost::asio::buffer(&c, 1), error);

        if (error) {
            std::cerr << "Error while reading: " << error.message() << std::endl;
            break; // Exit if there's an error
        }

        // Check for the end of a message
        if (c == '\n') {
            // Process the accumulated message
            std::cout << "Data read from serial: " << data << std::endl;
            limitSwitches.updateFromSerialData(data);
            std::cout << "The current state is " << limitSwitches.getLimitSwitch() << std::endl;
            
            // Clear the data for the next message
            data.clear();
        } else {
            // Accumulate the data
            data.push_back(c);
        }
    }

    return 0;
}
