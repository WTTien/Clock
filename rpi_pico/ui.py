import sys
import serial
from PyQt6.QtWidgets import QApplication, QWidget, QPushButton, QLabel, QVBoxLayout, QTextEdit
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
        layout = QVBoxLayout()

        # Button
        self.button = QPushButton("Send to Pico")
        self.button.clicked.connect(self.send_to_pico)
        layout.addWidget(self.button)

        self.output_box = QTextEdit()
        self.output_box.setReadOnly(True)
        layout.addWidget(self.output_box)

        # # Label for output
        # self.label = QLabel("Waiting...")
        # layout.addWidget(self.label)

        self.setLayout(layout)

        self.serial_thread = SerialReader(PORT, BAUDRATE)
        self.serial_thread.new_line.connect(self.append_output)
        self.serial_thread.start()


    def send_to_pico(self):
        try:
            # Send a test command
            self.serial_thread.ser.write(b"PING\n")
            self.serial_thread.ser.flush()
            self.append_output(">> Sent: PING")
        except Exception as e:
            self.append_output(f"Error sending to Pico: {e}")

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