# ESP32 Motion Monitoring System (MPU6050 + OLED)

Embedded motion sensing system built using **ESP32 (ESP-IDF)** with direct register-level driver implementation.

---

## 🚀 Features
- ESP-IDF based firmware
- Custom I2C driver
- SH1106 OLED driver (No Arduino libraries)
- MPU6050 accelerometer interface
- Real-time motion visualization
- FreeRTOS task-based design

---

## 🧠 Skills Demonstrated
- Embedded C
- I2C Protocol
- Sensor Driver Development
- Hardware Debugging
- Firmware Architecture
- ESP32 Development

---

## 🛠 Hardware Used
- ESP32 DevKit V1
- MPU6050 IMU Sensor
- 1.3" SH1106 OLED Display

---

## 🔌 Circuit Diagram
![Setup](images/setup.jpg)

---

## 🎥 Working Demo
(Video inside `/videos` folder)

---

## 📊 Output
Displays real-time XYZ acceleration data on OLED screen.

---

## ⚙️ Build & Flash

```bash
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
