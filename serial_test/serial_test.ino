String str="";

void setup() {
  Serial.begin(9600); //set up serial library baud rate to 9600
}

void loop() {
  str = "";
  while (Serial.available() > 0)
  {
    str += char(Serial.read());   // read是剪切，而不是复制
    delay(10);  // 延时
  }
  if (str.length() > 0)
  {
    Serial.print(F("命令行发送内容："));
    Serial.println(str);
  }
}
