#include <U8g2lib.h>

#define R 10 //红灯
#define Y 9 //黄灯
#define G 8 //绿灯
#define BEEP 13 //蜂鸣器30

#define AdirPin 3     // A方向控制引脚
#define AstepPin 4    // A步进控制引脚
#define BdirPin 5     // B方向控制引脚
#define BstepPin 6    // B步进控制引脚

#define Q1 37
#define Q2 29
#define Q3 35
#define Q4 27
#define Q5 33
#define Q6 25
#define Q7 31
#define Q8 23

#define back 2000 //暂停点后退距离
#define Line 250 //过线后移动距离
#define Line2 270 //过线后移动距离
#define ZJ 1000 //旋转90度
#define W 10 //旋转补偿偏移量
#define box 12 //药仓检测

int Aim=0;//目的地编号
int ZY=3;
int count=0;//过线数
int Mid=2;
int Far=2;
int Long=2;
int place=0;//绝对位置

String Data=" "; 
char inData=' ';
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0,21,20,U8X8_PIN_NONE);

void OLED_Show(void);

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);//连接蓝牙
  Serial2.begin(9600);//连接openmv
  Serial3.begin(9600);
  pinMode(2,OUTPUT);
  digitalWrite(2,HIGH);//步进电机电平比较端
  pinMode(AdirPin,OUTPUT);
  pinMode(AstepPin,OUTPUT);
  pinMode(BdirPin,OUTPUT);
  pinMode(BstepPin,OUTPUT);
  pinMode(R,OUTPUT);digitalWrite(R,HIGH);
  pinMode(Y,OUTPUT);digitalWrite(Y,HIGH);
  pinMode(G,OUTPUT);digitalWrite(G,HIGH);
  pinMode(box,INPUT);
  pinMode(Q1,INPUT);pinMode(Q2,INPUT);
  pinMode(Q3,INPUT);pinMode(Q4,INPUT);
  pinMode(Q5,INPUT);pinMode(Q6,INPUT);
  pinMode(Q7,INPUT);pinMode(Q8,INPUT);
  pinMode(BEEP,OUTPUT);digitalWrite(BEEP,LOW);
  u8g2.begin();
  BT_begin();//蓝牙自动连接
}

void BT_begin(void)
{
  Serial1.print("AT+CO00015872002B1");
  delay(100);
  if(Serial1.available())
  {
    String D=Serial1.readString();
    if(D=="OK+CO00A")
    {
      u8g2.clearBuffer();  
      u8g2.setFont(u8g2_font_logisoso28_tf);
      u8g2.drawStr(0,48,"-BTYES-");
      u8g2.sendBuffer();  
    }
  }
}

void OpenmvRead(void)
{
  while(Serial2.read()>=0);
//  Serial2.print('1');//发送开启指令
  while(Serial2.available()==0);//等待信号传输
  if(Serial2.available())
  {
    inData=Serial2.read();//读取接收的字符
    if(inData=='1')Aim=1;
    if(inData=='2')Aim=2;
    if(inData=='3')Aim=3;
    if(inData=='4')Aim=4;
    if(inData=='5')Aim=5;
    if(inData=='6')Aim=6;
    if(inData=='7')Aim=7;
    if(inData=='8')Aim=8;
    inData=' ';
    while(Serial2.read()>=0);//清除缓存区数据
  }
}

void OpenmvZY(void)
{
  while(Serial2.read()>=0);
  Serial2.write(0x31);//发送开启指令
  while(Serial2.available()==0);//等待信号传输
  if(Serial1.available())
  {
    inData=Serial2.read();//读取第一个接收的字符
    if(inData=='1')ZY=0;
    if(inData=='2')ZY=1;
    if(inData=='0')ZY=2;
    inData=' ';
    while(Serial2.read()>=0);//清除缓存区数据
  }
}

void OpenmvZY4(void)
{
  while(Serial2.read()>=0);
  Serial2.write(0x32);//发送开启指令
  while(Serial2.available()==0);//等待信号传输
  if(Serial2.available())
  {
    inData=Serial2.read();//读取第一个接收的字符
    if(inData=='1')ZY=0;
    if(inData=='2')ZY=1;
    if(inData=='0')ZY=2;
    inData=' ';
    while(Serial2.read()>=0);//清除缓存区数据
  }
}

