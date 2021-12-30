/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example runs directly on ESP8266 chip.

  Note: This requires ESP8266 support package:
    https://github.com/esp8266/Arduino

  Please be sure to select the right ESP8266 module
  in the Tools -> Board menu!

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

/* Fill-in your Template ID (only if using Blynk.Cloud) */
//#define BLYNK_TEMPLATE_ID   "YourTemplateID"

#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

const char* mqtt_server = "test.ranye-iot.net";
char msg[80];   //发送的数据
WiFiClient espClient;
PubSubClient client(espClient);
#define MSG_BUFFER_SIZE  (80)
void decodeJson(char msg[80]);
void encodeJson();
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "1T1Z9Y6prkdmsLWe6LUQrqyG0dMucz8r";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "615 vip 专线";
char pass[] = "zjsu1904615";

//#define  light 5
//#define  magnet 4
long Ctime = 0;
long lastAlarmTime=0;
int light_sensor_flag=1;
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("LSD");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {       //关注回调函数
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char Receive_data[80];

  for (int i = 0; i < length; i++) {
    Receive_data[i]=(char)payload[i];
  }
  decodeJson(Receive_data);

}

///////widget/////////////
WidgetLED tag_led(V3);

///////widget/////////////




////////////////////////////////JSON///////////////

void encodeJson(){                //发送数据 json 编码
  DynamicJsonBuffer jsonBuffer;
  JsonObject& pack = jsonBuffer.createObject();
  pack["AirTag_signal"] =1;
  pack.printTo(msg);   //绑定至待发数据
}


void decodeJson(char Msg[80]){   //JSON解码函数
  Serial.println("decoding");
  DynamicJsonBuffer jsonBuffer;
  JsonObject& pack = jsonBuffer.parseObject(Msg);

  if (pack["AirTag_response"]=="played"){
//    Serial.print("ok");
    tag_led.on();
    delay(1000);
    tag_led.off();
  }
  if (pack["duration"]){
    int tmp=pack["duration"];
   Blynk.virtualWrite(V9,tmp) ;
  }
  
}


void setup()
{
  // Debug console
  Serial.begin(9600);
  pinMode(4, INPUT);
  pinMode(A0, INPUT);
  pinMode(5, OUTPUT);
  Ctime = millis();
  Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 31, 160), 8080);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  if (!client.connected()) {
    reconnect();
  }
}


void loop()
{
  Blynk.run();
  if (!client.connected()) {
    reconnect();
  }
      client.loop();
  doorSensor();
//  Serial.println(light_sensor_flag);
  lightSensor(light_sensor_flag);
//  AirTag_request();
  delay(1000);
}

void doorSensor() {

  if (digitalRead(4) == LOW) {
    Ctime = millis();
  }
  if (digitalRead(4) != LOW) {

    if ((millis() - Ctime) >= 20000) {
      Blynk.virtualWrite(V5, "超时报警");
//      Blynk.email("929123714@qq.com","门锁报警","房门未关，请检查！");
        alarm();
      
    } else {
      Blynk.virtualWrite(V5, "未关");
    }
  }
  else {
    Blynk.virtualWrite(V5, "正常闭合");
 
  }


}
void lightSensor(int flag) {
//  Serial.println(analogRead(A0));
 if(flag){
    if (analogRead(A0) >500) {
      Blynk.virtualWrite(V0, "环境光线较暗，已为您自动开灯！");
      //开灯
      light(1);
    } else {
      Blynk.virtualWrite(V0, "当前室内光照很不错哦！");
      light(0);
    }
 }
  

}

void light(int flag) {
//  Serial.println(flag);
  if (flag == 1) {
    
    digitalWrite(5, HIGH);
  } else {
   
    digitalWrite(5, LOW);
  }
}
//void AirTag_request(){
//  
//    encodeJson();
//    client.publish("LSD", msg);
//  
//}
void alarm(){
  long Now=millis();
  if(Now-lastAlarmTime>20000){
    DynamicJsonBuffer jsonBuffer;
  JsonObject& pack = jsonBuffer.createObject();
  pack["alarm"] =1;
  pack.printTo(msg);   //绑定至待发数据
  client.publish("LSD", msg);
  lastAlarmTime=millis();
  }
    
}

BLYNK_WRITE(V2){
  int value = param[0].asInt();
  if (value==1){
    light_sensor_flag=0;
   digitalWrite(5, HIGH);
  }else{
     light_sensor_flag=1;
     digitalWrite(5, LOW);
  }
}
BLYNK_WRITE(V7){
  String value = param[0].asString();
  Serial.println(value);
   DynamicJsonBuffer jsonBuffer;
  JsonObject& pack = jsonBuffer.createObject();
  pack["timer"] =value;
  pack.printTo(msg);   //绑定至待发数据
  client.publish("LSD", msg);
}
BLYNK_WRITE(V8){
  int value = param[0].asInt();
  Serial.println(value);
   
  if(value==1){
    DynamicJsonBuffer jsonBuffer;
  JsonObject& pack = jsonBuffer.createObject();
  pack["stop"] =value;
  pack.printTo(msg);   //绑定至待发数据
  client.publish("LSD", msg);
  }
  
}
