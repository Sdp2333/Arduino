#include <SoftwareSerial.h>
#include <U8g2lib.h>
#include "U8glib.h"
//左电机端口定义
#define MotorLpin1   35 //控制位3
#define MotorLpin2   37 //控制位4
#define MotorLFpwm    3  //使能调速 ENB左前轮
#define MotorLBpwm    5  //使能调速 ENB左后轮
#define MotorLcountA 18 //编码器A 中断号：5
#define MotorLcountB 20 //编码器B 中断号：4
 
//右电机端口定义
#define MotorRpin1   31 //控制位1
#define MotorRpin2   33 //控制位2
#define MotorRFpwm    2  //使能调速 ENA
#define MotorRBpwm    6  //使能调速 ENA
#define MotorRcountA 19 //编码器A 中断号：3
#define MotorRcountB 21 //编码器B 中断号：2

 
volatile float motorL=0;//中断变量，左轮子脉冲计数
volatile float motorR=0;//中断变量，右轮子脉冲计数
float V_L=20; //左轮速度 单位cm/s
float V_R=20; //右边轮速 单位cm/s
int v1=0;  //单位cm/s
int v2=0;  //单位cm/s
float Target_V_L=40,Target_V_R=40;   //单位cm/s
int Pwm_L=0,Pwm_R=0;  //左右轮PWM
int x;//存放数组
String A_String = "";//定义用来存数据的字符串
//以上为电机部分

#define Q1 23
#define Q2 31
#define Q3 25
#define Q4 33
#define Q5 27
//以上为灰度传感器部分

int Aim=0;//目的地编号

char inData=' ';

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0,21,20,U8X8_PIN_NONE);//21 SCL引脚 20 IIC引脚

SoftwareSerial softSerial(10,11); //定义软串口,rx为10号端口,tx为11号端口
//PID变量
                              float kp=0,ki=0.1,kd=0; //PID参数
 
 
/**************************************
 * Arduino初始化函数
 * 
 *************************************/
void setup() {
   Motor_Init();//电机端口初始化
   softSerial.begin(9600);
   Serial.begin(9600);//开启串口
      u8g2.begin();
      u8g2.clearBuffer();  
      u8g2.setFont(u8g2_font_logisoso28_tf);
      u8g2.drawStr(0,48,"-------");
      u8g2.sendBuffer();
}
 
 
 
/*********************************************************
 * 函数功能：增量式PI控制器(左轮)
 * 入口参数：当前速度（编码器测量值），目标速度
 * 返回 值：电机PWM 
 * 参考资料： 
 *    增量式离散PID公式：
 *                Pwm-=Kp*[e(k)-e(k-1)]+Ki*e(k)+Kd*[e(k)-2e(k-1)+e(k-2)]
 *                e(k):本次偏差
 *                e(k-1):上一次偏差
 *                e(k-2):上上次偏差
 *                Pwm:代表增量输出
 *    在速度闭环控制系统里面我们只使用PI控制，因此对PID公式可简化为：
 *                Pwm-=Kp*[e(k)-e(k-1)]+Ki*e(k)
 *                e(k):本次偏差
 *                e(k-1):上一次偏差
 *                Pwm:代表增量输出
 *                
 *    注意增量式PID先调I,再调P,最后再调D
 *********************************************************/
 int Incremental_Pi_L(int current_speed,int target_speed){
  static float pwm,bias,last_bias,prev_bias;  //静态变量存在程序全周期：pwm:增量输出,bias:本次偏差,last_bias:上次偏差,prev_bais_:上上次偏差
  bias=current_speed-target_speed;    //计算本次偏差e(k)
  pwm-=(kp*(bias-last_bias)+ki*bias+kd*(bias-2*last_bias+prev_bias));   //增量式PID控制器
  prev_bias=last_bias;  //保存上上次偏差
  last_bias=bias;     //保存上一次偏差
 
 
 
  //PWM 限幅度  Arduino的PWM 最高为255  限制在250
  if(pwm<-250){
    pwm=250;     
  }
  if(pwm>250){
    pwm=250;  
  }
  //Serial.println(pwm);
  return pwm;         //增量输出
 }
 
 
//右轮速度增量式PID控制器
int Incremental_Pi_R(float current_speed,float target_speed){
  static float pwm,bias,last_bias,prev_bias;  //静态变量存在程序全周期：pwm:增量输出,bias:本次偏差,last_bias:上次偏差,prev_bais_:上上次偏差
  bias=current_speed-target_speed;    //计算本次偏差e(k)
  pwm-=(kp*(bias-last_bias)+ki*bias+kd*(bias-2*last_bias+prev_bias));   //增量式PID控制器
  prev_bias=last_bias;  //保存上上次偏差
  last_bias=bias;     //保存上一次偏差
 
  //PWM 限幅度  Arduino的PWM 最高为255限制在250
  if(pwm<-250){
    pwm=250;     
  }
  if(pwm>250){
    pwm=250;  
  }
  //Serial.println(pwm);
  return pwm;         //增量输出
 }
 /***************以下为电机部分函数************/
