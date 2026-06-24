#define BUZZER_PIN 4

// Note frequencies (Hz)
#define C4  262
#define D4  294
#define E4  330
#define F4  349
#define G4  392
#define A4  440
#define Bb4 466
#define C5  523

// Melody notes
int melody[] = {
  C4, C4, D4, C4, F4, E4,
  C4, C4, D4, C4, G4, F4,
  C4, C4, C5, A4, F4, E4, D4,
  Bb4, Bb4, A4, F4, G4, F4
};

// Note durations
int durations[] = {
  4, 4, 2, 2, 2, 1,
  4, 4, 2, 2, 2, 1,
  4, 4, 2, 2, 2, 2, 1,
  4, 4, 2, 2, 2, 1
};

void setup() {
}

void loop() {
  int notes = sizeof(melody) / sizeof(melody[0]);

  for (int i = 0; i < notes; i++) {
    int noteDuration = 1000 / durations[i];

    tone(BUZZER_PIN, melody[i], noteDuration);

    // Pause between notes
    int pauseBetweenNotes = noteDuration * 1.3;
    delay(pauseBetweenNotes);

    noTone(BUZZER_PIN);
  }

  delay(3000); // Wait before repeating
}
