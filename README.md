# 🖱️ DIY ESP32 Bluetooth Air Mouse

A hands-on hardware project where I connected an **ESP32 DevKit V1** and an **MPU-6050 6-Axis Gyroscope/Accelerometer** to create a wireless Bluetooth air mouse.

* **Hardware & Assembly:** [Aditya Sharma](https://github.com/AGIQdev-Aditya)  
* **Components:** ESP32 DevKit V1 + MPU-6050 (GY-521) + Tactile Push Button + Connecting Wires  
* **Firmware:** Developed using Arduino IDE with AI assistance and open-source ESP32 BLE libraries  

---

## 💡 How It Works

1. **Motion Input:** The MPU-6050 detects hand and wrist movement (pitch and roll).
2. **Bluetooth Mouse Emulation:** The ESP32 acts as a standard Bluetooth Low Energy (BLE) mouse, connecting directly to any PC or laptop without extra dongles.
3. **Drift Calibration:** Holding the push button while resting the device on a desk recalibrates the sensor zero-point if the cursor starts drifting.

---

## 🔌 Circuit Pinout

| Component Pin | ESP32 Pin | Note |
| :--- | :--- | :--- |
| **MPU-6050 VCC** | **3V3** | 3.3V power |
| **MPU-6050 GND** | **GND** | System ground |
| **MPU-6050 SCL** | **GPIO 22** | I2C Clock |
| **MPU-6050 SDA** | **GPIO 21** | I2C Data |
| **Push Button (Leg 1)** | **GPIO 15** | Left Click / Hold for Calibrate |
| **Push Button (Leg 2)** | **GND** | Ground reference |

---

## 🚀 How to Flash & Run

1. Open [`Air_Mouse/Air_Mouse.ino`](Air_Mouse/Air_Mouse.ino) in the Arduino IDE.
2. Ensure the ESP32 board package and `ESP32-BLE-Mouse` library are installed.
3. Select your board (**DOIT ESP32 DEVKIT V1**) and upload the sketch.
4. Turn on Bluetooth on your computer and connect to **Aditya Air Mouse**.

---

## 📄 License
MIT License
