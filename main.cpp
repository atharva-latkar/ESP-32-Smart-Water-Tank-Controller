#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- PIN DEFINITIONS ---
#define TRIG_PIN 5
#define ECHO_PIN 18
#define RELAY_PIN 19   
#define BUZZER_PIN 23  
#define BUTTON_PIN 4

// LCD address
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- CONSTANTS ---
const float TANK_DEPTH_CM = 100.0; 
const float ON_THRESHOLD = 15.0;   
const float OFF_THRESHOLD = 90.0;  

// State Machine
enum PumpState { PUMP_OFF, PUMP_ON };
PumpState currentPumpState = PUMP_OFF;

// --- FUNCTION PROTOTYPES ---
float readDistance();
float calculateLevel(float distance);
void updatePumpState(float level);
void updateDisplay(float level);
void handleAlerts(float level);

// Timer variables
unsigned long lastUpdate = 0;
const long interval = 500; // 500ms interval

void setup() {
  Serial.begin(115200);
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Init...");
  
  digitalWrite(RELAY_PIN, LOW); 
}

void loop() {
  // NON-BLOCKING TIMER using millis()
  unsigned long currentMillis = millis();
  
  if (currentMillis - lastUpdate >= interval) {
    lastUpdate = currentMillis;

    // 1. Read sensors
    float distance = readDistance();
    float level = calculateLevel(distance);
    
    // 2. Update State
    updatePumpState(level);
    
    // 3. Update UI
    updateDisplay(level);
    handleAlerts(level);
    
    // 4. Serial Debug
    Serial.print("Level: ");
    Serial.print(level);
    Serial.print(" % | Pump: ");
    Serial.println(currentPumpState == PUMP_ON ? "ON" : "OFF");
  }
}

float readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1.0;
  
  return (duration * 0.0343) / 2.0;
}

float calculateLevel(float distance) {
  if (distance < 0) return -1.0; 
  
  float level = ((TANK_DEPTH_CM - distance) / TANK_DEPTH_CM) * 100.0;
  
  if (level < 0.0) level = 0.0;
  if (level > 100.0) level = 100.0;
  
  return level;
}

void updatePumpState(float level) {
  if (level < 0) {
    digitalWrite(RELAY_PIN, LOW);
    currentPumpState = PUMP_OFF;
    return;
  }

  switch (currentPumpState) {
    case PUMP_OFF:
      if (level < ON_THRESHOLD) {
        currentPumpState = PUMP_ON;
        digitalWrite(RELAY_PIN, HIGH);
      }
      break;

    case PUMP_ON:
      if (level > OFF_THRESHOLD) {
        currentPumpState = PUMP_OFF;
        digitalWrite(RELAY_PIN, LOW);
      }
      break;
  }
}

void updateDisplay(float level) {
  // If sensor fault, show error
  if (level < 0) {
    lcd.setCursor(0, 0);
    lcd.print("SENSOR FAULT!   ");
    lcd.setCursor(0, 1);
    lcd.print("Pump: OFF       ");
    return;
  }

  // Line 1: Level
  lcd.setCursor(0, 0);
  lcd.print("Lvl: ");
  lcd.print(level, 1); // Print 1 decimal place
  lcd.print("%    ");  // Spaces clear leftover characters

  // Line 2: Pump Status
  lcd.setCursor(0, 1);
  lcd.print("Pump: ");
  if (currentPumpState == PUMP_ON) {
    lcd.print("ON     ");
  } else {
    lcd.print("OFF    ");
  }
}

void handleAlerts(float level) {
  if (level < 0) return; // Ignore if fault

  // Overflow alert (> 95%)
  if (level > 95.0) {
    tone(BUZZER_PIN, 1000); // 1000Hz continuous beep
  } 
  // Dry-run alert (pump is ON but level is < 5%)
  else if (currentPumpState == PUMP_ON && level < 5.0) {
    tone(BUZZER_PIN, 1500); // Higher pitch for dry-run
  } 
  // Normal state
  else {
    noTone(BUZZER_PIN);
  }
}