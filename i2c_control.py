import smbus
import time
import argparse

bus = smbus.SMBus(1)

DEVICE_ADDRESS = 0x20

def write_pcf8575(state1, state2):
	try:
		bus.write_i2c_block_data(DEVICE_ADDRESS, state1, [state2])
		#print(f"Written: {state1:08b} {state2:08b}")
	except OSError as e:
		print(f"I2C Write Error: {e}")


def setup():
	global step_sequence
	step_sequence = [0b1001, 0b1000, 0b1100, 0b0100, 0b0110, 0b0010, 0b0011, 0b0001]
	
	global step
	step = 0    
    
def spinMotor(direction, state1input, state2input):
	
	global step
	global state1

	for i in range(410):
		state1 = 0b00000000
		state2 = 0b00000000
		
		#if state1input == 1:
		#	state1 |= (step_sequence[step])	
		if state1input == 2:
			state1 |= (step_sequence[step] << 4)
			
		if state2input == 1:
			state2 |= (step_sequence[step])
		elif state2input == 2:
			state2 |= (step_sequence[step] << 4)
			
		print(f"State1: {state1:08b}")
		print(f"State2: {state2:08b}")
		
		state1 |= 0x05
		
		if state1input != 0 and state2input != 0:
			write_pcf8575(state1, state2)
		elif state1input != 0 and state2input == 0:
			write_pcf8575(state1, 0x00)
		elif state1input == 0 and state2input != 0:
			write_pcf8575(0x00, state2)
		elif state1input ==  0 and state2input == 0:
			write_pcf8575(0x00, 0x00)
				
		time.sleep(0.005)

		if direction == 1:
			step = (step + 1) % 8
		elif direction == 0:
			step = (step - 1) % 8


if __name__ == "__main__":
	
	parser = argparse.ArgumentParser()
	parser.add_argument("direction", type=int)
	parser.add_argument("state1", type=int)
	parser.add_argument("state2", type=int)
	
	args = parser.parse_args()
	
	setup()
	spinMotor(args.direction, args.state1, args.state2)
	write_pcf8575(0x05, 0x00)
