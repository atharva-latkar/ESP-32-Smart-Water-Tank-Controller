# ESP32 Smart Water Tank Monitoring & Control System

An end-to-end embedded systems project simulating a smart water tank controller. This project was designed to prevent motor burnout and water overflow using a robust, non-blocking state machine architecture.

## 🛠️ Hardware Architecture (Simulated via Wokwi)
*   **Microcontroller:** ESP32 DevKit C V4
*   **Sensor:** HC-SR04 Ultrasonic Sensor (Non-contact water level detection)
*   **User Interface:** 16x2 I2C LCD Display
*   **Actuator:** 5V Relay Module (Simulating water pump control)
*   **Alert System:** Piezoelectric Buzzer
*   **Communication Protocols:** I2C (LCD), GPIO (Sensor/Relay)

## ⚙️ Firmware Features
*   **Hysteresis State Machine:** Implemented a dual-threshold state machine (Pump ON below 15%, Pump OFF above 90%) to prevent relay chatter and motor burnout caused by water level fluctuations.
*   **Non-Blocking Logic:** Utilized `millis()` instead of `delay()` to ensure the sensor, LCD, and buzzer run concurrently without freezing the microcontroller.
*   **Fault Handling (Failsafe):** If the ultrasonic sensor times out (e.g., disconnected or out of range), the system defaults to a safe state, turning the pump OFF and displaying a sensor fault on the LCD.
*   **Audio Alerts:** Integrated buzzer logic for critical conditions—Overflow (>95%) and Dry-Run (Pump ON but level <5%).

## 🧠 Engineering Challenges & Learnings
*   **Debouncing Sensor Noise:** The raw HC-SR04 readings fluctuate. Implementing the state machine prevents these minor fluctuations from rapidly toggling the relay.
*   **Timeout Handling:** Added a 30,000 microsecond timeout to `pulseIn()` to ensure the code doesn't block indefinitely if the sensor fails to receive an echo.

## 📸 Simulation & Validation
<img width="578" height="275" alt="circuit diagram" src="https://github.com/user-attachments/assets/a07e1430-f955-4362-9228-8812e8bb16ca" />

The project was validated in the Wokwi simulator by manually adjusting the distance slider to verify:
1.  Distance-to-percentage math accuracy.
2.  State transitions at exactly 15% and 90%.
3.  Non-blocking LCD updates and buzzer triggers.

## 💻 How to Run
1. Open [Wokwi](https://wokwi.com/).
2. Create a new ESP32 project.
3. Copy the contents of `main.cpp` into the code editor.
4. Copy the contents of `diagram.json` into the diagram editor.
5. Add the `LiquidCrystal I2C` library via the Library Manager.
6. Click Play.
