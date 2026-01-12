import sys
import serial
from PyQt6.QtCore import Qt
from PyQt6.QtWidgets import \
    QApplication, \
    QWidget, \
    QPushButton, \
    QHBoxLayout, \
    QGridLayout, \
    QVBoxLayout, \
    QTextEdit, \
    QSizePolicy, \
    QSpinBox, \
    QLabel, \
    QFrame, \
    QCheckBox, \
    QLineEdit
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

        # Debug Button
        self.debug_button = QPushButton("Debug Mode OFF")
        self.debug_button.clicked.connect(self.toggle_debug_mode)

        # WiFi Layout
        wifi_section = QWidget()
        wifi_section.setLayout(QHBoxLayout())

        wifi_check_button = QPushButton("Check WiFi status")
        wifi_check_button.clicked.connect(self.wifi_check_button_pressed)
        wifi_check_button.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)

        wifi_disconnect_button = QPushButton("Disconnect WiFi")
        wifi_disconnect_button.clicked.connect(self.wifi_disconnect_button_pressed)
        wifi_disconnect_button.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)

        wifi_connect_button = QPushButton("Connect WiFi")
        wifi_connect_button.clicked.connect(self.wifi_connect_button_pressed)
        wifi_connect_button.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)

        wifi_input_details_section = QWidget()
        wifi_input_details_section.setLayout(QVBoxLayout())

        self.wifi_ssid_input = QLineEdit()
        self.wifi_ssid_input.setPlaceholderText("SSID")
        
        self.wifi_password_input = QLineEdit()
        self.wifi_password_input.setPlaceholderText("Password")
        self.wifi_password_input.setEchoMode(QLineEdit.EchoMode.Password)

        wifi_input_details_section.layout().addWidget(self.wifi_ssid_input)
        wifi_input_details_section.layout().addWidget(self.wifi_password_input)        

        wifi_section.layout().addWidget(wifi_check_button)
        wifi_section.layout().addWidget(wifi_disconnect_button)
        wifi_section.layout().addWidget(wifi_connect_button)
        wifi_section.layout().addWidget(wifi_input_details_section)


        # RTC Layouts
        rtc_layout = QWidget()

        # RTC - Read section
        rtc_read_button = QPushButton(f"RTC Read")
        rtc_read_button.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)  # Make button fill the grid cell
        rtc_read_button.clicked.connect(self.rtc_read_button_pressed)
        # RTC - Read section

        # RTC - Set section
        rtc_set_section = QFrame()
        rtc_set_section.setFrameShape(QFrame.Shape.StyledPanel)
        rtc_set_section.setFrameShadow(QFrame.Shadow.Raised)
        rtc_set_section.setLayout(QVBoxLayout())

            # RTC - Set section - Unlock Checkbox
        rtc_set_unlock_checkbox = QCheckBox("Enable RTC Set")
        rtc_set_unlock_checkbox.stateChanged.connect(self.toggle_rtc_set_inputs)
            # RTC - Set section - Unlock Checkbox

            # RTC - Set section - Setting instruction inputs        
        rtc_set_inputs_section = QHBoxLayout()
                # RTC - Set section - Setting instruction inputs - Manual
        rtc_manual_set_section = QWidget()
        rtc_manual_set_layout = QVBoxLayout()
        rtc_manual_set_section.setLayout(rtc_manual_set_layout)

        rtc_manual_set_input_grid_layout = QGridLayout()
        
        self.rtc_manual_set_inputs = {}
        rtc_manual_set_inputs_fields = [
            ("Hour", 0, 23),
            ("Minute", 0, 59),
            ("Second", 0, 59),
            ("Day", 0, 6),     # If using weekday 0=Sunday
            ("Date", 1, 31),
            ("Month", 1, 12),
            ("Year", 2000, 3000)
        ]

        for i, (label_text, min_val, max_val) in enumerate(rtc_manual_set_inputs_fields):
            label = QLabel(label_text)
            spinbox = QSpinBox()
            spinbox.setRange(min_val, max_val)
            spinbox.setMaximumWidth(70)
            spinbox.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)
            self.rtc_manual_set_inputs[label_text] = spinbox
            rtc_manual_set_input_grid_layout.addWidget(label, i, 0)
            rtc_manual_set_input_grid_layout.addWidget(spinbox, i, 1)

        rtc_manual_set_layout.addLayout(rtc_manual_set_input_grid_layout)

        self.rtc_manual_set_button = QPushButton("Manual Set RTC")
        self.rtc_manual_set_button.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)
        self.rtc_manual_set_button.clicked.connect(self.rtc_set_button_manual_pressed)

        rtc_manual_set_layout.addWidget(self.rtc_manual_set_button)
                # RTC - Set section - Setting instruction inputs - Manual
                # RTC - Set section - Setting instruction inputs - NTP Sync
        self.rtc_set_ntp_button = QPushButton(f"NTP Time RTC Sync Set")
        self.rtc_set_ntp_button.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)  # Make button fill the grid cell
        self.rtc_set_ntp_button.clicked.connect(self.rtc_set_button_ntp_pressed)
                # RTC - Set section - Setting instruction inputs - NTP Sync
        rtc_set_inputs_section.addWidget(rtc_manual_set_section)
        rtc_set_inputs_section.addWidget(self.rtc_set_ntp_button)
            # RTC - Set section - Setting instruction inputs

        rtc_set_section.layout().addWidget(rtc_set_unlock_checkbox)
        rtc_set_section.layout().addLayout(rtc_set_inputs_section)

        # Make set disabled at the start
        rtc_set_unlock_checkbox.setChecked(False)
        self.toggle_rtc_set_inputs(Qt.CheckState.Unchecked)
        # RTC - Set section

        # RTC - System Time section
        rtc_system_time_section = QWidget()
        rtc_system_time_section.setLayout(QVBoxLayout())

        rtc_check_system_time_button = QPushButton(f"Check System Time")
        rtc_check_system_time_button.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)
        rtc_check_system_time_button.clicked.connect(self.rtc_check_system_time_button_pressed)

        rtc_sync_system_time_button = QPushButton(f"Sync System Time via NTP")
        rtc_sync_system_time_button.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)
        rtc_sync_system_time_button.clicked.connect(self.rtc_sync_system_time_button_pressed)

        rtc_system_time_section.layout().addWidget(rtc_check_system_time_button)
        rtc_system_time_section.layout().addWidget(rtc_sync_system_time_button)
        # RTC - System Time section

        rtc_layout.setLayout(QHBoxLayout())
        rtc_layout.layout().addWidget(rtc_read_button)
        rtc_layout.layout().addWidget(rtc_set_section)
        rtc_layout.layout().addWidget(rtc_system_time_section)

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
        controls_layout.addWidget(self.debug_button)
        controls_layout.addWidget(wifi_section)
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


    def wifi_check_button_pressed(self):
        try:
            self.serial_thread.ser.write(f"[WIFI] STATUS : NOW\n".encode())
            self.serial_thread.ser.flush()
            self.append_output(">>> Checking WiFi status...")
        except Exception as e:
            self.append_output(f"Error communicating with Pico: {e}")

    def wifi_disconnect_button_pressed(self):
        try:
            self.serial_thread.ser.write(f"[WIFI] DISCONNECT : NOW\n".encode())
            self.serial_thread.ser.flush()
            self.append_output(">>> Disconnecting WiFi...")
        except Exception as e:
            self.append_output(f"Error communicating with Pico: {e}")

    def wifi_connect_button_pressed(self):
        ssid = self.wifi_ssid_input.text().strip()
        password = self.wifi_password_input.text().strip()

        if not ssid:
            connect_details = "DEFAULT"
        else:
            connect_details = f"{ssid}_{password}"
        try:
            self.serial_thread.ser.write(f"[WIFI] CONNECT : {connect_details}\n".encode())
            self.serial_thread.ser.flush()
            self.append_output(">>> Connecting to WiFi...")
        except Exception as e:
            self.append_output(f"Error communicating with Pico: {e}")

    def rtc_read_button_pressed(self):
        try:
            self.serial_thread.ser.write(f"[RTC] READ : NOW\n".encode())
            self.serial_thread.ser.flush()
            self.append_output(">>> Reading RTC time...")
        except Exception as e:
            self.append_output(f"Error communicating with Pico: {e}")

    def toggle_rtc_set_inputs(self, state):
        enabled = (state == Qt.CheckState.Checked.value)
        for spinbox in self.rtc_manual_set_inputs.values():
            spinbox.setEnabled(enabled)
        self.rtc_manual_set_button.setEnabled(enabled)
        self.rtc_set_ntp_button.setEnabled(enabled)

    def rtc_set_button_manual_pressed(self):
        second = self.rtc_manual_set_inputs["Second"].value()
        minute = self.rtc_manual_set_inputs["Minute"].value()
        hour = self.rtc_manual_set_inputs["Hour"].value()
        day = self.rtc_manual_set_inputs["Day"].value() # weekday, 0=Sunday
        date = self.rtc_manual_set_inputs["Date"].value()
        month = self.rtc_manual_set_inputs["Month"].value()
        year = self.rtc_manual_set_inputs["Year"].value()

        try:
            manual_time_to_send = f"{hour:02d}-{minute:02d}-{second:02d}_{day}_{date:02d}-{month:02d}-{year}"
            self.serial_thread.ser.write(f"[RTC] SET_MANUAL : {manual_time_to_send}\n".encode())
            self.serial_thread.ser.flush()
            self.append_output(">>> Setting RTC time via manual user input...")
        except Exception as e:
            self.append_output(f"Error communicating with Pico: {e}")

    def rtc_set_button_ntp_pressed(self):
        try:
            self.serial_thread.ser.write(f"[RTC] SET_NTP : NOW\n".encode())
            self.serial_thread.ser.flush()
            self.append_output(">>> Setting RTC time via NTP...")
        except Exception as e:
            self.append_output(f"Error communicating with Pico: {e}")

    def rtc_check_system_time_button_pressed(self):
        try:
            self.serial_thread.ser.write(f"[RTC] CHECK_SYS_TIME : NOW\n".encode())
            self.serial_thread.ser.flush()
            self.append_output(">>> Checking Pico system time synced with NTP...")
        except Exception as e:
            self.append_output(f"Error communicating with Pico: {e}")

    def rtc_sync_system_time_button_pressed(self):
        try:
            self.serial_thread.ser.write(f"[RTC] SYNC_SYS_TIME : NOW\n".encode())
            self.serial_thread.ser.flush()
            self.append_output(">>> Syncing Pico system time with NTP...")
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

    def toggle_debug_mode(self):
        try:
            # User pressed - Now exit Debug mode
            if "ON" in self.debug_button.text():
                try:
                    self.serial_thread.ser.write(f"[DEBUG] MODE : OFF\n".encode())
                    self.serial_thread.ser.flush()
                    self.append_output(f">>> Quitting Debug Mode...")
                except Exception as e:
                    self.append_output(f"Error communicating with Pico: {e}")
                    return
                self.debug_button.setStyleSheet("") # Now Debug OFF so background back to default
                self.debug_button.setText(f"Debug Mode OFF") # User can switch it ON next

            # User pressed - Now enter Debug mode
            elif "OFF" in self.debug_button.text():
                try:
                    self.serial_thread.ser.write(f"[DEBUG] MODE : ON\n".encode())
                    self.serial_thread.ser.flush()
                    self.append_output(f">>> Entering Debug Mode...")
                except Exception as e:
                    self.append_output(f"Error communicating with Pico: {e}")
                    return
                self.debug_button.setStyleSheet("background-color: yellow;") # Now Debug ON so background yellow
                self.debug_button.setText(f"Debug Mode ON") # User can switch it OFF next

        except Exception as e:
            self.append_output(f"Error: {e}")

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