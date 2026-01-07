import sys
import serial
from PyQt6.QtWidgets import QApplication, QWidget, QPushButton, QHBoxLayout, QGridLayout, QVBoxLayout, QTextEdit, QSizePolicy, QSpinBox
from PyQt6.QtCore import QThread, pyqtSignal
import time

# Change this to your Pico’s port
# PORT = "COM3"    # Windows: COMx
PORT = "/dev/ttyACM0"   # Linux/Mac
BAUDRATE = 115200

# ser = serial.Serial(PORT, 115200, timeout=1)
class SerialReader(QThread):
    new_line = pyqtSignal(str)

    def __init__(self, port, baudrate=115200):
        super().__init__()
        self.ser = serial.Serial(port, baudrate, timeout=1)
        self.running = True

    def run(self):
        while self.running:
            try:
                line = self.ser.readline().decode(errors="ignore").strip()
                if line:
                    self.new_line.emit(line)
            except Exception as e:
                self.new_line.emit(f"Error: {e}")
                break
            time.sleep(0.1)
        
    def stop(self):
        self.running = False
        self.ser.close()


class PicoWindow(QWidget):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("Pico Serial UI")

        # Layout
        clock_layout = QHBoxLayout()

        # Left: Output Box
        self.output_box = QTextEdit()
        self.output_box.setReadOnly(True)
        clock_layout.addWidget(self.output_box)

        # Right: Controls
        controls_widget = QWidget()
        controls_layout = QVBoxLayout()

        # Reboot to BOOTSEL Button
        reboot_button = QPushButton("Reboot to BOOTSEL")
        reboot_button.clicked.connect(self.reboot_to_bootsel)

        # RTC Buttons
        rtc_layout = QWidget()
        rtc_read_button = QPushButton(f"RTC Read")
        rtc_read_button.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)  # Make button fill the grid cell
        rtc_read_button.clicked.connect(self.rtc_read_button_pressed)

        rtc_set_button = QPushButton(f"RTC Set")
        rtc_set_button.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)  # Make button fill the grid cell
        rtc_set_button.clicked.connect(self.rtc_set_button_pressed)

        rtc_layout.setLayout(QHBoxLayout())
        rtc_layout.layout().addWidget(rtc_read_button)
        rtc_layout.layout().addWidget(rtc_set_button)

        # Motor Buttons
        motor_controls_layout = QWidget()
        motor_inputs_layout = QVBoxLayout(motor_controls_layout)
        motor_ids = [
            "HOUR",
            "MINUTE",
            "DAYTENTH",
            "DAYONES",
            "MONTH",
        ]
        # self.motor_buttons = {} # Dictionary to store button references

        for motor_id in motor_ids:
            motor_row_widget = QWidget()
            motor_row_layout = QHBoxLayout(motor_row_widget)
            motor_row_layout.setContentsMargins(0,0,0,0)

            # Button
            button = QPushButton(f"SPIN Motor {motor_id}")
            button.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)

            # Number Input
            spinbox = QSpinBox()
            spinbox.setRange(-10000, 10000)
            spinbox.setValue(0)  # Default value

            button.clicked.connect(lambda checked, motor_id=motor_id, spin=spinbox: self.motor_button_pressed(motor_id,spin.value()))

            motor_row_layout.addWidget(button, stretch=3)
            motor_row_layout.addWidget(spinbox, stretch=1)

            motor_inputs_layout.addWidget(motor_row_widget)
            # self.motor_buttons[motor_id] = button  # Store button reference

        # LED Buttons
        led_controls_layout = QWidget()
        led_buttons_grid = QGridLayout(led_controls_layout)
       
        led_buttons = [
            ("1A", 1, 1),
            ("2A", 1, 2),
            ("1B", 2, 1),
            ("2B", 2, 2),
            ("1C", 3, 1),
            ("2C", 3, 2),
        ]
        self.led_buttons = {} # Dictionary to store button references

        for led_id, row, col in led_buttons:
            button = QPushButton(f"LED {led_id}\nOFF")
            button.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)  # Make button fill the grid cell
            button.clicked.connect(lambda checked, led_id=led_id: self.led_button_pressed(led_id))
            led_buttons_grid.addWidget(button, row, col)
            self.led_buttons[led_id] = button  # Store reference
        # for i in range(1, 4):  # Assuming 3 rows
        #     led_buttons_grid.setRowStretch(i, 1)
        # for j in range(1, 3):  # Assuming 2 columns
        #     led_buttons_grid.setColumnStretch(j, 1)

        controls_layout.addWidget(reboot_button)
        controls_layout.addWidget(rtc_layout)
        controls_layout.addWidget(motor_controls_layout)
        controls_layout.addWidget(led_controls_layout)
        controls_widget.setLayout(controls_layout)

        clock_layout.addWidget(controls_widget)
        self.setLayout(clock_layout)

        # This thread reads serial data FROM RPi Pico
        self.serial_thread = SerialReader(PORT, BAUDRATE)
        self.serial_thread.new_line.connect(self.append_output)
        self.serial_thread.start()

    def rtc_read_button_pressed(self):
        try:
            self.serial_thread.ser.write(f"[RTC] READ : NOW\n".encode())
            self.serial_thread.ser.flush()
            self.append_output(">>> Reading RTC time...")
        except Exception as e:
            self.append_output(f"Error communicating with Pico: {e}")
    def rtc_set_button_pressed(self):
        try:
            self.serial_thread.ser.write(f"[RTC] SET : NOW\n".encode())
            self.serial_thread.ser.flush()
            self.append_output(">>> Setting RTC time...")
        except Exception as e:
            self.append_output(f"Error communicating with Pico: {e}")

    def motor_button_pressed(self, motor_id, spin):
        try:
            # button = self.motor_buttons.get(motor_id)
            # if not button:
            #     print(f"No button found for MOTOR {motor_id}")
            #     self.append_output(f"Error: No button found for MOTOR {motor_id}")
            #     return
            try:
                self.serial_thread.ser.write(f"[MOTOR] {motor_id} : {spin}\n".encode())
                self.serial_thread.ser.flush()
                self.append_output(f">>> Spinning MOTOR {motor_id} for {spin} steps.")
            except Exception as e:
                self.append_output(f"Error communicating with Pico: {e}")
                return

        except Exception as e:
            self.append_output(f"Error: {e}")

    def led_button_pressed(self, led_id):
        try:
            button = self.led_buttons.get(led_id)
            if not button:
                print(f"No button found for LED {led_id}")
                self.append_output(f"Error: No button found for LED {led_id}")
                return
            
            # User pressed - LED now becomes OFF
            if "ON" in button.text():
                try:
                    self.serial_thread.ser.write(f"[LED] {led_id} : OFF\n".encode())
                    self.serial_thread.ser.flush()
                    self.append_output(f">>> Switching LED {led_id} OFF")
                except Exception as e:
                    self.append_output(f"Error communicating with Pico: {e}")
                    return
                button.setStyleSheet("") # Now LED OFF so background back to default
                button.setText(f"LED {led_id}\nOFF") # User can switch it ON next

            # User pressed - LED now becomes ON
            elif "OFF" in button.text():
                try:
                    self.serial_thread.ser.write(f"[LED] {led_id} : ON\n".encode())
                    self.serial_thread.ser.flush()
                    self.append_output(f">>> Switching LED {led_id} ON")
                except Exception as e:
                    self.append_output(f"Error communicating with Pico: {e}")
                    return
                button.setStyleSheet("background-color: green;") # Now LED ON so background green
                button.setText(f"LED {led_id}\nON") # User can switch it OFF next

        except Exception as e:
            self.append_output(f"Error: {e}")

    def reboot_to_bootsel(self):
        try:
            self.serial_thread.ser.write(f"[POWER] PICO : REBOOT\n".encode())
            self.serial_thread.ser.flush()
            self.append_output(">>> Rebooting to BOOTSEL mode...")
        except Exception as e:
            self.append_output(f"Error communicating with Pico: {e}")

    # Callback to update output box
    def append_output(self, text):
        if text.startswith(">>>"):
            user_input = f'<span style="color: orange;">{text}</span>'
            self.output_box.append(user_input)
        else:
            self.output_box.append(text)

    def closeEvent(self, event):
        self.serial_thread.stop()
        event.accept()


if __name__ == "__main__":

    app = QApplication(sys.argv)
    window = PicoWindow()
    window.resize(500,400)
    window.show()
    sys.exit(app.exec())