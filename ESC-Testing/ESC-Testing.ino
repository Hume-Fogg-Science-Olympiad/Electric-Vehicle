#include <Servo.h>

Servo ESC;

void setup() {
  // put your setup code here, to run once:
  
  ESC.attach(9, 1000, 2000);
}

void loop() {
  // put your main code here, to run repeatedly:

  ESC.write(0);

  delay(1000);

  int speed = 0;
  for (int i = 1; i <= 10; i++) {
    speed += 10 * i;
    ESC.write(speed);
    delay(500);
  }

  delay(1000);
  
  for (int i = 1; i <= 10; i++) {
    speed -= 10 * i;
    ESC.write(speed);
    delay(500);
  }
}
