
import smbus
import time

bus = smbus.SMBus(1)

DEVICE_ADDRESS = 0x20

def write_pcf8575(number):
	try:		
		bus.write_i2c_block_data(DEVICE_ADDRESS, number, [0x00])
		print("Written")
	except OSError as e:
		print(f"I2C Write Error: {e}")
		
def read_pcf8575(file):
    """
    Read the states of all 16 GPIO pins on the PCF8575.
    :return: Tuple of two bytes (state1, state2) representing the states of pins P0-P7 and P8-P15.
    """
    try:
        # Read two bytes from the PCF8575 (register 0x00)
        data = bus.read_i2c_block_data(DEVICE_ADDRESS, 0x00, 2)
        
        # Extract states of pins
        state1 = data[0]  # P0-P7
        state2 = data[1]  # P8-P15
        
        print(f"Pin states: {state1:08b} {state2:08b}",file=file)
        return state1, state2
    except OSError as e:
        print(f"I2C Read Error: {e}")

if __name__ == "__main__":
	write_pcf8575(0x05)
	

