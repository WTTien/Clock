import sys
import serial
from PyQt6.QtWidgets import QApplication, QWidget, QPushButton, QLabel, QHBoxLayout, QGridLayout, QVBoxLayout, QTextEdit
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

        led_controls_layout = QWidget()
        led_buttons_grid = QGridLayout(led_controls_layout)

        # Buttons
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
            button = QPushButton(f"LED {led_id} - OFF")
            button.clicked.connect(lambda checked, led_id=led_id: self.led_button_pressed(led_id))
            led_buttons_grid.addWidget(button, row, col)
            self.led_buttons[led_id] = button  # Store reference
            
        # self.led_1A_button = QPushButton("Switch LED 1A ON")
        # self.led_1A_button.clicked.connect(self.led_1A_button_pressed)
        # led_buttons_grid.addWidget(self.led_1A_button, 1, 1)

        # self.led_2A_button = QPushButton("Switch LED 2A ON")
        # self.led_2A_button.clicked.connect(self.led_2A_button_pressed)
        # led_buttons_grid.addWidget(self.led_2A_button, 1, 2)

        # self.led_1B_button = QPushButton("Switch LED 1B ON")
        # self.led_1B_button.clicked.connect(self.led_1B_button_pressed)
        # led_buttons_grid.addWidget(self.led_1B_button, 2, 1)

        # self.led_2B_button = QPushButton("Switch LED 2B ON")
        # self.led_2B_button.clicked.connect(self.led_2B_button_pressed)
        # led_buttons_grid.addWidget(self.led_2B_button, 2, 2)

        # self.led_1C_button = QPushButton("Switch LED 1C ON")
        # self.led_1C_button.clicked.connect(self.led_1C_button_pressed)
        # led_buttons_grid.addWidget(self.led_1C_button, 3, 1)
        
        # self.led_2C_button = QPushButton("Switch LED 2C ON")
        # self.led_2C_button.clicked.connect(self.led_2C_button_pressed)
        # led_buttons_grid.addWidget(self.led_2C_button, 3, 2)

        controls_layout.addWidget(led_controls_layout)
        controls_widget.setLayout(controls_layout)

        clock_layout.addWidget(controls_widget)
        self.setLayout(clock_layout)

        # This thread reads serial data FROM RPi Pico
        self.serial_thread = SerialReader(PORT, BAUDRATE)
        self.serial_thread.new_line.connect(self.append_output)
        self.serial_thread.start()

    def led_button_pressed(self, led_id):
        try:
            # Send a test command
            self.append_output(f">> Switching LED {led_id}")
            self.serial_thread.ser.write(f"[LED] {led_id} : ON\n".encode())
            self.serial_thread.ser.flush()
        except Exception as e:
            self.append_output(f"Error sending to Pico: {e}")

    # def led_1A_button_pressed(self):
    #     try:
    #         # Send a test command
    #         self.append_output(">> Switching LED 1A")
    #         self.serial_thread.ser.write(b"[LED] 1A : ON\n")
    #         self.serial_thread.ser.flush()
    #     except Exception as e:
    #         self.append_output(f"Error sending to Pico: {e}")

    # def led_1B_button_pressed(self):
    #     try:
    #         # Send a test command
    #         self.append_output(">> Switching LED 1B")
    #         self.serial_thread.ser.write(b"[LED] 1B : ON\n")
    #         self.serial_thread.ser.flush()
    #     except Exception as e:
    #         self.append_output(f"Error sending to Pico: {e}")

    # def led_2A_button_pressed(self):
    #     try:
    #         # Send a test command
    #         self.append_output(">> Switching LED 2A")
    #         self.serial_thread.ser.write(b"[LED] 2A : ON\n")
    #         self.serial_thread.ser.flush()
    #     except Exception as e:
    #         self.append_output(f"Error sending to Pico: {e}")

    # def led_2B_button_pressed(self):
    #     try:
    #         # Send a test command
    #         self.append_output(">> Switching LED 2B")
    #         self.serial_thread.ser.write(b"[LED] 2B : ON\n")
    #         self.serial_thread.ser.flush()
    #     except Exception as e:
    #         self.append_output(f"Error sending to Pico: {e}")

    # def led_1C_button_pressed(self):
    #     try:
    #         # Send a test command
    #         self.append_output(">> Switching LED 1C")
    #         self.serial_thread.ser.write(b"[LED] 1C : ON\n")
    #         self.serial_thread.ser.flush()
    #     except Exception as e:
    #         self.append_output(f"Error sending to Pico: {e}")

    # def led_2C_button_pressed(self):
    #     try:
    #         # Send a test command
    #         self.append_output(">> Switching LED 2C")
    #         self.serial_thread.ser.write(b"[LED] 2C : ON\n")
    #         self.serial_thread.ser.flush()
    #     except Exception as e:
    #         self.append_output(f"Error sending to Pico: {e}")

    # def send_to_pico(self):
    #     try:
    #         # Send a test command
    #         self.serial_thread.ser.write(b"PING\n")
    #         self.serial_thread.ser.flush()
    #         self.append_output(">> Sent: PING")
    #     except Exception as e:
    #         self.append_output(f"Error sending to Pico: {e}")

    # Callback to update output box
    def append_output(self, text):
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