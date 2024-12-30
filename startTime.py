import smbus
import time
import math
import argparse

bus = smbus.SMBus(1)
DEVICE_ADDRESS = 0x20

step_sequence = [0b1001, 0b1000, 0b1100, 0b0100, 0b0110, 0b0010, 0b0011, 0b0001]


def write_pcf8575(state1, state2):
	try:
		bus.write_i2c_block_data(DEVICE_ADDRESS, state1, [state2])
	except OSError as e:
		print(f"I2C Write Error: {e}")

def spin_motor(minuteStep, hourStep):
	global step_sequence

	step = 0
	
	for i in range(max(minuteStep, hourStep)):
		state1 = 0b00000000
		state2 = 0b00000000

		if (i < minuteStep):
			state1 |= (step_sequence[step] << 4)

		if (i < hourStep):
			state2 |= (step_sequence[step])

		state1 |= 0x05

		write_pcf8575(state1, state2)
				
		time.sleep(0.005)

		step = (step + 1) % 8


def start_at(minute, hour, updown):

  if updown == True:
    minuteOutput = math.ceil((4096/60) * minute)
    hourOutput = math.ceil((4096/720) * (minute + (60*hour)))

  elif updown == False:
    minuteOutput = math.floor((4096/60) * minute)
    hourOutput = math.floor((4096/720) * (minute + (60*hour)))

	spin_motor(minuteOutput, hourOutput)
	write_pcf8575(0x05, 0x00)
	
if __name__ == "__main__":
	
  parser = argparse.ArgumentParser()
	parser.add_argument("minute", type=int)
	parser.add_argument("hour", type=int)
	parser.add_argument("updown", type=bool)
	
	args = parser.parse_args()
  
  start_at(args.minute, args.hour, args.updown)
	
