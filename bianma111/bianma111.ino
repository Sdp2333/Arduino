#include "U8glib.h"
#include "stdio.h"
U8GLIB_SSD1306_128X64 u8g(13, 11, 10, 9); // SW SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9 

//电机接线
#define AIN1  12
#define AIN2  4
#define AIN3  7
#define AIN4  8
#define PWMA  5
#define PWMB  6
#define AA  2 
#define BB  3  
                                                
int valA=0,valB=0;
float  l,r;
int flag=0,pwm;
int lspeed=70,rspeed=70,speed_b=100;//speed_a:自定速度初始值 speed_b:目标速度
 
unsigned long starttime,stoptime;//定时器
void Ltimer()
{
  valA++;
  stoptime=millis();//毫秒级定时器 
   if((stoptime-starttime)>100) //每100毫秒记一次数
   { 
    detachInterrupt(0); //关中断
    flag=1;
   }

}
void Rtimer()
{
  valB++;
  stoptime=millis();//毫秒级定时器 
   if((stoptime-starttime)>100) //每100毫秒记一次数
   { 
    detachInterrupt(1); //关中断
    flag=1;
   }

}


void setup() {
  pinMode(AIN1,OUTPUT);
  pinMode(AIN2,OUTPUT);
  pinMode(AIN3,OUTPUT);
  pinMode(AIN4,OUTPUT);
  pinMode(PWMA,OUTPUT);
  pinMode(PWMB,OUTPUT);
  pinMode(AA,INPUT);
  pinMode(BB,INPUT);
  Serial.begin(9600);
  attachInterrupt(0,Ltimer,RISING); //上升沿开中断
   attachInterrupt(1,Rtimer,RISING); //上升沿开中断
  starttime=millis();
   Serial.println(starttime); 
 if ( u8g.getMode() == U8G_MODE_R3G3B2 ) 
    u8g.setColorIndex(255);     // white
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT )
    u8g.setColorIndex(3);         // max intensity
  else if ( u8g.getMode() == U8G_MODE_BW )
    u8g.setColorIndex(1);         // pixel on
   u8g.setFont(u8g_font_6x10); //设置字体
  u8g.setFontRefHeightExtendedText();//一坨
  u8g.setDefaultForegroundColor();
  u8g.setFontPosTop();
}
void loop() {  
  digitalWrite(AIN1,HIGH); 
  digitalWrite(AIN2,LOW);
  digitalWrite(AIN3,HIGH); 
  digitalWrite(AIN4,LOW);
  analogWrite(PWMA,lspeed); 
  analogWrite(PWMB,rspeed);
  
    if(flag==1)         //if()前面加while（1）不行
  
    { 
      l=valA;
      r=valB;//*100/156.000;     //放大一百倍串口绘图
      Serial.println(l,3);
      Serial.println(r,3);
      valA=0;
      valB=0;
      flag=0;
      
      
      if(l>speed_b) //如果编码器测得的实际速度大于我们设定的目标速度就减速
      {
        lspeed--;//speed 为60/100 r/s
      }
      else //如果编码器测得的实际速度小于我们设定的目标速度就加速
      {
        lspeed++;
      }
      if(r>speed_b) //如果编码器测得的实际速度大于我们设定的目标速度就减速
      {
        rspeed--;//speed 为60/100 r/s
      }
      else //如果编码器测得的实际速度小于我们设定的目标速度就加速
      {
        rspeed++;
      }
     delay(0);
      starttime=millis();
      attachInterrupt(0,Ltimer,RISING); 
      attachInterrupt(1,Rtimer,RISING);
    }
    u8g.firstPage();  
    do {
      char Lspeed[1];
      char Rspeed[1];
      itoa(l,Lspeed,10); 
      itoa(r,Rspeed,10);
    u8g.drawStr(0,0,Lspeed);
    u8g.drawStr(20,0,Rspeed);
    } while( u8g.nextPage() );
    delay(500);
}
