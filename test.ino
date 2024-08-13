#include<Arduino.h>
#include<Vector.h>
#include<Wire.h>

// i2c addresses
#define TWI_FREQ 100000L
byte ikb1z = 0x48;

// pins
int led_pin = 18;
int sw_1 = 0;
int us_1 = 34;

// i/o pins
std::vector<int> input_pins({us_1, sw_1});
std::vector<int> output_pins({led_pin});

// class
class slave {
  public:
    byte addr;
    String name;
    slave(byte addr_, String name_ = "noname"){
      addr = addr_;
      name = name_;
    }
    void send(byte reg,byte data){
      Wire.beginTransmission(this->addr);
      Wire.write(reg);
      Wire.write(data);
      Wire.endTransmission();
    }
};
class ikb1z_board : public slave {
  public:
    int motor_max_speed = 100;
    int servo_max_rotation = 180;
    void motor(int channel, byte speed){
      byte reg;
      switch(channel){
        case 1: reg = 0x21;break;
        case 2: reg = 0x22;break;
        case 3: reg = 0x23;break;
        case 4: reg = 0x24;break;
      }
      speed = (speed <= motor_max_speed) ? speed : motor_max_speed;
      this->send(reg, speed);
    }
    void servo(int channel, byte position){
      byte reg = 0x40;
      switch(channel){
        case 10: reg = reg|0x1;break;
        case 11: reg = reg|0x2;break;
        case 12: reg = reg|0x4;break;
        case 13: reg = reg|0x8;break;
        case 14: reg = reg|0x10;break;
        case 15: reg = reg|0x20;break;
      }
      Serial.println(position, HEX);
      position = (position <= servo_max_rotation) ? position : servo_max_rotation;
      this->send(reg, position);
    }
};

// variable
ikb1z_board ikbz(ikb1z);
int btn_state;
int btn_last_state;
int motor_run = 0;
int motor_max_speed = 100;
int servo_max_rotation = 180;

// functions
void flip(int &val){val = !val;}
void add(int &val, int num){val += num;}
void led(int state) {analogWrite(led_pin, (state == 1) ? 10 : 0 );}
int ultraSo() {return analogRead(us_1);}
byte int28b(int integer) {return static_cast<uint8_t>(integer);}
void switch_(int mode=0){
  btn_state = digitalRead(sw_1);
  if(btn_state == 1 && btn_last_state == 0){
      flip(motor_run);
    }
  btn_last_state = btn_state;
}

void setup() {
  for(int pin : input_pins) pinMode(pin, INPUT);
  for(int pin : output_pins) pinMode(pin, OUTPUT);
  Serial.begin(9600);
  // i2c start 
  Wire.begin();
}

void loop() {
  switch_();
  if(motor_run==0){ikbz.motor(1, 0);ikbz.motor(2, 0);}
  else if(motor_run==1){ikbz.motor(1, motor_max_speed);ikbz.motor(2, motor_max_speed);}
  else if(motor_run==2){ikbz.motor(1, motor_max_speed);ikbz.motor(2, -motor_max_speed);}
  else if(motor_run==3){ikbz.motor(1, -motor_max_speed);ikbz.motor(2, motor_max_speed);}
  int distance = ultraSo();
  (distance <= 300) ? led(1) : led(0);
  // ikbz.motor(1, 255);
  // ikbz.motor(2, -255);
  // for(int i = 0;i<256;i++){
  //   ikbz.servo(10, i);
  //   ikbz.servo(15, i);
  //   delay(300);
  // }
  // for(int i = 0;i<256;i++){
  //   ikbz.motor(1, i);
  //   ikbz.motor(2, i);
  //   delay(300);
  // }
}
