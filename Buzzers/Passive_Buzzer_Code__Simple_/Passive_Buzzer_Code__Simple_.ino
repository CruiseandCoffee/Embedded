#define BUZZER_PIN 4

void setup() {
}

void loop() {
  tone(BUZZER_PIN, 1000); // 1000 Hz
  delay(500);

  noTone(BUZZER_PIN);
  delay(500);
}
