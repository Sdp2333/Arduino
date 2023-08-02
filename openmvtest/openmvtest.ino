#include <SoftwareSerial.h>

//设置软件串口
SoftwareSerial softSerial(2, 3); // RX, TX
typedef struct
{
  int data[50][2] = {{0,0}};
  int len = 0;
}List;
List list;

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200); // 设置波特率
  softSerial.begin(115200); // 设置软件串口波特率，必须与OpenMV中的一致
  Serial.print("hello");
}

void loop() 
{
  if(softSerial.available())  // 如果串口缓存区中有数据
  {
    getList();  // 将数据存储到结构体的数组list.data[][]中
    for (int i=0; i<list.len; i++)  //  遍历结构体数组
    {
      Serial.print(list.data[i][0]);    // 打印中心的x坐标cx()
      Serial.print('\t'); // 打印制表符  
      Serial.println(list.data[i][1]);  //打印中心的y坐标cy()
    }
    Serial.println("============");
    clearList();  //清空内存
  }

}


String detectString() // 判断传入的字符串能否被接收
{
  // 我们传入的数据形式 you send: {(90, 101)}
  while(softSerial.read() != '{');  // 如果数据不以'{'开头，那么我们会一直进入死循环

  //返回"{}"中的字符串
  return(softSerial.readStringUntil('}'));
                                          // readStringUntil函数可用于从设备接收到的数据中读取信息。读取到的数据信息将以字符串形式返回
                                          // 当函数读取到终止字符后，会立即停止函数执行。此时函数所返回的字符串为”终止字符”前的所有字符信息
}


void clearList()//清空内存
{
  memset(list.data, sizeof(list.data),0);
  list.len = 0;
}

void getList()  // 重点是把数据存储在结构体中的数组list.data[][]中！
{
  String s = detectString();// 将返回的字符串({}中的元素)赋值给s
                      
  // 此时s中的数据形如：(90, 101)
  String numStr = "";
  for(int i = 0; i<s.length(); i++) // 遍历刚刚返回的字符串"s"
  {
    if(s[i]=='(') // 读到开头
    {
      numStr = "";
    }
    else if(s[i] == ',')  // 读到数据分割标记','
    {
      // toInt()将有效字符串转换为整数。输入字符串应以整数开头。如果 String 包含非整数，则该函数将停止执行转换。
      list.data[list.len][0] = numStr.toInt();  // 将刚刚拼接好的数据str（色块的cx()）用toInt函数转换成整形，存储在结构体数组list.data[list.len][0]中
      
      numStr = "";  // 清空numstr，方便接受','后的另一组数据
    }
    else if(s[i]==')') // 读到结尾
    {
      list.data[list.len][1] = numStr.toInt();  // 将刚刚拼接好的数据str（色块的cy()）用toInt函数转换成整形，存储在结构体数组list.data[list.len][1]中
      numStr = "";
      list.len++; // 长度+1，读取下一组坐标数组
    }
    else  // 读到数据
    {
      numStr += s[i]; // 相当于把读到的数字(此时为char型)按顺序拼接到numstr后
    }
  }
}
