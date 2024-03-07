import keyboard
import time

def spinMotor():
    print("Spin motor")

def main():
    running = True
    while running:
        if keyboard.is_pressed('esc'):
            running = False

        else:
            if keyboard.is_pressed('down'):
                spinMotor()

            elif keyboard.is_pressed('up'):
                spinMotor()


if __name__ == "__main__":
    main()
            