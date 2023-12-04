def parseKeyboardCommands(command):
    # first goal is to parse the command
    FL = 0
    FR = 0
    BL = 0
    BR = 0 
    z = command.replace(',', '').split() # replace this so it's deleting commas that are in the command coming in
    
    # second goal is to check whether start and end are equal to 'a'
    if z[0] == 'a' and z[-1] == 'a':
        # I want to remove these two so I slice
        z = z[1:-1]

        for i in range(len(z)): # take the value and convert it to an integer
            z[i] = int(z[i])
        
        # after I convert them to an integer, I want to assign them based on w a s d, right
        if z[0] == 1:
            FL = FL + 20
            FR = FR + 50
            BL = BL + 50
            BR = BR + 50

        if z[1] == 1:
            # when we want to turn left, FR, BR are greater than FL, BL
            FL = FL + 50
            FR = FR + 100
            BL = BL + 25
            BR = BR + 100
            
        
        if z[2] == 1:
            
            FL = FL - 100
            FR = FR - 20
            BL = BL - 30
            BR = BR - 20
        
        if z[3] == 1:
            # when we want to turn right FL, BL are greater than FR, BR
            FL = FL + 120
            FR = FR + 30
            BL = BL + 100
            BR = BR + 20

    else:
        print('The command input is not parsable')
    
    return f'a, {FL}, {FR}, {BL}, {BR}, a' # we want to pass these into the Arduino code which you already have

testing = parseKeyboardCommands('a, 1, 1, 1, 1, a')
print(testing)