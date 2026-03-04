const int hallPin = 2;    // 49E Sensor
const int buzzerPin = 9;  // Buzzer
const int ledPin = 18;    // LED

int baseline = 0;         // Neutral magnetic field value
int threshold = 100;      // Minimum movement to trigger (deadzone)

void setup() {
  Serial.begin(115200);
  
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  
  // Calibration: Find the neutral point of your sensor
  // The 49E rests at roughly half voltage (approx 2048 on ESP32)
  long total = 0;
  for(int i=0; i<20; i++) {
    total += analogRead(hallPin);
    delay(20);
  }
  baseline = total / 20;
  
  Serial.print("Baseline established at: ");
  Serial.println(baseline);
}

void loop() {
  // Read the Hall Sensor (0 - 4095)
  int currentVal = analogRead(hallPin);

  // Calculate how far the magnet is from the sensor
  // abs() ensures it works for both North and South poles
  int strength = abs(currentVal - baseline);

  // If the magnet is close enough to exceed our threshold
  if (strength > threshold) {
    
    // 1. LED BRIGHTNESS (Simple PWM)
    // Map strength (from threshold to ~1500) to PWM range (0 to 255)
    int brightness = map(strength, threshold, 1000, 0, 255);
    brightness = constrain(brightness, 0, 255);
    analogWrite(ledPin, brightness); 

    // 2. BUZZER PITCH
    // Map strength to a frequency range (300Hz to 3000Hz)
    int pitch = map(strength, threshold, 1000, 300, 3000);
    pitch = constrain(pitch, 300, 3000);
    tone(buzzerPin, pitch);

    // Debugging
    Serial.print("Strength: ");
    Serial.print(strength);
    Serial.print(" | Brightness: ");
    Serial.println(brightness);
  } 
  else {
    // Turn off if magnet is far away
    analogWrite(ledPin, 0);
    noTone(buzzerPin);
  }

  delay(15); // Short delay for stability
}