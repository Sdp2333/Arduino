#include <U8g2lib.h>
#include "U8glib.h"
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0,21,20,U8X8_PIN_NONE);//21 SCL引脚 20 IIC引脚
#define STOP      0 //停止
#define RUN       1 //前进
#define BACK      2 //后退
#define LEFT      3 //左转
#define RIGHT     4 //右转

int a1 = 38;//左电机1
int a2 = 39;//左电机2
int b1 = 40;//右电机1
int b2 = 41;//右电机2
int sensor1 = 17; //从车头方向的最右边开始排序 探测器
int sensor2 = 16; 
int sensor3 = 15; 
int sensor4 = 14; 
int leftPWM = 3;//L298N使能端左
int rightPWM = 2;//L298N使能端右
void setup()
{
  Serial.begin(9600);//电机控制引脚初始化
  pinMode(a1, OUTPUT);
  pinMode(a2, OUTPUT);
  pinMode(b1, OUTPUT);
  pinMode(b2, OUTPUT);
  pinMode(leftPWM, OUTPUT);
  pinMode(rightPWM, OUTPUT);
  pinMode(sensor1, INPUT);//寻迹模块引脚初始化
  pinMode(sensor2, INPUT);
  pinMode(sensor3, INPUT);
  pinMode(sensor4, INPUT);
  u8g2.begin();
  u8g2.clearBuffer();  
  u8g2.setFont(u8g2_font_logisoso28_tf);
  u8g2.drawStr(0,48,"-------");
  u8g2.sendBuffer();
}
void loop() 
{
  tracking();       
}
void Work(int c,int value)
{
  analogWrite(leftPWM, value);  //设置PWM输出，设置速度
  analogWrite(rightPWM, value);
  
  switch(c)
  {
     case RUN:
      // Serial.println("RUN"); //前进输出状态
      u8g2.clearBuffer();  
      u8g2.setFont(u8g2_font_logisoso28_tf);
      u8g2.drawStr(0,48,"--RUN---");
      u8g2.sendBuffer();   
      digitalWrite(a1, HIGH);
      digitalWrite(a2, LOW);
      digitalWrite(b1, HIGH);
      digitalWrite(b2, LOW);
      break;
     case BACK:
      //Serial.println("BACK"); //后退输出状态
      u8g2.clearBuffer();  
      u8g2.setFont(u8g2_font_logisoso28_tf);
      u8g2.drawStr(0,48,"--BAC---");
      u8g2.sendBuffer();   
      digitalWrite(a1, LOW);
      digitalWrite(a2, HIGH);
      digitalWrite(b1, LOW);
      digitalWrite(b2, HIGH);
      break;
     case LEFT:
     // Serial.println("LEFT"); //左转输出状态
     u8g2.clearBuffer();  
      u8g2.setFont(u8g2_font_logisoso28_tf);
      u8g2.drawStr(0,48,"--LFT---");
      u8g2.sendBuffer();   
      digitalWrite(a1, HIGH);
      digitalWrite(a2, LOW);
      digitalWrite(b1, LOW);
      digitalWrite(b2, HIGH);
      break;
     case RIGHT:
      //Serial.println("RIGHT"); //右转弯输出状态
      u8g2.clearBuffer();  
      u8g2.setFont(u8g2_font_logisoso28_tf);
      u8g2.drawStr(0,48,"--RIG---");
      u8g2.sendBuffer();   
      digitalWrite(a1, LOW);
      digitalWrite(a2, HIGH);
      digitalWrite(b1, HIGH);
      digitalWrite(b2, LOW);
      break;
     default:
      //Serial.println("STOP"); //停止输出状态
      u8g2.clearBuffer();  
      u8g2.setFont(u8g2_font_logisoso28_tf);
      u8g2.drawStr(0,48,"--STP---");
      u8g2.sendBuffer();   
      digitalWrite(a1, LOW);
      digitalWrite(a2, LOW);
      digitalWrite(b1, LOW);
      digitalWrite(b2, LOW);
  }
}
void tracking()
{
  int d[4];
  d[0] = digitalRead(17);
  d[1] = digitalRead(16);
  d[2] = digitalRead(15);
  d[3] = digitalRead(14);
  if(!d[0] && !d[1] && !d[2] && !d[3]) //左右都没有检测到黑线
  {
    Work(RUN, 100);
  }
  if(d[0] || d[1]) //右边检测到黑线
  {
    Work(RIGHT, 90);
  }
  if(d[2] || d[3]) //左边检测到黑线
  {
    Work(LEFT, 90);
  }
  if(d[0] && d[3]) //左右都检测到黑线是停止
  {
    Work(STOP, 0);
    while(1);
  }
  Serial.print(d[0]);
  Serial.print("---");
  Serial.print(d[1]);
  Serial.print("---");
  Serial.print(d[2]);
  Serial.print("---");
  Serial.println(d[3]);
}


