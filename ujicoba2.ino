//Library Firebase
#include <FirebaseArduino.h>

//Library Servo  
#include <Servo.h>

//Library WiFi
#include <ESP8266WiFi.h>

//Library LCD OLED 128x64
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Library RTC
#include <Wire.h>
#include "RTClib.h"
RTC_DS1307 rtc;

//Alamat I2C LCD OLED
#define OLED_Address 0x3C
//Resolusi LCD OLED
Adafruit_SSD1306 oled(128, 64);

// Set these to run example.
#define FIREBASE_HOST "-"
#define FIREBASE_AUTH "-"
#define WIFI_SSID "SmartHome"
#define WIFI_PASSWORD "SmartHome1315"

//defineultrasonic 1
const int trigPin = D7; // Trigger Pin of Ultrasonic Sensor
const int echoPin = D6; // Echo Pin of Ultrasonic Sensor

//defineultrasonic 2
const int trig = D4; // Trigger Pin of Ultrasonic Sensor
const int echo = D5; // Echo Pin of Ultrasonic Sensor

//define LDR
int pin_ldr = D0;

//Konstanta Mode
int ModeAuto=0;
int ModeTimer=0;
int ModeManual=0;

//konstanta RTC Timer
int set_jam;      
int set_menit;  
int set_detik;
  
int durasi;
int jarak;
int pos;

Servo valve;

void setup() {
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    }
    Serial.println();
    Serial.print("connected: ");
    Serial.println(WiFi.localIP());
    delay(500);
 
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  
//Set LCD OLED 128x64
oled.begin(SSD1306_SWITCHCAPVCC, OLED_Address);

//Mode Pin Ultrasonik1  
pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);

//Mode Pin Ultrasonik2
pinMode(trig, OUTPUT);
pinMode(echo, INPUT);

//Mode Pin LDR
pinMode(pin_ldr,INPUT);

//Mode Pin Servo
valve.attach(D3);
valve.write(20);

// Set UP RTC
#ifndef ESP8266
  while (!Serial); 
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    //rtc.adjust(DateTime(__DATE__, __TIME__));
  }
}

//koding timer mode
void timerMode(){
  //ambil set waktu dari fire base
  set_jam   = Firebase.getInt("settime_JAM");
  set_menit = Firebase.getInt("settime_MENIT");
  
  DateTime now = rtc.now();
  int jam = now.hour();
  int menit = now.minute();
  int detik = now.second();
 
 //kirim jam now ke firebase
 Firebase.set("timenow_NJAM", jam);
 Firebase.set("timenow_NMENIT", menit);
 Firebase.set("timenow_NDETIK", detik);
 Serial.print(jam, DEC);
 Serial.print(':');
 Serial.print(menit, DEC);
 Serial.print(':');
 Serial.print(detik, DEC);
 Serial.println();
 delay(1000);
 if(jam==set_jam && menit==set_menit) //jika jam, menit, detik sesuai dengan firebase maka memanggil servo2
 {
  servo3();
  delay(500);
  Firebase.set("settime_JAM","");
  Firebase.set("settime_MENIT","");
  }
  //Tampilan LCD Mode timer
  oled.clearDisplay();
  oled.setTextColor(WHITE);

 //Tampilan LCD SetTime
 oled.setCursor(45, 20);
 oled.setTextSize(2);
 oled.print(set_jam);
 oled.print(":");
 oled.print(set_menit);
 //Tampilan LCD Time Now
 oled.setCursor(45, 45);
 oled.setTextSize(1.8);
 oled.print(jam,DEC);
 oled.print(':');
 oled.print(menit,DEC);
 oled.print(':');
 oled.print(detik,DEC);
 oled.println(" ");
 
 oled.setCursor(35, 4);
 oled.setTextSize(1);
 oled.print("Timer Mode");
 
 oled.display();
 }
//CODING Auto MODE
void AutoMode()
{
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(2);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  Firebase.set("KJARAK",distance);
  Serial.print ("jarak");
  Serial.print (" : ");
  Serial.println (distance);
  if(distance<20)
  {
    servo1();
    delay(500);
    }
    else
    {
      autoLCD();
      }
      delay(500);
}

//Coding SERVO 1
void servo1(){
  if( digitalRead( pin_ldr ) == 0 ){
    feedingLCD();
  for (pos = 20; pos <= 90; pos += 10) { // goes from 0 degrees to 180 degrees
    valve.write(pos);              
    delay(15);                      
    }
  for (pos = 90; pos >= 20; pos -= 10) { // goes from 180 degrees to 0 degrees
    valve .write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
    }
    Serial.print ("Mode Auto ");
    Serial.print (": "); 
    Serial.println("FEEDING");
  for (pos = 20; pos <= 90; pos += 10) { // goes from 0 degrees to 180 degrees
    valve.write(pos);              
    delay(15);                      
    }
  for (pos = 90; pos >= 20; pos -= 10) { // goes from 180 degrees to 0 degrees
    valve .write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
    }
   }
    else {
      Serial.println("Terdapat Sisa");
      fullLCD();
    }
}

