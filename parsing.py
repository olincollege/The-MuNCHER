import serial
import time
import socket

# Define the IP address and the Port Number
ip      = "192.168.16.99";
port    = 12000;

listeningAddress = (ip, port);

# Create a datagram based server socket that uses IPv4 addressing scheme
datagramSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM);
datagramSocket.bind(listeningAddress);

ser = serial.Serial("/dev/ttyUSB0", 250000, timeout = 0)

systemTime = time.time()
serialLoopPeriod = 0.01; #in seconds, not ms
serialUpdateLoopTimer = 0
driveFL = driveFR = driveBL = driveBR = 0
speed = 100

#Returns incoming serial data as a string (latest string from start character 'a' to
#end char '\n', making sure that the entire sting is there, not just a portion. 
def driveSerialInputs():
	list_strings = ser.readlines()
	if(len(list_strings) > 0):
		print(list_strings)
		goodString = list_strings[-1]
		if(goodString[0] == 97 and goodString[-1] == 10):
			return goodString
	return -1

#Send data from the Jetson to the drive ESP32
def driveSerialOutputs(command):
	ser.write(bytearray(str(command),'ascii'))


# sample string is 'a,1,1,1,1 a' where the sequence is w, a, s, d
def parseKeyboardCommands(command):
    command = str(command)
    # first goal is to parse the command
    FL = 0
    FR = 0
    BL = 0
    BR = 0 
    z = command.replace(',', '').split() # replace this so it's deleting commas that are in the command coming in
    
    # second goal is to check whether start and end are equal to 'a'
    z = z[0]
    if z[0] == 'A' and z[-1] == 'A':
        # I want to remove these two so I slice
        z = z[1:-1]
        
        #for i in range(len(z)): # take the value and convert it to an integer
            #print(z[i])
            #z[i] = int(z[i])
	    


        # after I convert them to an integer, I want to assign them based on w a s d, right
        if z[0] == "1":
            FL = FL + -speed
            FR = FR + -speed
            BL = BL + speed
            BR = BR + speed

        if z[1] == "1":
            # when we want to turn left, FR, BR are greater than FL, BL
            FL = FL + speed
            FR = FR + speed
            BL = BL + speed
            BR = BR + speed


        if z[2] == "1":

            FL = FL + speed
            FR = FR + speed
            BL = BL + -speed
            BR = BR + -speed

        if z[3] == "1":
            # when we want to turn left, FR, BR are greater than FL, BL
            FL = FL + -speed
            FR = FR + -speed
            BL = BL + -speed
            BR = BR + -speed


    else:
        print('The command input is not parsable')

    return f'a{FL},{FR},{BL},{BR},a' # we want to pass these into the Arduino code which you already hav


# Read line   
while True:
	tempVal, sourceAddress = datagramSocket.recvfrom(128);
	tempVal = tempVal.decode('utf-8')
	command = parseKeyboardCommands(tempVal);
	#print(command)
	systemTime = time.time()
	if(serialUpdateLoopTimer + serialLoopPeriod < systemTime):
		#print(driveSerialInputs())
		driveSerialOutputs(command)
		serialUpdateLoopTimer = systemTime