/**************************************************************************（测试完成）
  函数功能：设置双轮工作模式和运动速度
  入口参数：工作模式，左右轮pwm
  返回  值：无
**************************************************************************/
void Set_Pwm(int mode,int speed_L,int speed_R){
 
  if(mode==1){
  //前进模式
  //左电机
  digitalWrite(MotorLpin1,LOW);
  digitalWrite(MotorLpin2,HIGH);
  analogWrite(MotorLFpwm,speed_L);
  analogWrite(MotorLBpwm,speed_L);
  //右电机
  digitalWrite(MotorRpin1,HIGH);
  digitalWrite(MotorRpin2,LOW);
  analogWrite(MotorRFpwm,speed_R);
  analogWrite(MotorRBpwm,speed_R);
  
  }else if(mode==2){
  //后退模式
  //左电机
  digitalWrite(MotorLpin1,HIGH);
  digitalWrite(MotorLpin2,LOW);
  analogWrite(MotorLFpwm,speed_L);
  analogWrite(MotorLBpwm,speed_L);
  //右电机
  digitalWrite(MotorRpin1,LOW);
  digitalWrite(MotorRpin2,HIGH);
  analogWrite(MotorRFpwm,speed_R);
  analogWrite(MotorRBpwm,speed_R);
  }else if(mode==3){
  //左转模式
  //左电机
  digitalWrite(MotorLpin1,LOW);
  digitalWrite(MotorLpin2,HIGH);
  analogWrite(MotorLFpwm,speed_L);
  analogWrite(MotorLBpwm,speed_L);
  //右电机
  digitalWrite(MotorRpin1,LOW);
  digitalWrite(MotorRpin2,HIGH);
  analogWrite(MotorRFpwm,speed_R);
  analogWrite(MotorRBpwm,speed_R);
  }else if(mode==4){
  //右转模式
  //左电机
  digitalWrite(MotorLpin1,HIGH);
  digitalWrite(MotorLpin2,LOW);
  analogWrite(MotorLFpwm,speed_L);
  analogWrite(MotorLBpwm,speed_L);
  
  //右电机
  digitalWrite(MotorRpin1,HIGH);
  digitalWrite(MotorRpin2,LOW);
  analogWrite(MotorRFpwm,speed_R);
  analogWrite(MotorRBpwm,speed_R);
  
  }else if(mode==5){
  //停止模式
  //左电机
  digitalWrite(MotorLpin1,LOW);
  digitalWrite(MotorLpin2,LOW);
  //analogWrite(MotorLpwm,speed_L);
  
  //右电机
  digitalWrite(MotorRpin1,LOW);
  digitalWrite(MotorRpin2,LOW);
 // analogWrite(MotorRpwm,speed_R);
  }
}
 
 
 /**************************************************************************（测试完成）
  函数功能：电机端口初始化，控制芯片引脚拉低
  入口参数：无
  返回  值：无
**************************************************************************/
void Motor_Init(){
  //左电机
  pinMode(MotorLpin1,OUTPUT);  //驱动芯片控制引脚
  pinMode(MotorLpin2,OUTPUT);  //驱动芯片控制引脚
  pinMode(MotorLFpwm,OUTPUT); 
  pinMode(MotorLBpwm,OUTPUT);   //驱动芯片控制引脚，PWM调速
  pinMode(MotorLcountA,INPUT); //左轮编码器A引脚
  pinMode(MotorLcountB,INPUT); //左轮编码器B引脚
  
  //右电机
  pinMode(MotorRpin1,OUTPUT);  //驱动芯片控制引脚
  pinMode(MotorRpin2,OUTPUT);  //驱动芯片控制引脚
  pinMode(MotorRFpwm,OUTPUT); 
  pinMode(MotorRBpwm,OUTPUT);   //驱动芯片控制引脚，PWM调速
  pinMode(MotorRcountA,INPUT); //右轮编码器A引脚
  pinMode(MotorRcountB,INPUT); //右轮编码器B引脚
 
  //驱动芯片控制引脚全部拉低
  digitalWrite(MotorLpin1,LOW); //左电机
  digitalWrite(MotorLpin2,LOW);
  digitalWrite(MotorLFpwm,LOW);
  digitalWrite(MotorLBpwm,LOW);
  digitalWrite(MotorRpin1,LOW); //右电机
  digitalWrite(MotorRpin2,LOW);
  digitalWrite(MotorRFpwm,LOW);
  digitalWrite(MotorRBpwm,LOW);
}
 
 
 
