#include <Wire.h>
#include <BleMouse.h>

// ==========================================
// 🖱️ ADITYA's ESP32 AIR MOUSE (v2.0 - Anti-Drift)
// Hardware: ESP32 DevKit V1 (30-pin) + MPU-6050 + 1 Button
// ==========================================

// --- Pin Definitions ---
#define BUTTON_LEFT_CLICK 15  // GPIO 15 (D15) -> Button -> GND
#define LED_PIN 2             // Built-in Blue LED on ESP32 DevKit V1
#define MPU_ADDR 0x68         // Default I2C address of MPU-6050

// --- Anti-Drift & Smoothness Tuning ---
const int DEADZONE = 180;        // Soft deadzone (DLPF removes resting noise)
const float SENSITIVITY = 140.0; // Cursor speed (Lower = faster, Higher = slower)
const float SMOOTHING = 0.40;    // Exponential smoothing factor (0.1 = heavy filter, 1.0 = raw)

// --- Bluetooth Mouse Name ---
BleMouse bleMouse("Aditya Air Mouse", "Espressif", 100);

// --- Gyro Offsets ---
long gyroX_offset = 0;
long gyroY_offset = 0;
long gyroZ_offset = 0;

// Helper function to read raw 6-byte gyroscope registers from MPU-6050
void readRawGyro(int16_t &gx, int16_t &gy, int16_t &gz) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x43); // Starting register for GYRO_XOUT_H
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)MPU_ADDR, (size_t)6, true);

  if (Wire.available() >= 6) {
    gx = (Wire.read() << 8) | Wire.read();
    gy = (Wire.read() << 8) | Wire.read();
    gz = (Wire.read() << 8) | Wire.read();
  }
}

// Calibration Routine: Calculates zero-point offsets while resting flat
void calibrateGyro() {
  digitalWrite(LED_PIN, HIGH); // Turn ON LED to indicate calibration in progress
  Serial.println("\n⏳ CALIBRATING... KEEP THE SENSOR FLAT & STILL ON YOUR DESK!");

  long sumX = 0, sumY = 0, sumZ = 0;
  const int SAMPLES = 300;

  for (int i = 0; i < SAMPLES; i++) {
    int16_t gx, gy, gz;
    readRawGyro(gx, gy, gz);
    sumX += gx;
    sumY += gy;
    sumZ += gz;
    delay(4);
  }

  gyroX_offset = sumX / SAMPLES;
  gyroY_offset = sumY / SAMPLES;
  gyroZ_offset = sumZ / SAMPLES;

  digitalWrite(LED_PIN, LOW); // Turn OFF LED when done
  Serial.println("🎯 CALIBRATION COMPLETE! Drift zeroed out.");
}

void setup() {
  Serial.begin(115200);
  delay(500);

  // 1. Configure Pins
  pinMode(BUTTON_LEFT_CLICK, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // 2. Initialize I2C Communication on ESP32 default pins (SDA=21, SCL=22)
  Wire.begin(21, 22);
  Wire.setClock(400000); // 400kHz Fast I2C mode for reduced latency

  // 3. Wake up MPU-6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); // Power Management Register 1
  Wire.write(0x00); // Wake up
  byte error = Wire.endTransmission();

  if (error != 0) {
    Serial.println("❌ ERROR: Could not connect to MPU-6050! Check wiring.");
    while (1) {
      digitalWrite(LED_PIN, HIGH);
      delay(200);
      digitalWrite(LED_PIN, LOW);
      delay(200);
    }
  }

  // Configure Digital Low Pass Filter (DLPF) to 98Hz for hardware noise smoothing
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1A); // CONFIG register
  Wire.write(0x02); // DLPF_CFG = 2 (98Hz bandwidth, eliminates gyro jitter)
  Wire.endTransmission();

  // 4. Initial Calibration on startup
  calibrateGyro();

  // 5. Start Bluetooth Mouse Service
  bleMouse.begin();
  Serial.println("✨ Ready! Connect Bluetooth to 'Aditya Air Mouse'.");
}

void loop() {
  // Check if Bluetooth is connected
  if (bleMouse.isConnected()) {
    int16_t rawGX, rawGY, rawGZ;
    readRawGyro(rawGX, rawGY, rawGZ);

    // Apply calibration offsets
    long gx = rawGX - gyroX_offset;
    long gy = rawGY - gyroY_offset;
    long gz = rawGZ - gyroZ_offset;

    // Movement calculation with soft deadzone (smooth transition, no sudden jump)
    float effGZ = 0.0f;
    if (abs(gz) > DEADZONE) {
      effGZ = (gz > 0) ? (gz - DEADZONE) : (gz + DEADZONE);
    }

    float effGX = 0.0f;
    if (abs(gx) > DEADZONE) {
      effGX = (gx > 0) ? (gx - DEADZONE) : (gx + DEADZONE);
    }

    // Direction mapping:
    // Horizontal (Yaw): -effGZ
    // Vertical (Pitch):  effGX (Inverted from previous -effGX so Up/Down is natural)
    float targetX = -effGZ / SENSITIVITY;
    float targetY =  effGX / SENSITIVITY;

    // Exponential Moving Average filter for silky-smooth motion without lag
    static float smoothX = 0.0f;
    static float smoothY = 0.0f;
    smoothX += SMOOTHING * (targetX - smoothX);
    smoothY += SMOOTHING * (targetY - smoothY);

    // Sub-pixel accumulator preserves fractional pixels so slow movement glides like butter
    static float accumX = 0.0f;
    static float accumY = 0.0f;
    accumX += smoothX;
    accumY += smoothY;

    int moveX = (int)accumX;
    int moveY = (int)accumY;

    accumX -= moveX;
    accumY -= moveY;

    // Move cursor if beyond deadzone
    if (moveX != 0 || moveY != 0) {
      bleMouse.move(moveX, moveY);
    }

    // --- Button & On-the-fly Recalibration Logic ---
    // Short press (< 1.5s) = Left Click
    // Long hold (> 2.0s) = Auto Re-calibrate!
    static unsigned long buttonPressStart = 0;
    static bool buttonWasPressed = false;
    static bool longPressHandled = false;

    if (digitalRead(BUTTON_LEFT_CLICK) == LOW) {
      if (!buttonWasPressed) {
        buttonPressStart = millis();
        buttonWasPressed = true;
        longPressHandled = false;
        bleMouse.press(MOUSE_LEFT); // Immediate click feedback
      } else {
        // If user holds button for 2 seconds while resting flat -> RECALIBRATE!
        if (!longPressHandled && (millis() - buttonPressStart > 2000)) {
          bleMouse.release(MOUSE_LEFT);
          longPressHandled = true;
          calibrateGyro(); // Instant zero-point reset
        }
      }
    } else {
      if (buttonWasPressed) {
        if (!longPressHandled) {
          bleMouse.release(MOUSE_LEFT);
        }
        buttonWasPressed = false;
      }
    }

    delay(5); // ~180-200Hz high-refresh polling rate for butter-smooth movement
  } else {
    delay(100);
  }
}