void Advance(int a,int t)  //a为前进距离，x=800时为轮子转一圈，距离约为23.56cm。 t为一次脉冲的周期时间（us）。频率f=1/t。
{
   digitalWrite(AdirPin,LOW);//A电机逆时针为向前
   digitalWrite(BdirPin,HIGH);//B电机顺时针为向前
   for(int i=0;i<a;i++)
   {
     digitalWrite(AstepPin,HIGH); 
     digitalWrite(BstepPin,HIGH);
     delayMicroseconds(t/2);
     digitalWrite(AstepPin,LOW); 
     digitalWrite(BstepPin,LOW);
     delayMicroseconds(t/2);   
   }
}

void Back(int a,int t)  //a为后退距离，x=800时为轮子转一圈，距离约为23.56cm。 t为一次脉冲的周期时间。频率f=1/t。
{
   digitalWrite(AdirPin,HIGH);//A电机顺时针为向后
   digitalWrite(BdirPin,LOW);//B电机逆时针为向后
   for(int i=0;i<a;i++)
   {
     digitalWrite(AstepPin,HIGH); 
     digitalWrite(BstepPin,HIGH);
     delayMicroseconds(t/2);
     digitalWrite(AstepPin,LOW); 
     digitalWrite(BstepPin,LOW);
     delayMicroseconds(t/2);
   }
}

void Clockwise(int a,int t)  //a为相对顺时针自转角度，x=800时为轮子转一圈，旋转角度未知。 t为一次脉冲的周期时间（us）。频率f=1/t。
{
   digitalWrite(BdirPin,HIGH);//B电机顺时针为向前
   for(int i=0;i<a;i++)
   {
     digitalWrite(BstepPin,HIGH);
     delayMicroseconds(t/2);
     digitalWrite(BstepPin,LOW);
     delayMicroseconds(t/2);
   }
   while(!digitalRead(Q4)&&!digitalRead(Q5))
   {
     for(int i=0;i<10;i++)
     {
       digitalWrite(BstepPin,HIGH);
       delayMicroseconds(t/2);
       digitalWrite(BstepPin,LOW);
       delayMicroseconds(t/2);
     }
   }
   for(int i=0;i<10;i++)
   {
     digitalWrite(BstepPin,HIGH);
     delayMicroseconds(t/2);
     digitalWrite(BstepPin,LOW);
     delayMicroseconds(t/2);
   }
}

void AnitClockwise(int a,int t)  //a为相对逆时针自转角度，x=800时为轮子转一圈，旋转角度未知。 t为一次脉冲的周期时间（us）。频率f=1/t。
{   
   digitalWrite(AdirPin,LOW);//A电机逆时针为向前
   for(int i=0;i<a;i++)
   {
     digitalWrite(AstepPin,HIGH); 
     delayMicroseconds(t/2);
     digitalWrite(AstepPin,LOW); 
     delayMicroseconds(t/2);
   }
   while(!digitalRead(Q4)&&!digitalRead(Q5))
   {
     for(int i=0;i<10;i++)
     {
       digitalWrite(AstepPin,HIGH); 
       delayMicroseconds(t/2);
       digitalWrite(AstepPin,LOW); 
       delayMicroseconds(t/2);
     }
   }
   for(int i=0;i<10;i++)
   {
     digitalWrite(AstepPin,HIGH); 
     delayMicroseconds(t/2);
     digitalWrite(AstepPin,LOW); 
     delayMicroseconds(t/2);
   }
}

void AnitClockwisePX(int a,int t)  //a为相对顺时针自转角度，x=800时为轮子转一圈，旋转角度未知。 t为一次脉冲的周期时间（us）。频率f=1/t。
{
   digitalWrite(AdirPin,HIGH);//A电机顺时针为向后
   digitalWrite(BdirPin,HIGH);//B电机顺时针为向前
   for(int i=0;i<a;i++)
   {
     digitalWrite(AstepPin,HIGH); 
     digitalWrite(BstepPin,HIGH);
     delayMicroseconds(t/2);
     digitalWrite(AstepPin,LOW); 
     digitalWrite(BstepPin,LOW);
     delayMicroseconds(t/2);
   }
}

void ClockwisePX(int a,int t)  //a为相对逆时针自转角度，x=800时为轮子转一圈，旋转角度未知。 t为一次脉冲的周期时间（us）。频率f=1/t。
{
   digitalWrite(AdirPin,LOW);//A电机逆时针为向前
   digitalWrite(BdirPin,LOW);//B电机逆时针为向后
   for(int i=0;i<a;i++)
   {
     digitalWrite(AstepPin,HIGH); 
     digitalWrite(BstepPin,HIGH);
     delayMicroseconds(t/2);
     digitalWrite(AstepPin,LOW); 
     digitalWrite(BstepPin,LOW);
     delayMicroseconds(t/2);
   }
}

