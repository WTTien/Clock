import smbus
import time
import argparse

bus = smbus.SMBus(1)

DEVICE_ADDRESS = 0x20

def write_pcf8575(state1, state2):
	try:
		bus.write_i2c_block_data(DEVICE_ADDRESS, state1, [state2])
	except OSError as e:
		print(f"I2C Write Error: {e}")


def setup():
	global step_sequence
	step_sequence = [0b1001, 0b1000, 0b1100, 0b0100, 0b0110, 0b0010, 0b0011, 0b0001]
	
	global moved_step
	moved_step = 0
	
	global minute
	minute = 0
    
def spinMotor():
	global step_sequence
	global moved_step
	global minute
	
	moved_step_lag = moved_step < (4096/60)*minute
	
	if moved_step_lag:
		count = 69
	else:
		count = 68	

	step = 0
	
	for i in range(count):
		state1 = 0b00000000
		state2 = 0b00000000

		state1 |= (step_sequence[step] << 4)
		state2 |= (step_sequence[step])

		
		state1 |= 0x05

		write_pcf8575(state1, state2)
				
		time.sleep(0.005)

		step = (step + 1) % 8
		
	
	moved_step = moved_step + count


if __name__ == "__main__":
	
	setup()
	spinMotor(args.direction, args.state1, args.state2)
	write_pcf8575(0x05, 0x00)
