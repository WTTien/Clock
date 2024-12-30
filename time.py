import smbus
import time
from datetime import datetime

bus = smbus.SMBus(1)
DEVICE_ADDRESS = 0x20

step_sequence = [0b1001, 0b1000, 0b1100, 0b0100, 0b0110, 0b0010, 0b0011, 0b0001]


def setup():	
	global minuteHandStepped
	minuteHandStepped = 0
	
	global hourHandStepped
	hourHandStepped = 0


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


def move_hands_to(minute, hour):
	global minuteHandStepped
	global hourHandStepped

	# This is comparing the past minute
	if minute == 0:
		minuteAdjust = 59 # 60 - 1
	else:
		minuteAdjust = minute - 1
	minuteHandLag = minuteHandStepped < ((8192/60) * minuteAdjust)

	if minuteHandLag :
		minuteOutput = 137
	else:
		minuteOutput = 136

	# This is comparing the past minute
	if (hour % 12) == 0 and minute == 0:
		hourAdjust = 719 # 0 + 60*12 - 1
	else:
		hourAdjust = minute + (60*(hour % 12)) - 1
	hourHandLag = hourHandStepped < ((4096/720) * hourAdjust)

	if hourHandLag :
		hourOutput = 6
	else:
		hourOutput = 5

	spin_motor(minuteOutput, hourOutput)
	write_pcf8575(0x05, 0x00)

	minuteHandStepped = minuteHandStepped + minuteOutput
	if minuteHandStepped == 8192:
		minuteHandStepped = 0
	
	hourHandStepped = hourHandStepped + hourOutput
	if hourHandStepped == 4096:
		hourHandStepped = 0

def check_time():

	prev_minute = datetime.now().minute
	
	while True:
		curr_minute = datetime.now().minute
		curr_hour = datetime.now().hour
		
		if curr_minute != prev_minute:
			move_hands_to(curr_minute, curr_hour)
			prev_minute = curr_minute

		time.sleep(1)



if __name__ == "__main__":
	setup()
	check_time()
	
