# LED Control Using OpenCV 🎥

## Overview
This project demonstrates **real-time LED control using computer vision**. The system uses OpenCV to process webcam input, detect specific colors or hand gestures, and sends control commands wirelessly to XIAO microcontroller boards via serial communication or wireless protocols.

## Features
- 👆 **Gesture Recognition**: Hand gestures control LED on/off
- 🎨 **Color Detection**: Specific color triggers LED changes
- 🎥 **Real-time Processing**: Live webcam feed analysis
- 📡 **Wireless Control**: ESP32 XIAO with wireless transmission
- ⚡ **Low Latency**: Instant response to gestures

## Components Used
- **Microcontroller**: 2x Seeed XIAO ESP32 (or XIAO RP2040)
- **LED**: RGB or standard LED with resistor
- **Computer**: Python with OpenCV
- **Communication**: Serial (USB) or wireless (ESP-NOW/WiFi)
- **Camera**: Webcam or USB camera

## Project Files

### 1. LED.py - Computer Vision Script
**Purpose**: Detect gestures/colors and send commands via serial

**Key Features:**
- Opens webcam feed
- Detects hand/color using OpenCV
- Analyzes motion or color changes
- Sends serial commands to microcontroller

**Algorithm:**
```
1. Capture frame from webcam
2. Process frame (resize, blur)
3. Detect hand/color
4. Analyze gesture
5. Send command via serial
6. Display annotated frame
```

### 2. Receiver_XIAO.ino - Receiver Board
**Purpose**: Receives commands and controls LED

**Features:**
- Listens on Serial port
- Parses incoming commands
- Controls LED based on commands
- Provides feedback (optional)

### 3. Transmiteer_XIAO.ino - Transmitter Board (Optional)
**Purpose**: Transmits LED status or sensor data

**Features:**
- Sends status back to PC (optional)
- Can include sensor readings
- Confirms command receipt

## Communication Protocol

### Serial Communication
```
PC (OpenCV) ──────Serial Connection────→ XIAO Board
                  (USB or CH340)

Commands:
- "LED_ON\n"
- "LED_OFF\n"
- "TOGGLE\n"
- "BRIGHTNESS:128\n"
```

### Baud Rate: 115200

## How It Works

### OpenCV Detection Methods

#### Method 1: Color Detection (HSV)
```python
# Detect specific color (e.g., red)
hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
lower_red = np.array([0, 100, 100])
upper_red = np.array([10, 255, 255])
mask = cv2.inRange(hsv, lower_red, upper_red)
```

#### Method 2: Hand Gesture Detection
```python
# Detect hand using contours
gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
contours, _ = cv2.findContours(gray, ...)
# Analyze contour shape for gesture
```

#### Method 3: Face Detection
```python
# Detect face presence
face_cascade = cv2.CascadeClassifier(...)
faces = face_cascade.detectMultiScale(gray)
# Trigger LED based on face count
```

## Pin Configuration

### XIAO ESP32 Receiver
```
LED Pin: GPIO 2
RX Pin: GPIO 0 (Serial RX)
TX Pin: GPIO 1 (Serial TX)
GND: Ground
```

### Circuit
```
XIAO ────── 220Ω ───── LED Anode
             ↓
            LED Cathode ──── GND
```

## Code Explanation

### LED.py Main Structure
```python
import cv2
import serial
import numpy as np

# Initialize serial connection
ser = serial.Serial('COM3', 115200)  # Adjust COM port

cap = cv2.VideoCapture(0)  # Open webcam

while True:
    ret, frame = cap.read()
    if not ret:
        break
    
    # Detect gesture/color
    command = detect_gesture(frame)
    
    # Send command
    if command:
        ser.write(command.encode() + b'\n')
    
    # Display result
    cv2.imshow('Frame', frame)
    
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
```

### Receiver_XIAO.ino Structure
```cpp
#define LED_PIN 2

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    
    if (command == "LED_ON") {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED: ON");
    }
    else if (command == "LED_OFF") {
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED: OFF");
    }
  }
}
```

## Gesture Recognition Methods

### 1. Hand Present Detection
```python
# Detect if hand is in frame
if hand_detected:
    send_command("LED_ON")
else:
    send_command("LED_OFF")
```

### 2. Swipe Gesture
```python
# Left swipe = ON, Right swipe = OFF
if hand_moved_left:
    send_command("LED_ON")
elif hand_moved_right:
    send_command("LED_OFF")
```

### 3. Finger Count
```python
# 1 finger = ON, 2+ fingers = OFF
if fingers == 1:
    send_command("LED_ON")
else:
    send_command("LED_OFF")
```

### 4. Distance-Based
```python
# Hand near camera = ON, far = OFF
if distance < threshold:
    send_command("LED_ON")
else:
    send_command("LED_OFF")
```

## Setup Instructions

