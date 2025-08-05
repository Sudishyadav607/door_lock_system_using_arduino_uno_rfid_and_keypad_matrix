# door_lock_system_using_arduino_uno_rfid_and_keypad_matrix
Project Description
A secure door lock system using Arduino Uno, RFID (MFRC522), and a 4x4 Matrix Keypad. Authorized users can unlock the door with either an RFID tag or a PIN code.

Features
🔒 Dual Authentication – Unlock via RFID tag or PIN code.
📟 4x4 Keypad – Enter a custom PIN for access.
🛡️ Security Alerts – Buzzer sounds on invalid attempts.
📊 LCD Feedback – Displays access status (granted/denied).
⚙️ Admin Mode – Add/remove RFID tags or change PIN.

Hardware Requirements
Arduino Uno

RFID Module (MFRC522) + Tags

4x4 Matrix Keypad

LCD (16x2 I2C)

Servo Motor (for door latch)

Buzzer (for alerts)

LEDs (Optional: Red for denied, Green for granted)

Setup & Wiring
RFID → SPI pins (SCK=13, MISO=12, MOSI=11, SDA=10, RST=9).

Keypad → Connect rows & cols to Arduino digital pins.

Servo → PWM pin (~9) for door lock mechanism.

LCD (I2C) → SDA (A4), SCL (A5).

How It Works
Scan RFID or Enter PIN → System verifies credentials.

If valid → Servo unlocks door + LCD shows "Access Granted."

If invalid → Buzzer beeps + LCD shows "Access Denied."

Admin Functions
Add New RFID Tag: Enter admin mode → scan new tag → stored in EEPROM.

Change PIN: Enter admin mode → set new 4-digit code.

Future Improvements
📲 Bluetooth/Wi-Fi – Remote unlock via smartphone.
🔐 Fingerprint Sensor – Biometric authentication.
☁ IoT Integration – Log access attempts to cloud.
