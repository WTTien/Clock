import gpiozero
import keyboard
import time


in1 = 2
in2 = 3
in3 = 4
in4 = 17

def setup():
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



def spinMotor(input):
    for pin in range(0, len(motor_pins)):
        selected_pin = motor_pins[pin]
        if(step_sequence[step][pin] == 1):
            selected_pin.on()
        elif(step_sequence[step][pin] == 0):
            selected_pin.off()

        if input == 1:
            step = (step + 1) % 8
        elif input == 0:
            step = (step - 1) % 8

        time.sleep(0.002)


def main():
    setup()
    running = True
    while running:
        if keyboard.is_pressed('esc'):
            running = False

        else:
            if keyboard.is_pressed('down'):
                spinMotor(1)

            elif keyboard.is_pressed('up'):
                spinMotor(0)


if __name__ == "__main__":
    main()
            