void servo2(){
  feedingLCD();
  for (pos = 20; pos <= 90; pos += 10) { // goes from 0 degrees to 180 degrees
    valve.write(pos);              
    delay(15);                      
    }
  for (pos = 90; pos >= 20; pos -= 10) { // goes from 180 degrees to 0 degrees
    valve .write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
    }
    Serial.print ("Mode Manual ");
    Serial.print (": "); 
    Serial.println("FEEDING");
  for (pos = 20; pos <= 90; pos += 10) { // goes from 0 degrees to 180 degrees
    valve.write(pos);              
    delay(15);                      
    }
  for (pos = 90; pos >= 20; pos -= 10) { // goes from 180 degrees to 0 degrees
    valve .write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
    } 
    Firebase.set("MANUAL", 0);
    delay(500);
    homeLCD();
}

void servo3(){
  feedingLCD();
   for (pos = 20; pos <= 90; pos += 10) { // goes from 0 degrees to 180 degrees
    valve.write(pos);              
    delay(15);                      
    }
  for (pos = 90; pos >= 20; pos -= 10) { // goes from 180 degrees to 0 degrees
    valve .write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
    }
    Serial.print ("Mode Timer ");
    Serial.print (": "); 
    Serial.println("FEEDING");
  for (pos = 20; pos <= 90; pos += 10) { // goes from 0 degrees to 180 degrees
    valve.write(pos);              
    delay(15);                      
    }
  for (pos = 90; pos >= 20; pos -= 10) { // goes from 180 degrees to 0 degrees
    valve .write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
    }
    homeLCD();
}

//Coding LCD
void homeLCD()
{
  //Tampilan Pada LCD
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  //oled.print("Tes LCD OLED 128x64");
  oled.setCursor(10, 10);
  oled.setTextSize(2);
  oled.print("SMART PET");
  
  oled.setCursor(30, 30);
  oled.setTextSize(2);
  oled.print("FEEDER");
  oled.display();
}

void autoLCD()
{
  //Tampilan Pada LCD
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setCursor(10, 10);
  oled.setTextSize(2);
  oled.print("SMART PET");
  
  oled.setCursor(30, 30);
  oled.setTextSize(2);
  oled.print("FEEDER");

  oled.setCursor(40, 50);
  oled.setTextSize(1);
  oled.print("Auto Mode");
  
  oled.display();
}

void feedingLCD()
{
  //Tampilan Pada LCD
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  
  oled.setCursor(30, 30);
  oled.setTextSize(2);
  oled.print("FEEDING");

  oled.display();
}

void fullLCD()
{
  //Tampilan Pada LCD
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  
  oled.setCursor(20, 30);
  oled.setTextSize(2);
  oled.print("FULLTANK");
  oled.display();
  delay(500);
}
//coding ultrasonik 2 (jarak stok pakan dispenser)
void kapasiti(){
    long durasi,jarak;
    int heightTank=12;
    int deviation=1;
  
      digitalWrite(trig,HIGH);
      delayMicroseconds(2);
      digitalWrite(trig,LOW);
      durasi=pulseIn(echo,HIGH);
      jarak=(durasi/2)/29.1;
      
      int percentage=100-(((jarak-deviation)*100)/heightTank);
      //Serial.println(jarak);
      //Serial.print(percentage);
      //Serial.println("%");
      Firebase.setInt("STOK",percentage);     
  }

  void loop() {
//Variabel mode ambil value dari firebase
ModeAuto    = Firebase.getInt("AUTO");
ModeManual  = Firebase.getInt("MANUAL");
ModeTimer   = Firebase.getInt("TIMER");

//kontrol mode
if(ModeAuto==1){
  AutoMode();
}  
else if(ModeManual==1){
   servo2();
} else {
  homeLCD();
}
if(ModeTimer==1){
  timerMode();
}else if (ModeTimer==0){
   //homeLCD();
}

//KODING sensor cahaya
//int ldrStatus = digitalRead(pin_ldr);
//if (ldrStatus == 1){
  //Serial.print("Terdapat Sisa Makan");
  //Serial.print(" = ");
  //Serial.println(pin_ldr);
  //Firebase.setInt("SISA",ldrStatus);
//}else 
//Serial.print("Kosong");
//Serial.print(" = ");
//Serial.println(pin_ldr);
//delay(1000);
int nilai = analogRead (pin_ldr); //Membaca nilai analog dari pin D0
int hasil = 1 * nilai / 800;
delay(1000);  
Serial.print("sisa = ");
Serial.println (hasil);
Firebase.setInt("SISA",hasil);

kapasiti();
}