void track_Q()
{
  int D1=digitalRead(Q3);
  int D2=digitalRead(Q4);
  int D3=digitalRead(Q5);
  int D4=digitalRead(Q6);
  if((D1==0)&&(D2==0)&&(D3==0)&&(D4==1))
  {
    AnitClockwisePX(10,3000);
  }else if((D1==0)&&(D2==0)&&(D3==1)&&(D4==0))
  {
    AnitClockwisePX(10,3000);
  }else if((D1==0)&&(D2==0)&&(D3==1)&&(D4==1))
  {
    AnitClockwisePX(10,3000);
  }else if((D1==1)&&(D2==0)&&(D3==0)&&(D4==0))
  {
    ClockwisePX(10,3000);
  }else if((D1==0)&&(D2==1)&&(D3==0)&&(D4==0))
  {
    ClockwisePX(10,3000);
  }else if((D1==1)&&(D2==1)&&(D3==0)&&(D4==0))
  {
    ClockwisePX(10,3000);
  }
  Advance(50,1800);
}

void loop() {
  if(Serial1.available())//蓝牙接收器
  {
    Data=Serial1.readString();//读取字符串
     
    //返回的是绝对位置
    if(Data=="4")//中端病房 去同一个中端病房
    {
      u8g2.clearBuffer();  
      u8g2.setFont(u8g2_font_logisoso28_tf);
      u8g2.drawStr(0,48,"--YES--");
      u8g2.sendBuffer();
      OpenmvRead();//获取任务码
      OLED_Show();
      while(digitalRead(box)==0);//等待药物放下
      delay(500);
      int X=1;
      while(X)//过红线
      {
        track_Q();
        if(digitalRead(Q1)&&digitalRead(Q2)&&digitalRead(Q3)&&digitalRead(Q4)&&digitalRead(Q5)&&digitalRead(Q6)&&digitalRead(Q7)&&digitalRead(Q8))
        {
          X=0;
          break;
        }
      }
      Advance(Line2,2000);
      AnitClockwise(ZJ,2000);
      ClockwisePX(W,2000);
      Back(back,2000);
      digitalWrite(Y,LOW);//亮黄灯等待
      while(Serial1.available()==0);
      if(Serial1.available())
      {
        if(Serial1.read()=='O')digitalWrite(Y,HIGH);//灭灯
      }
      while(Serial1.available()==0);
      if(Serial1.available())
      {
        if(Serial1.read()=='K')//可以回去了
        {
          delay(3000);delay(3000);
          X=1;
          while(X)//过红线
          {
            track_Q();
            if(digitalRead(Q1)&&digitalRead(Q2)&&digitalRead(Q3)&&digitalRead(Q4)&&digitalRead(Q5)&&digitalRead(Q6)&&digitalRead(Q7)&&digitalRead(Q8))
            {
              X=0;
              break;
            }
          }
          Advance(Line,2000);
          Clockwise(ZJ,2000);
          X=1;
          while(X)//过红线
          {
            track_Q();
            if(digitalRead(Q1)&&digitalRead(Q2)&&digitalRead(Q3)&&digitalRead(Q4)&&digitalRead(Q5)&&digitalRead(Q6)&&digitalRead(Q7)&&digitalRead(Q8))
            {
              X=0;
              break;
            }
          }
          Advance(Line,2000);
          AnitClockwise(ZJ,2000);
          X=1;
          while(X)//到达病房
          {
            track_Q();
            if(!digitalRead(Q1)&&!digitalRead(Q2)&&!digitalRead(Q3)&&!digitalRead(Q4)&&!digitalRead(Q5)&&!digitalRead(Q6)&&!digitalRead(Q7)&&!digitalRead(Q8))
            {
              Advance(30,2000);
              if(!digitalRead(Q1)&&!digitalRead(Q2)&&!digitalRead(Q3)&&!digitalRead(Q4)&&!digitalRead(Q5)&&!digitalRead(Q6)&&!digitalRead(Q7)&&!digitalRead(Q8))
              {
                X=0;
                break;
              }
            }
          }
        }
        //已到达收货区4
        digitalWrite(R,LOW);//亮红灯
        digitalWrite(BEEP,HIGH);
        delay(500);
        digitalWrite(BEEP,LOW);
        while(digitalRead(box)==1);
        delay(500);
        digitalWrite(R,HIGH);//灭红灯
        //开始返回
        Back(100,2000);
        X=1;
        while(X)
        {
          Back(30,2000);
          if(digitalRead(Q3)&&digitalRead(Q4)&&digitalRead(Q5)&&digitalRead(Q6))
          {
            X=0;
            break;
          }
        }
        Advance(Line,2000);
        AnitClockwise(ZJ,2000);
        X=1;
        while(X)
        {
          track_Q();//灰度传感器状态全白即到达位置------------------------------------------------
          if(!digitalRead(Q1)&&!digitalRead(Q2)&&!digitalRead(Q3)&&!digitalRead(Q4)&&!digitalRead(Q5)&&!digitalRead(Q6)&&!digitalRead(Q7)&&!digitalRead(Q8))
          {
            Advance(30,2000);
            if(!digitalRead(Q1)&&!digitalRead(Q2)&&!digitalRead(Q3)&&!digitalRead(Q4)&&!digitalRead(Q5)&&!digitalRead(Q6)&&!digitalRead(Q7)&&!digitalRead(Q8))
            {
              X=0;
              break;
            }
          }
        }
        digitalWrite(G,LOW);//亮绿灯
        digitalWrite(BEEP,HIGH);
        delay(500);
        digitalWrite(BEEP,LOW);
        while(1);
      }
    }

    if(Data=="3")//中端病房 去同一个中端病房
    {
      u8g2.clearBuffer();  
      u8g2.setFont(u8g2_font_logisoso28_tf);
      u8g2.drawStr(0,48,"--YES--");
      u8g2.sendBuffer();
      OpenmvRead();//获取任务码
      OLED_Show();
      while(digitalRead(box)==0);//等待药物放下
      delay(500);
      int X=1;
      while(X)//过红线
      {
        track_Q();
        if(digitalRead(Q1)&&digitalRead(Q2)&&digitalRead(Q3)&&digitalRead(Q4)&&digitalRead(Q5)&&digitalRead(Q6)&&digitalRead(Q7)&&digitalRead(Q8))
        {
          X=0;
          break;
        }
      }
      Advance(Line,2000);
      Clockwise(ZJ,2000);
      AnitClockwisePX(W,2000);
      Back(back,2000);
      digitalWrite(Y,LOW);//亮黄灯等待
      while(Serial1.available()==0);
      if(Serial1.available())
      {
        if(Serial1.read()=='O')digitalWrite(Y,HIGH);//灭灯
      }
      while(Serial1.available()==0);
      if(Serial1.available())
      {
        if(Serial1.read()=='K')//可以回去了
        delay(3000);delay(3000);
        {
          X=1;
          while(X)//过红线
          {
            track_Q();
            if(digitalRead(Q1)&&digitalRead(Q2)&&digitalRead(Q3)&&digitalRead(Q4)&&digitalRead(Q5)&&digitalRead(Q6)&&digitalRead(Q7)&&digitalRead(Q8))
            {
              X=0;
              break;
            }
          }
          Advance(Line,2000);
          AnitClockwise(ZJ,2000);
          X=1;
          while(X)//过红线
          {
            track_Q();
            if(digitalRead(Q1)&&digitalRead(Q2)&&digitalRead(Q3)&&digitalRead(Q4)&&digitalRead(Q5)&&digitalRead(Q6)&&digitalRead(Q7)&&digitalRead(Q8))
            {
              X=0;
              break;
            }
          }
          Advance(Line,2000);
          Clockwise(ZJ,2000);
          X=1;
          while(X)//到达病房
          {
            track_Q();
            if(!digitalRead(Q1)&&!digitalRead(Q2)&&!digitalRead(Q3)&&!digitalRead(Q4)&&!digitalRead(Q5)&&!digitalRead(Q6)&&!digitalRead(Q7)&&!digitalRead(Q8))
            {
              Advance(30,2000);
              if(!digitalRead(Q1)&&!digitalRead(Q2)&&!digitalRead(Q3)&&!digitalRead(Q4)&&!digitalRead(Q5)&&!digitalRead(Q6)&&!digitalRead(Q7)&&!digitalRead(Q8))
              {
                X=0;
                break;
              }
            }
          }
        }
        //已到达收货区3
        digitalWrite(R,LOW);//亮红灯
        digitalWrite(BEEP,HIGH);
        delay(500);
        digitalWrite(BEEP,LOW);
        while(digitalRead(box)==1);
        delay(500);
        digitalWrite(R,HIGH);//灭红灯
        //开始返回
        Back(100,2000);
        X=1;
        while(X)
        {
          Back(30,2000);
          if(digitalRead(Q3)&&digitalRead(Q4)&&digitalRead(Q5)&&digitalRead(Q6))
          {
            X=0;
            break;
          }
        }
        Advance(Line,2000);
        Clockwise(ZJ,2000);
        X=1;
        while(X)
        {
          track_Q();//灰度传感器状态全白即到达位置------------------------------------------------
          if(!digitalRead(Q1)&&!digitalRead(Q2)&&!digitalRead(Q3)&&!digitalRead(Q4)&&!digitalRead(Q5)&&!digitalRead(Q6)&&!digitalRead(Q7)&&!digitalRead(Q8))
          {
            Advance(30,2000);
            if(!digitalRead(Q1)&&!digitalRead(Q2)&&!digitalRead(Q3)&&!digitalRead(Q4)&&!digitalRead(Q5)&&!digitalRead(Q6)&&!digitalRead(Q7)&&!digitalRead(Q8))
            {
              X=0;
              break;
            }
          }
        }
        digitalWrite(G,LOW);//亮绿灯
        digitalWrite(BEEP,HIGH);
        delay(500);
        digitalWrite(BEEP,LOW);
        while(1);
      }
    }
    //绝对位置5-8，发挥部分2
//
//    
//    if(Data=="A"){OpenmvRead();OLED_Show();}
//    if(Data=="B"){OpenmvZY();Serial1.println(ZY);}
//    if(Data=="C"){OpenmvZY4();Serial1.println(ZY);}
//    if(Data=="TQ"){while(1){track_Q();}}
//    if(Data=="Qian")Advance(200,2000);
//    if(Data=="Hou")Back(200,2000);
//    if(Data=="QianShort")Advance(100,2000);
//    if(Data=="HouShort")Back(100,2000);
//    if(Data=="QianLong")Advance(400,2000);
//    if(Data=="HouLong")Back(400,2000);
//    if(Data=="Shun")Clockwise(ZJ,2000);
//    if(Data=="Ni")AnitClockwise(ZJ,2000);
//    if(Data=="readQ")
//    {
//      Serial1.print(digitalRead(Q1));Serial1.print(digitalRead(Q2));
//      Serial1.print(digitalRead(Q3));Serial1.print(digitalRead(Q4));
//      Serial1.print(digitalRead(Q5));Serial1.print(digitalRead(Q6));
//      Serial1.print(digitalRead(Q7));Serial1.println(digitalRead(Q8));
//    }
//    if(Data=="K")//可以回去了
//    {
//      int X=1;
//      while(X)//到达病房
//      {
//        track_Q();
//        if(!digitalRead(Q1)&&!digitalRead(Q2)&&!digitalRead(Q3)&&!digitalRead(Q4)&&!digitalRead(Q5)&&!digitalRead(Q6)&&!digitalRead(Q7)&&!digitalRead(Q8))
//        {
//          X=0;
//          break;
//        }
//      }
//    }
//    inData=' ';
    Data=" ";
    }//结束
}

