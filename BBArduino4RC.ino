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

void set_servo(int i, int n) {
  int v = n * (servo_max[i] - servo_min[i]) / 32;
  servo_pos[i] = max(v, servo_min[i]);
  servo_pos[i] = min(v, servo_max[i]);
  servo[i].write(servo_pos[i]);
  Serial.print("Servo ");
  Serial.print(i, DEC);
  Serial.print(" : ");
  Serial.println(servo_pos[i], DEC);
} 

void set_motor(int i, int n) {
  int v = n * (motor_max[i] - motor_min[i]) / 32;
  if (v < motor_min[i]) {
    if (motor_speed[i] < v) {
      motor_speed[i] = motor_min[i];
    } else {
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
  
void setup() {
  Serial.begin(9600);
  servo_count = 2;
  setup_servo(0,3,90,0,180);
  setup_servo(1,5,90,0,180);
  motor_count = 1;
  setup_motor(0,6,30,255);
  led_count = 1;
  setup_led(0,13,0,0);
  Serial.println("Setup completed");
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
    switch (cmd>>5) {
      case 0: // Y1
        Serial.print("Y1: ");
        Serial.print(cmd & 31, DEC);
        Serial.print(" #");
        Serial.println(cmd, BIN);
        set_motor(0, cmd & 31);
        break;
      case 1: // X1
        Serial.print("X1: ");
        Serial.print(cmd & 31, DEC);
        Serial.print(" #");
        Serial.println(cmd, BIN);
        break;
      case 2: // Y2
        Serial.print("Y2: ");
        Serial.print(cmd & 31, DEC);
        Serial.print(" #");
        Serial.println(cmd, BIN);
        set_servo(0, cmd & 31);
        break;
      case 3: // X2
        Serial.print("X2: ");
        Serial.print(cmd & 31, DEC);
        Serial.print(" #");
        Serial.println(cmd, BIN);
        set_servo(1, cmd & 31);
        break;
      case 4: // z+t
        Serial.print("ZT: ");
        led_state[0] = (cmd & 31) >> 4;
        led_blink[0] = (cmd & 15) * 200;
        Serial.print(led_state[0], DEC);
        Serial.print(" % ");
        Serial.print(led_blink[0], DEC);
        Serial.print(" #");
        Serial.println(cmd, BIN);
        break;
      default:
        Serial.print("Unknown command: ");
        Serial.println(cmd, DEC);
      break;
    }
  }
}
