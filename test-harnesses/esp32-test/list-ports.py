import serial.tools.list_ports

print("Listing all COM ports")
# Get a list of all available COM ports
com_ports = serial.tools.list_ports.comports()

# Print the list of COM ports
for port in com_ports:
    print(port.device)
