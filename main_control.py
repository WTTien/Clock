from gpiozero import *
import keyboard
import time
import argparse

def setup(in1, in2, in3, in4):
    global step_sequence
    step_sequence = [[1,0,0,1],
                     [1,0,0,0],
                     [1,1,0,0],
                     [0,1,0,0],
                     [0,1,1,0],
                     [0,0,1,0],
                     [0,0,1,1],
                     [0,0,0,1]]

    motorIn1 = DigitalOutputDevice(in1, active_high = True, initial_value = False)
    motorIn2 = DigitalOutputDevice(in2, active_high = True, initial_value = False)
    motorIn3 = DigitalOutputDevice(in3, active_high = True, initial_value = False)
    motorIn4 = DigitalOutputDevice(in4, active_high = True, initial_value = False)

    global motor_pins
    motor_pins = [motorIn1, motorIn2, motorIn3, motorIn4]

    global step
    step = 0



def spinMotor(direction, grp):
    global step
    
    if grp == 1 or grp == 3:
        count = 1600
    elif grp == 2:
        count = 420
        
    for i in range(count):
        for pin in range(0, len(motor_pins)):
            selected_pin = motor_pins[pin]
            if(step_sequence[step][pin] == 1):
                selected_pin.on()
      #          print("ON")
            elif(step_sequence[step][pin] == 0):
                selected_pin.off()
     #           print("OFF")
            time.sleep(0.002)
            
        if direction == 1:
            step = (step + 1) % 8
        elif direction == 0:
            step = (step - 1) % 8

    #    print(f"Step: {step}")

    


def main():
    setup()
    running = True
    while running:
        
        key = keyboard.read_event().name
        
        if key == 'esc':
            running = False

        else:
            if key == 'down':
                print('down')
                spinMotor(1)

            elif key == 'up':
                print('up')
                spinMotor(0)
	
        time.sleep(0.1)

if __name__ == "__main__":
    
    #main()
    parser = argparse.ArgumentParser()
    parser.add_argument("direction", type=int)
    parser.add_argument("type", type=int)
	
    args = parser.parse_args()

    if args.type == 1:
        setup(4,17,27,22)
    
    if args.type == 2:
        setup(6,13,19,26)
    
    if args.type == 3:
        setup(18,23,24,25)
    
    
    spinMotor(args.direction, args.type)
    
