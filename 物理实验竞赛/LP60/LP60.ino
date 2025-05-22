#include <SoftwareSerial.h>

#define controlButton 5
SoftwareSerial lp60Serial(8, 9);

byte lp60Data[8];
int buttonState = HIGH; 

void setup()
{
  Serial.begin(9600);
  lp60Serial.begin(9600);
  pinMode(controlButton, INPUT);
}

bool checkData() 
{
  // 如果第一字节为0x55且最后一字节为0xAA，返回true
  if (lp60Data[0] == 0x55 && lp60Data[7] == 0xAA) 
  {
    return true;
  }
  // 否则，返回false
  else 
  {
    return false;
  }
}

int convertToDecimal() 
{
  // 把第三字节到第六字节拼接成一个32位整数
  int n = (lp60Data[2] << 24) | (lp60Data[3] << 16) | (lp60Data[4] << 8) | lp60Data[5];
  // 返回这个整数的十进制表示
  return n;
}

void lp60Measure()
{
  byte command[8] = { 0x55, 0x05, 0x00, 0x00, 0x00, 0x00, 0xCC, 0xAA }; // 启动测量
  for (int i = 0; i < 8; i++) 
  {
    lp60Serial.write(command[i]);
  }
}
float lp60GetLen()
{
  lp60Measure();
  delay(200); //延迟避免测量过快
  
  if (lp60Serial.available()>= 8) 
  { 
    lp60Serial.readBytes(lp60Data, 8); // 读取8字节到缓冲区
    for (int i = 0; i < 8; i++) 
    {
      Serial.print(lp60Data[i], HEX); // 输出每个字节的值（16进制）
      Serial.print(" ");
    }
    Serial.print("\n");
  }
  int length ;
  if (checkData())
  {
    length = convertToDecimal();
  }
  return length/1.0;            //转换单位到m
}

void loop()
{
  Serial.println("请在不装水时按动按钮");
  buttonState = digitalRead(controlButton);
  while(buttonState == LOW)
  {
    buttonState = digitalRead(controlButton);
  }
  Serial.println(buttonState);
  float airLength= lp60GetLen();
  Serial.print("空气中光程是：");
  Serial.println(airLength);
  Serial.println("请装水，并按动按钮");
  buttonState = digitalRead(controlButton);
  while(buttonState == LOW)
  {
    buttonState = digitalRead(controlButton);
  }
  float waterLength= lp60GetLen();
  Serial.print("水中光程是：");
  Serial.println(waterLength);
  Serial.print("水的对于该种激光的折射率是：");
  Serial.println(waterLength/airLength);
}