/***********************************
 * 电机实际速度计算：
 * 公式：
 * 已知参数：
 *     车轮直径65mm,
 *     左边轮子一圈：13*45=585脉冲（RISING）每圈脉冲数等于每圈脉冲数*减速比的倒数
 *     右边轮子一圈：585脉冲（RISING），
 * 单位时间读两个轮子脉冲读取两个轮子脉冲
 ***********************************/
 void Read_Moto_V(){
  unsigned long nowtime=0;
  motorL=0;
  motorR=0;
  nowtime=millis()+50;//读50毫秒
  attachInterrupt(digitalPinToInterrupt(MotorLcountA),Read_Moto_L,RISING);//左轮脉冲开中断计数
  attachInterrupt(digitalPinToInterrupt(MotorRcountA),Read_Moto_R,RISING);//右轮脉冲开中断计数
  while(millis()<nowtime); //达到50毫秒关闭中断
  detachInterrupt(digitalPinToInterrupt(MotorLcountA));//左轮脉冲关中断计数
  detachInterrupt(digitalPinToInterrupt(MotorRcountA));//右轮脉冲关中断计数
  V_L=((motorL/585)*6.5*PI)/0.05;   //单位cm/s
  V_R=((motorR/585)*6.5*PI)/0.05;   //单位cm/s
  v1=V_L;
  v2=V_R;
}
 
 
 
/***************************
 * 中断函数：读左轮脉冲
 *
 **************************/
void Read_Moto_L(){
  motorL++;
}
 
 
 
/**************************
 * 中断函数：读右轮脉冲
 * 
 *************************/
void Read_Moto_R(){
  motorR++;
}

/****************************************以上为电机部分函数**************************************************/

 void OpenmvRead(void)//读取软串口数据
{
  while(Serial.read()>=0);//清除缓存
  while(Serial.available()==0);//等待信号传输
  if(Serial.available())//接收到openmv发来的数字
  {
    inData=Serial.read();//读取接收的字符
    if(inData=='1')Aim=1;                         //'1' '1''1''1''1''1''1''1''1''1''1''1''1''1''1''1''1''1''1''1'
    if(inData=='2')Aim=2;
    if(inData=='3')Aim=3;
    if(inData=='4')Aim=4;
    if(inData=='5')Aim=5;
    if(inData=='6')Aim=6;
    if(inData=='7')Aim=7;
    if(inData=='8')Aim=8;
    inData=' ';
    while(Serial.read()>=0);//清除缓存区数据
  }
}
 void track_Q()//循迹部分
{
  int D1=digitalRead(Q1);
  int D2=digitalRead(Q2);
  int D3=digitalRead(Q4);
  int D4=digitalRead(Q5);
  if((D1==0)&&(D2==0)&&(D3==0)&&(D4==1))
  {
    Set_Pwm(4,Pwm_L,Pwm_R);
  }else if((D1==0)&&(D2==0)&&(D3==1)&&(D4==0))
  {
    Set_Pwm(4,Pwm_L,Pwm_R);
  }else if((D1==0)&&(D2==0)&&(D3==1)&&(D4==1))
  {
   Set_Pwm(4,Pwm_L,Pwm_R);
  }else if((D1==1)&&(D2==0)&&(D3==0)&&(D4==0))
  {
   Set_Pwm(3,Pwm_L,Pwm_R);
  }else if((D1==0)&&(D2==1)&&(D3==0)&&(D4==0))
  {
   Set_Pwm(3,Pwm_L,Pwm_R);
  }else if((D1==1)&&(D2==1)&&(D3==0)&&(D4==0))
  {
   Set_Pwm(3,Pwm_L,Pwm_R);
  }
 Set_Pwm(1,Pwm_L,Pwm_R);
}

void OLEDshow(void)
{
if(Aim==1){      
      u8g2.clearBuffer();  
      u8g2.setFont(u8g2_font_logisoso28_tf);
      u8g2.drawStr(0,48,"---1---");
      u8g2.sendBuffer();      
}
if(Aim==2){     
      u8g2.clearBuffer();  
      u8g2.setFont(u8g2_font_logisoso28_tf);
      u8g2.drawStr(0,48,"---2---");
      u8g2.sendBuffer();
}
}
/***************************************
 * Arduino主循环
 * 
 ***************************************/
void loop() {
  Read_Moto_V();//读取脉冲计算速度
  Pwm_L=Incremental_Pi_L(V_L,Target_V_L);//左轮PI运算
  Pwm_R=Incremental_Pi_R(V_R,Target_V_R);//右轮PI运算
    Serial.println(V_L);  //直接用串口绘图画出速度曲线
    Serial.print(",");
    Serial.println(V_R);
   Set_Pwm(1,Pwm_L,Pwm_R);  //设置左右轮速度
 //任务部分主代码------------------------------------
      // OpenmvRead();//获取任务码
      // OLEDshow();

}
