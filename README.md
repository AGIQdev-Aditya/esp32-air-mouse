# 🖱️ ESP32 Wireless Bluetooth Air Mouse (v2.0 - Anti-Drift)

An embedded IoT project turning an **ESP32 DevKit V1** and an **MPU-6050 6-Axis Gyroscope/Accelerometer** into a high-precision, wireless Bluetooth Human Interface Device (HID) Air Mouse.

**Author:** [Aditya Sharma](https://github.com/AGIQdev-Aditya)  
**Hardware:** ESP32 DevKit V1 (30-pin) + MPU-6050 (GY-521) + Push Button + Blue LED indicator  
**Language / Environment:** C++ / Arduino IDE / Linux (Arch)

---

## ✨ Features & Engineering Highlights

* **📡 Wireless Bluetooth HID:** Directly emulates a standard Bluetooth mouse — connects to Linux, Windows, macOS, Android, and iOS with zero custom drivers or dongles needed.
* **🎯 Anti-Drift Signal Processing:**
  * **Dynamic Soft Deadzone:** Eliminates sensor resting noise and hand tremors without sticky cursor snapping.
  * **Exponential Moving Average (EMA) Filter:** Smooths micro-tremors for silky, fluid cursor glide.
  * **Sub-Pixel Motion Accumulator:** Preserves fractional movement between cycles so slow aiming never drops frames.
* **⚡ Ultra-Low Latency & High Refresh Rate:**
  * **400kHz Fast I2C:** High-speed sensor register polling cuts bus transaction latency by 75%.
  * **Hardware DLPF (Digital Low Pass Filter):** Configured MPU-6050 internal low-pass filter (`CONFIG` register `0x1A = 0x02` @ 98Hz) to eliminate high-frequency motor and electrical noise at the silicon level.
  * **~180–200Hz Polling Rate:** Provides fluid cursor tracking on high-refresh-rate displays.
* **🔄 On-the-Fly Recalibration:**
  * Holding the primary button (GPIO 15) for **>2 seconds** while resting flat triggers an instant 300-sample gyroscope zero-point recalibration.
  * Built-in Blue LED (`GPIO 2`) illuminates during calibration and turns off when zeroed out.

---

## 🔌 Circuit Pinout & Hardware Schematic

No external resistors needed — utilizes ESP32's internal software pull-up resistors (`INPUT_PULLUP`).

### 1. MPU-6050 Sensor (GY-521) to ESP32
| MPU-6050 Pin | ESP32 Pin | Function |
| :--- | :--- | :--- |
| **VCC** | **3V3** (3.3V) | Power Supply |
| **GND** | **GND** | System Ground |
| **SCL** | **D22** (GPIO 22) | I2C Clock Line |
| **SDA** | **D21** (GPIO 21) | I2C Data Line |
| *AD0 / INT / XDA / XCL* | *NC* | Leave Unconnected |

### 2. Tactile Push Button & Indicator
| Component | Connection | Function |
| :--- | :--- | :--- |
| **Button Leg 1** | **D15** (GPIO 15) | Left Click (Short press) / Calibrate (Hold >2s) |
| **Button Leg 2** | **GND** | Ground Reference |
| **Built-in LED** | **GPIO 2** | Visual indicator during sensor calibration |

---

## 💻 Firmware Installation

1. Add the ESP32 board manager URL in Arduino IDE (`File -> Preferences`):
   ```text
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
2. Install **`esp32 by Espressif Systems`** via the Boards Manager.
3. Install the patched **`ESP32-BLE-Mouse`** library (compatible with ESP32 Core 3.x).
4. Open [`Air_Mouse/Air_Mouse.ino`](Air_Mouse/Air_Mouse.ino):
   * Select Board: **`DOIT ESP32 DEVKIT V1`** (or `ESP32 Dev Module`).
   * Select Port: **`/dev/ttyUSB0`** (or COM port on Windows).
5. Click **Upload (`→`)**.

---

## 🎮 How to Calibrate & Fly

1. **Pairing:** Open Bluetooth settings on your device and pair with **`Aditya Air Mouse`**.
2. **Re-calibration:** Whenever temperature drift occurs:
   * Place the board flat and motionless on a desk.
   * Press and hold the button (**D15**) for 2 seconds until the blue LED turns on.
   * Keep still until the LED turns off. Drift is instantly reset to zero!
3. **Usage:**
   * Tilt your wrist to glide the cursor across your screen.
   * Single-click for standard Left Click.

---

## 📄 License
Open source under the MIT License.
