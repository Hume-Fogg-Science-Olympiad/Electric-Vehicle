#include <Servo.h>

Servo ESC;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  ESC.attach(9, 1000, 2000);

  delay(2000);
}

int value = 140;
void loop() {
  ESC.write(value);

  if (Serial.available() > 0) {
    int temp = Serial.parseInt();
    if (temp > 1) {
      value = temp;
    } else if (temp == 1) {
      ESC.write(0);
      delay(500);

      for (int i = 0; i <= value/10; i++) {
        ESC.write(i * 10);
        delay(500);
      }
    }
  }

  Serial.println(value);
  delay(500);
}