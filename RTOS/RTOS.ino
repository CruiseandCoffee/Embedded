#include <Arduino.h>

// Pins
#define LED_PIN 3
#define BUTTON_PIN 4

//Variable
int counter = 0;

// Task handles (optional, useful if you want to control tasks later)
TaskHandle_t ledTaskHandle = NULL;
TaskHandle_t buttonTaskHandle = NULL;

// LED Blink Task
void ledTask(void *parameter) {
  pinMode(LED_PIN, OUTPUT);

  while (true) {
    digitalWrite(LED_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(500)); // LED ON for 500ms

    digitalWrite(LED_PIN, LOW);
    vTaskDelay(pdMS_TO_TICKS(500)); // LED OFF for 500ms
  }
}

// Button Read Task
void buttonTask(void *parameter) {
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Use internal pull-up

  bool lastState = HIGH;

  while (true) {
    bool currentState = digitalRead(BUTTON_PIN);

    if (currentState == LOW && lastState == HIGH) {
      counter++;
      Serial.print("Button Pressed!");
      Serial.println(counter);
    }

    lastState = currentState;
    vTaskDelay(pdMS_TO_TICKS(50)); // Small delay to reduce CPU usage + debounce
  }
}

void setup() {
  Serial.begin(115200);

  // Create LED task on Core 0
  xTaskCreatePinnedToCore(
    ledTask,            // Task function
    "LED Task",         // Name
    2048,               // Stack size
    NULL,               // Parameters
    1,                  // Priority
    &ledTaskHandle,     // Task handle
    0                   // Core 0
  );

  // Create Button task on Core 1
  xTaskCreatePinnedToCore(
    buttonTask,         // Task function
    "Button Task",      // Name
    2048,               // Stack size
    NULL,               // Parameters
    1,                  // Priority
    &buttonTaskHandle,  // Task handle
    1                   // Core 1
  );
}

void loop() {
  // Nothing here. FreeRTOS tasks are running.
}