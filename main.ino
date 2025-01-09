#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <TridentTD_LineNotify.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

//เเก้ตรงนี้
const int WeightPerBottle = 50; //น้ำหนักต่อขวดน้ำ
const int countBottle = 10; //เเจ้งเตือนถ้ามีขวดน้ำครบ x ขวด
const float DetectRange[2] = {8.55,12.2}; // {ระยะตรวจจับเเรก , ระยะยกเลิกการตรวจจับ}
#define SSID        "Debsirinnon_2.4G" //ชื่อไวไฟ
#define PASSWORD    "" //รหัสไวไฟ (ไม่มีให้ใส่ "" )
#define LINE_TOKEN  "NpGK7TRGC5iO8JdjAj7oiPBcQ00YrS81ArCXZWjRnI5" //Line Token

/* =========================== */

const int pingPin = D6;
const int inPin = D5;
const int MaxError = 795;
const int MaxTimeOut = 2e5;



//ตัวนับ
long long Bottles = 0;

void updateScreen(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bottle(s) = "+String(Bottles));
  lcd.setCursor(0, 1);
  lcd.print("Weight = "+String(Bottles*WaterPerBottle));
}

void setup()
{
  pinMode(pingPin, OUTPUT);
  pinMode(inPin, INPUT);
  Serial.begin(115200);
  lcd.begin();
  lcd.setCursor(0, 0); // กำหนดให้ เคอร์เซอร์ อยู่ตัวอักษรตำแหน่งที่0 แถวที่ 1 เตรียมพิมพ์ข้อความ
  lcd.print("Bottle Counter");
  delay(2000);
  lcd.clear();
  if(PASSWORD=="")WiFi.begin(SSID);
  else WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n",  SSID);
  lcd.print("Connecting...");
  lcd.setCursor(0, 1);
  lcd.print("Please Wait...");
  while(WiFi.status() != WL_CONNECTED) { Serial.print("."); delay(400); }
  Serial.printf("\nWiFi connected\nIP : ");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connected!");
  Serial.println(WiFi.localIP());  
  LINE.setToken(LINE_TOKEN);
  LINE.notify("เครื่องนับจำนวนขวดน้ำพร้อมใช้งานเเล้ว!!!");
  delay(1500);
  lcd.clear();
  updateScreen();
}

float get_distance(){
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin, LOW);
  long duration = pulseIn(inPin, HIGH,MaxTimeOut);
  float val = duration * 0.034 / 2.0;
  //Serial.println(val);
  if(val>=MaxError)return 0;
  return val;
}

void SendNotify(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Notify Sended.");
  LINE.notify("ขวดมีจำนวนเกิน "+String(Bottles)+" ขวดเเล้ว!!\nน้ำหนักรวม "+String(Bottles*WeightPerBottle)+" กรัม");
  updateScreen();
}

int _CountMode = 0;
bool inRange(){
  if(_CountMode == 0){
    return get_distance() < DetectRange[0];
  }
  else{
    return get_distance() < DetectRange[1];
  }
}

void loop(){
  if(inRange()^_CountMode){
    _CountMode = !_CountMode;
    Serial.println("Update!");
    if(_CountMode){
      Bottles++;
      updateScreen();
      if(Bottles%countBottle == 0){
        SendNotify();
      }
    }
  }

  
  delay(100);
}
