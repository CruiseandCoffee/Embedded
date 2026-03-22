/*
 * TCS3200 Color Sensor with Calibration for ESP32-S3
 * 1. Open Serial Monitor at 115200 baud.
 * 2. Follow the on-screen prompts to calibrate White and Black.
 */

// Pin Definitions (Matching previous wiring)
#define S0 5
#define S1 6
#define S2 39
#define S3 38
#define sensorOut 40

// Calibration Variables
int redMin, redMax;
int greenMin, greenMax;
int blueMin, blueMax;

void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  // Set frequency scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  Serial.begin(115200);
  delay(2000); // Give user time to open Serial Monitor

  calibrateSensor();
}

void loop() {
  // Read Raw Frequencies
  int rRaw = readColor(LOW, LOW);
  int gRaw = readColor(HIGH, HIGH);
  int bRaw = readColor(LOW, HIGH);

  // Map raw values to 0-255 (using the calibration data)
  // We use constrain to make sure values don't go outside 0-255
  int r = constrain(map(rRaw, redMin, redMax, 255, 0), 0, 255);
  int g = constrain(map(gRaw, greenMin, greenMax, 255, 0), 0, 255);
  int b = constrain(map(bRaw, blueMin, blueMax, 255, 0), 0, 255);

  // Print RGB values to monitor
  Serial.print("R: "); Serial.print(r);
  Serial.print(" G: "); Serial.print(g);
  Serial.print(" B: "); Serial.print(b);
  Serial.print("  |  Color: ");

  // ACCURATE COLOR DETECTION LOGIC
  if (r > 200 && g > 200 && b > 200) {
    Serial.println("WHITE");
  } else if (r < 50 && g < 50 && b < 50) {
    Serial.println("BLACK");
  } else if (r > g && r > b) {
    if (g > 100 && r > 200) Serial.println("YELLOW/ORANGE");
    else Serial.println("RED");
  } else if (g > r && g > b) {
    Serial.println("GREEN");
  } else if (b > r && b > g) {
    Serial.println("BLUE");
  } else {
    Serial.println("DETECTING...");
  }

  delay(500);
}

// Helper function to read a specific color frequency
int readColor(int s2Val, int s3Val) {
  digitalWrite(S2, s2Val);
  digitalWrite(S3, s3Val);
  delay(20); // Small delay for sensor stability
  return pulseIn(sensorOut, LOW);
}

// CALIBRATION SEQUENCE
void calibrateSensor() {
  Serial.println("--- CALIBRATION START ---");
  
  // 1. Calibrate White
  Serial.println("1. Place a WHITE object in front of the sensor.");
  Serial.println("Type 'y' and press Enter when ready...");
  while (Serial.read() != 'y'); 
  redMin = readColor(LOW, LOW);
  greenMin = readColor(HIGH, HIGH);
  blueMin = readColor(LOW, HIGH);
  Serial.println("White captured!");
  delay(1000);

  // 2. Calibrate Black
  Serial.println("2. Place a BLACK object (or nothing) in front of the sensor.");
  Serial.println("Type 'y' and press Enter when ready...");
  while (Serial.read() != 'y');
  redMax = readColor(LOW, LOW);
  greenMax = readColor(HIGH, HIGH);
  blueMax = readColor(LOW, HIGH);
  Serial.println("Black captured!");
  
  Serial.println("--- CALIBRATION COMPLETE ---");
  delay(2000);
}