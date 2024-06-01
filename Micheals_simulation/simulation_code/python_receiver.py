import serial
import json

# Set the serial port and baud rate
serial_port = 'COM2'
baud_rate = 9600

# Open serial connection
ser = serial.Serial(serial_port, baud_rate)
print("Ready to receive data");

try:
    while True:
        # Read a line from the serial port
        line = ser.readline().decode().strip()

        # Check if the line is not empty
        if line:
            try:
                # Parse the JSON data
                data = json.loads(line)
                
                # Print the JSON data
                print("Received JSON:", data)
            except json.JSONDecodeError:
                print("Error decoding JSON:", line)
except KeyboardInterrupt:
    # Close the serial connection when Ctrl+C is pressed
    ser.close()
    print("Serial connection closed")