void OLED_Show(void)
{
  switch(Aim)
  {
    case 1:{
      u8g2.clearBuffer();  
      u8g2.setFont(u8g2_font_logisoso28_tf);
      u8g2.drawStr(0,48,"---1---");
      u8g2.sendBuffer(); 
      break;
    }
    case 2:{
      u8g2.clearBuffer();  
      u8g2.setFont(u8g2_font_logisoso28_tf);
      u8g2.drawStr(0,48,"---2---");
      u8g2.sendBuffer(); 
      break;
    }
    case 3:{
      u8g2.clearBuffer();  
      u8g2.setFont(u8g2_font_logisoso28_tf);
      u8g2.drawStr(0,48,"---3---");
      u8g2.sendBuffer(); 
      break;
    }
    case 4:{
      u8g2.clearBuffer();  
      u8g2.setFont(u8g2_font_logisoso28_tf);
      u8g2.drawStr(0,48,"---4---");
      u8g2.sendBuffer(); 
      break;
    }
    case 5:{
      u8g2.clearBuffer();  
      u8g2.setFont(u8g2_font_logisoso28_tf);
      u8g2.drawStr(0,48,"---5---");
      u8g2.sendBuffer(); 
      break;
    }
    case 6:{
      u8g2.clearBuffer();  
      u8g2.setFont(u8g2_font_logisoso28_tf);
      u8g2.drawStr(0,48,"---6---");
      u8g2.sendBuffer(); 
      break;
    }
    case 7:{
      u8g2.clearBuffer();  
      u8g2.setFont(u8g2_font_logisoso28_tf);
      u8g2.drawStr(0,48,"---7---");
      u8g2.sendBuffer(); 
      break;
    }
    case 8:{
      u8g2.clearBuffer();  
      u8g2.setFont(u8g2_font_logisoso28_tf);
      u8g2.drawStr(0,48,"---8---");
      u8g2.sendBuffer(); 
      break;
    }
  }
}
