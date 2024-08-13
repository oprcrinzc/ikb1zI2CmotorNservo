#include<Arduino.h>
#include<Wire.h>

// i2c addresses
#define TWI_FREQ 100000L
byte ikb1z = 0x48;

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

void setup() {
  Serial.begin(9600);

  // i2c start 
  Wire.begin();
}
void loop(){}
