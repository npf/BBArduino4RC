/*#
* BBArduino4RC - arduino code
* Copyright: Pierre Neyron <pierre.neyron@free.fr>
* License: GNU GPL v3
*/
#include <Servo.h>
int cmd = 0;
Servo servo[6];
int led[6];
int servo_pos[6];
int servo_min[6];
int servo_max[6];

void setup() {
  Serial.begin(9600);
  servo[0].attach(3);
  servo[1].attach(5);
  servo[2].attach(6);
  servo[3].attach(9);
  servo[4].attach(10);
  servo[5].attach(11);
  servo_pos[0]=90;
  servo_min[0]=15;
  servo_max[0]=170;
  servo_pos[1]=90;
  servo_min[1]=15;
  servo_max[1]=170;
  for(int i=0;i<6;i++) {
    servo[i].write(servo_pos[i]);
  }
  led[0] = 2;
  pinMode(led[0], OUTPUT);
}

void servo_turn(int i, int v) {
  servo_pos[i] += v;
  if (servo_pos[i] < servo_min[i]) {
    servo_pos[i] = servo_min[i];
  } else if (servo_pos[i] > servo_max[i]) {
    servo_pos[i] = servo_max[i];
  }
  servo[i].write(servo_pos[i]);
  Serial.print("Servo ");
  Serial.print(i, DEC);
  Serial.print(" : ");
  Serial.print(v, DEC);
  Serial.print(" -> ");
  Serial.println (servo_pos[i], DEC);
} 

void loop() {
  if (Serial.available() > 0) {
    cmd = Serial.read();
    switch (cmd) {
      case 13:
        digitalWrite(led[0], HIGH); 
        break;
      case 127:
        digitalWrite(led[0], LOW); 
        break;
      case 67: //left arrow: 27,91,67 
        servo_turn(0,5);
        break;
      case 68: //right arrow: 27,91,68
        servo_turn(0,-5);
        break;
      case 53: // page up: 27,91,53,126
        servo_turn(1,5);
        break;
      case 54: // page down: 27,91,54,126
        servo_turn(1,-5);
        break;
      default:
        Serial.print("Unknown command: ");
        Serial.println(cmd, DEC);
      break;
    }
  }
}