### 1. Install Python Dependencies
```bash
pip install opencv-python
pip install pyserial
pip install numpy
```

### 2. Find XIAO COM Port
**Windows:**
- Device Manager → Ports (COM & LPT)
- Look for "USB-SERIAL CH340"

**Linux/Mac:**
- Terminal: `ls /dev/tty*`
- Usually `/dev/ttyUSB0` or `/dev/ttyACM0`

### 3. Update Serial Port in LED.py
```python
ser = serial.Serial('COM3', 115200)  # Change COM3 to your port
```

### 4. Upload Sketches
- Upload `Receiver_XIAO.ino` to Receiver XIAO
- (Optional) Upload `Transmiteer_XIAO.ino` to Transmitter XIAO

### 5. Run Python Script
```bash
python LED.py
```

### 6. Test
- Perform gestures in front of camera
- Watch LED on XIAO board respond
- Check console for debug output

## OpenCV Color Ranges (HSV)

```python
# Common color ranges in HSV
Red:     H: 0-10, S: 100-255, V: 100-255
Green:   H: 35-85, S: 100-255, V: 100-255
Blue:    H: 100-130, S: 100-255, V: 100-255
Yellow:  H: 15-35, S: 100-255, V: 100-255
```

## Debugging

### Check Serial Connection
```python
import serial.tools.list_ports
ports = serial.tools.list_ports.comports()
for port in ports:
    print(port)
```

### Test Communication
```python
ser.write(b'LED_ON\n')
response = ser.readline()
print(response)
```

### View Webcam Feed
```python
cv2.imshow('Webcam', frame)
cv2.waitKey(1)
```

## Performance Optimization

### Frame Rate
- Default: 30 FPS
- Adjust capture size for speed
- Reduce frame size for faster processing

### Processing Power
```python
# Smaller frame = faster processing
frame = cv2.resize(frame, (320, 240))
```

### Latency Reduction
- Minimize delay between detection and command
- Use binary commands (smallest data)
- Higher baud rate (115200)

## Troubleshooting

| Issue | Solution |
|-------|----------|
| Serial port not found | Check USB drivers, list ports |
| LED not responding | Verify baud rate matches |
| Gesture not detected | Adjust lighting, HSV ranges |
| Slow response | Reduce frame size, optimize code |
| Camera not opening | Check camera permissions, try USB port |

## Advanced Features

### 1. Multiple Gestures
```python
# Right hand = ON, Left hand = OFF
if hand_position == 'right':
    send_command("LED_ON")
elif hand_position == 'left':
    send_command("LED_OFF")
```

### 2. Brightness Control
```python
# Distance determines brightness (0-255)
brightness = calculate_distance_value()
send_command(f"BRIGHTNESS:{brightness}")
```

### 3. Color Selection
```python
# Different colors trigger different modes
if red_detected:
    send_command("MODE:RED")
elif blue_detected:
    send_command("MODE:BLUE")
```

### 4. WiFi Control
Replace serial with wireless:
```python
# Use ESP-NOW or WiFi instead
send_via_wifi(command)
```

## Applications
- 👆 Gesture-controlled lighting
- 🎨 Color-responsive displays
- 🎮 Interactive games
- 🎬 Video project control
- 🏠 Smart home integration
- 👨‍💻 Human-computer interaction
- 🎨 Art installations

## Limitations
- Requires good lighting for detection
- Works best with distinct colors/gestures
- Latency depends on processing speed
- USB cable length limits mobility

## Future Enhancements
- 📡 Switch to wireless (WiFi/ESP-NOW)
- 🎯 Multiple LED control
- 📱 Mobile app integration
- 🤖 Machine learning gestures
- 💾 Gesture recording/playback
- 🌐 Cloud integration
- 🎛️ Advanced gesture recognition

## File Structure
```
LED_ON_and_OFF_Using_OPENCV/
├── LED.py                    # Python OpenCV script
├── Receiver_XIAO.ino         # Receiver microcontroller
├── Transmiteer_XIAO.ino      # Optional transmitter
└── README.md
```

## Requirements

**Python Libraries:**
- opencv-python (image processing)
- pyserial (serial communication)
- numpy (numerical operations)

**Hardware:**
- Seeed XIAO ESP32 (or compatible)
- Webcam/USB camera
- LED + resistor
- USB cable

**Software:**
- Python 3.7+
- Arduino IDE
- Drivers for XIAO board

## References
- [OpenCV Documentation](https://docs.opencv.org/)
- [Seeed XIAO Docs](https://wiki.seeedstudio.com/XIAO_ESP32S3/)
- [PySerial Documentation](https://pyserial.readthedocs.io/)

## License
DIY Internship Project - Educational Use Only

---

**Key Concepts**: Computer vision + embedded systems = interactive IoT applications! This project combines high-level image processing with low-level hardware control.
