from gpiozero import *
import time


def setup(in1, in2):

    global pinp
    pinp = DigitalOutputDevice(in1, active_high = True, initial_value = True)
    
    global pinn
    pinn = DigitalOutputDevice(in2, active_high = True, initial_value = False)
    
    time.sleep(1)



def turnOn():
    pinp.on()
    pinn.off()
    time.sleep(1)
    
if __name__ == "__main__":
    setup(20, 21)
    turnOn
