/*#
* BBArduino4RC - arduino code
* Copyright: Pierre Neyron <pierre.neyron@free.fr>
* License: GNU GPL v3
*/
#include <Servo.h>
int cmd = 0;
int servo_count = 6;
Servo servo[6];
int servo_pos[6];
int servo_min[6];
int servo_max[6];
int led_count = 6;
int led_pin[6];
int led_internal[6];
int led_state[6];
int led_blink[6];
int motor_count = 6;
int motor_pin[6];
int motor_speed[6];
int motor_min[6];
int motor_max[6];

void setup_servo(int i, int p, int v, int v_min, int v_max) {
  servo_pos[i] = v;
  servo_min[i] = v_min;
  servo_max[i] = v_max;
  servo[i].attach(p);
  servo[i].write(servo_pos[i]);
}

void setup_led(int i, int p, int s, int b) {
  led_pin[i] = p;
  led_state[i] = s;
  led_blink[i] = b;
  pinMode(p, OUTPUT);
}

void setup_motor(int i, int p, int v_min, int v_max) {
  motor_pin[i] = p;
  motor_min[i] = v_min;
  motor_max[i] = v_max;
  motor_speed[i] = 0;
  pinMode(p, OUTPUT);
  analogWrite(motor_pin[i], motor_speed[i]);
}

void set_led_internal(int i, int v) {
  if (led_internal[i] != v) {
    Serial.print("Led ");
    Serial.print(i, DEC);
    Serial.print(" : ");
    if (v == 0) {
      digitalWrite(led_pin[i], LOW);
      led_internal[i] = v;
      Serial.println(v, DEC);
    } else if (v == 1) {
      digitalWrite(led_pin[i], HIGH); 
      led_internal[i] = v;
      Serial.println(v, DEC);
    } else {
      Serial.println("unknown value");
    }
  }
}

void set_servo(int i, int v) {
  servo_pos[i] = max(v, servo_min[i]);
  servo_pos[i] = min(v, servo_max[i]);
  servo[i].write(servo_pos[i]);
  Serial.print("Servo ");
  Serial.print(i, DEC);
  Serial.print(" : ");
  Serial.println(servo_pos[i], DEC);
} 

void set_servo_inc(int i, int v_inc) {
  int v = servo_pos[i] + v_inc;
  set_servo(i, v);
} 
  
void set_motor(int i, int v) {
  if (v < motor_min[i]) {
    if (motor_speed[i] < v) {
  	  Serial.println("up");
      motor_speed[i] = motor_min[i];
    } else {
  	  Serial.println("down");
      motor_speed[i] = 0;
    }
  } else {
    motor_speed[i] = v;
  }
  motor_speed[i] = min(motor_speed[i], motor_max[i]);
  analogWrite(motor_pin[i], motor_speed[i]);
  Serial.print("Motor ");
  Serial.print(i, DEC);
  Serial.print(" : ");
  Serial.println(motor_speed[i], DEC);
} 
  
void set_motor_inc(int i, int v_inc) {
  int v = motor_speed[i] + v_inc;
  set_motor(i, v);
} 

void setup() {
  Serial.begin(9600);
  servo_count = 2;
  setup_servo(0,3,90,15,170);
  setup_servo(1,5,90,15,170);
  led_count = 2;
  setup_led(0,2,0,500);
  setup_led(1,13,0,1000);
  motor_count = 1;
  setup_motor(0,11,25,255);
}

void loop() {
  for (int i = 0 ; i < led_count ; i++) {
    if (led_state[i] == 1) {
      if (led_blink[i] > 0 and (millis() % (led_blink[i] * 2)) < led_blink[i]) {
        set_led_internal(i, LOW);
      } else {
        set_led_internal(i, HIGH);
      }
    } else {
      set_led_internal(i, LOW);
    }        
  }
  if (Serial.available() > 0) {
    cmd = Serial.read();
    switch (cmd) {
      case 113: // q
        led_state[0] = 1; 
        break;
      case 119: // w
        led_state[0] = 0; 
        break;
      case 97: // a
        led_state[1] = 1; 
        break;
      case 115: // s
        led_state[1] = 0; 
        break;
      case 122: // z
        set_servo_inc(0,5);
        break;
      case 120: // x
        set_servo_inc(0,-5);
        break;
      case 99: // c
        set_servo_inc(1,5);
        break;
      case 118: // v
        set_servo_inc(1,-5);
        break;
      case 100: // d
        set_motor_inc(0,5);
        break;
      case 102: // f
        set_motor_inc(0,-5);
        break;
      default:
        Serial.print("Unknown command: ");
        Serial.println(cmd, DEC);
      break;
    }
  }
}
