/*
 * le code enregistre s'il y a un véhicule monte ou descent dans un googlesheet avec un timestamp
 * L'utilisation de RFID est ignoré.
 *
 */

//WIFI*******
#if defined ARDUINO_ARCH_ESP8266  // s'il s'agit d'un ESP8266
#include <ESP8266WiFi.h>
#elif defined ARDUINO_ARCH_ESP32  // s'il s'agit d'un ESP32
#include "WiFi.h"
#endif

#include <WiFiClientSecure.h>
//*******WIFI

#include <Servo.h>


const int sigPin[3] = {D0,D1,D2};
const int SerPin = D8;

int mode = 0;
bool goingUP = true;
int count = 0;
const int margin = 10;
const int servCW = 20;
const int servCCW = 150;
const int servSTOP = 90;
Servo myservo;

//WIFI**********
const char *ssid = "iPhone"; // à remplacer par le nom de votre réseau WiFi
const char *password = "11706224"; // à remplacer par le mot de passe WiFi
const char* host = "script.google.com";
String url;

//à remplacer par le fingerprint SHA1 de votre App Script
// utile pour l'ESP8266 mais pas pour l'ESP32
const char fingerprint[] PROGMEM = "9c 03 67 e3 4e b7 66 21 f3 5d b7 1f f2 06 33 bc 2c ad 17 6a";
//b3 3f f3 e4 5f 76 59 2e f9 a2 0b f7 99 5e 44 89 21 4d 2e 22
int compteur = 0;
//**********WIFI


void setup() {
  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed
  
  //WIFI*******************************
  Serial.print("Connexion a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connecte"); 
  //*******************************WIFI
  
  myservo.attach(SerPin);
}
void loop() {
  CheckReadings();Serial.println(mode); 
  //dataSend();
  delay(500);
  myservo.attach(SerPin);
  switch (mode){
    case 0: //waiting for a vehicul to come
      if (ReadSigX(0)<margin)
      {
        myservo.write(servCW);
        mode = 1;
        goingUP = true;
      }
      else if (ReadSigX(2)<margin)
      {
        myservo.write(servCCW);
        mode = 1;
        goingUP = false;
      }
      break;
    case 1: //wait for a vehicul to enter 
      if (ReadSigX(0)>margin && ReadSigX(2)>margin && ReadSigX(1)<1000)
      {
        count++;
        dataSend();
        myservo.write(goingUP ? servCCW : servCW);
        mode = 2;
      }
      break;
    case 2: //wait for a vehicul to leave
      if ((ReadSigX(0)<margin || ReadSigX(2)<margin) && ReadSigX(1)>0)
        mode = 3;
      break;
    case 3: //wait for the ground to clear
      if (ReadSigX(0)>margin && ReadSigX(2)>margin && ReadSigX(1)>0)
        mode = 0; 
      break;
  }

}

//show the reading data for each sensor on the serial screen
void CheckReadings(){
  Serial.println("=========");
  Serial.println(ReadSigX (0));
  Serial.println(ReadSigX (1));
  Serial.println(ReadSigX (2));
}
//Read and return the data from a specific sensor X in cm
long ReadSigX (int i) {
  pinMode(sigPin[i], OUTPUT);
  digitalWrite(sigPin[i], LOW);
  delayMicroseconds(2);
  digitalWrite(sigPin[i], HIGH);
  delayMicroseconds(10);
  digitalWrite(sigPin[i], LOW);
  pinMode(sigPin[i], INPUT);
  long duration;
  duration = pulseIn(sigPin[i], HIGH);//timeout => duration=0
  return duration * 0.034 / 2; //0.034cm/us
}

void dataSend()
{
  Serial.print("Connexion a ");
  Serial.println(host);

  WiFiClientSecure client;
  client.setInsecure();
  const int httpPort = 443;

  #if defined ARDUINO_ARCH_ESP8266  // s'il s'agit d'un ESP8266
    client.setFingerprint(fingerprint);
    //client.verify(fingerprint, host);
  #endif
  
  client.connect(host, httpPort);
  
  if (goingUP)
  url = "/macros/s/AKfycbws1eEV99HAfV9MSQ3hltqjnsiSrM64Hz3q4QFGVk7T3NNUXxE/exec?func=addData&val1=" + String("UP") ;
  else
  url = "/macros/s/AKfycbws1eEV99HAfV9MSQ3hltqjnsiSrM64Hz3q4QFGVk7T3NNUXxE/exec?func=addData&val1=" + String("DOWN") ;

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(500);
  String section = "header";
  while (client.available())
  {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  Serial.println("Fermeture de la connexion.");
  
}
