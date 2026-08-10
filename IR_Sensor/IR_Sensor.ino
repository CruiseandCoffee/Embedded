// IR Sensor with ESP32-S3

const int IR_PIN = 4;   // OUT pin of IR sensor

void setup()
{
  Serial.begin(115200);
  pinMode(IR_PIN, INPUT);
  Serial.println("IR Sensor Started...");
}

void loop()
{
  int sensorValue = digitalRead(IR_PIN);
  if(sensorValue == LOW)
  {
    Serial.println("Object Detected");
  }
  else
  {
    Serial.println("No Object");
  }

  delay(300);
}
