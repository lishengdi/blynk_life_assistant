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
#define triggerPin 5


int weather_code=0;
String reqRes = "https://api.seniverse.com/v3/weather/now.json?key=SSMOD7i7sMQYkBPXi&location=hangzhou&language=zh-Hans&unit=c" ; 
long Ctime=0;
const char* mqtt_server = "test.ranye-iot.net";
char msg[80];   //发送的数据
//WiFiClient espClient;
//PubSubClient client(espClient);
#define MSG_BUFFER_SIZE  (80)
//void decodeJson(char msg[80]);
//void encodeJson();
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "M80Fll8LT401y0aXUFuYJBVAvuw3OJNd";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "615 vip 专线";
char pass[] = "zjsu1904615";

///////widget/////////////
WidgetLED tag_led(V2);

///////widget/////////////

//void reconnect() {
//  while (!client.connected()) {
//    Serial.print("Attempting MQTT connection...");
//    String clientId = "ESP8266Client-";
//    clientId += String(random(0xffff), HEX);
//    if (client.connect(clientId.c_str())) {
//      Serial.println("connected");
//      client.subscribe("LSD");
//
//    } else {
//      Serial.print("failed, rc=");
//      Serial.print(client.state());
//      Serial.println(" try again in 5 seconds");
//      // Wait 5 seconds before retrying
//      delay(5000);
//    }
//  }
//}
//
//void callback(char* topic, byte* payload, unsigned int length) {       //关注回调函数
//  Serial.print("Message arrived [");
//  Serial.print(topic);
//  Serial.print("] ");
//  char Receive_data[80];
//
//  for (int i = 0; i < length; i++) {
//    Receive_data[i]=(char)payload[i];
//  }
//  decodeJson(Receive_data);

//}






////////////////////////////////JSON///////////////

//void encodeJson(){                //发送数据 json 编码
//  DynamicJsonBuffer jsonBuffer;
//  JsonObject& pack = jsonBuffer.createObject();
//  pack["AirTag_response"] = "played";
//  pack.printTo(msg);   //绑定至待发数据
//}
//
//
//void decodeJson(char Msg[80]){   //JSON解码函数
//  Serial.println("decoding");
//  DynamicJsonBuffer jsonBuffer;
//  JsonObject& pack = jsonBuffer.parseObject(Msg);
//  if ((int)pack["AirTag_signal"]==1){
//    Serial.println("start");
//    encodeJson();
//    client.publish("LSD", msg);
//    toFind();  
//  }
//  
//}


void setup()
{
  // Debug console
  Serial.begin(9600);
  pinMode(triggerPin,OUTPUT);
  Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 31, 160), 8080);
//  client.setServer(mqtt_server, 1883);
//  client.setCallback(callback);
//  
//  if (!client.connected()) {
//    reconnect();
//  }
Ctime = millis();
}


void loop()
{
  Blynk.run();
//  if (!client.connected()) {
//    reconnect();
//  }
//      client.loop();

getWeather();
  delay(1000);
}

void toFind(){
  int i=0;int j=0;
  tag_led.on();
  for(i=0;i<5;i++)
    {
//      client.publish("lsd", msg);
      digitalWrite(triggerPin,HIGH);
      delay(500);
      digitalWrite(triggerPin,LOW);
      delay(300);

      for(j=0;j<3;++j){
        digitalWrite(triggerPin,HIGH);
        delay(80);//wait for 2ms
        digitalWrite(triggerPin,LOW);
        delay(80);//wait for 2ms
      }
      
      delay(1000); 
    }
  tag_led.off();
    
  }

  BLYNK_WRITE(V1){
  int value = param[0].asInt();
  if (value==1){
   toFind();
  }
}
void getWeather() {
   long Now= millis();
if(Now-Ctime>=30000){
 Serial.println("weather");
Blynk.virtualWrite(V6,reqRes);
Ctime=Now;
}

//Blynk.virtualWrite(V7,weather_code);
}

BLYNK_WRITE(V6){
 
  Serial.println(param.asStr());
  decodeWeather(param.asStr());
  Blynk.virtualWrite(V7,weather_code);
}


void decodeWeather(String input){
  // Stream& input;
StaticJsonDocument<1000> doc;

DeserializationError error = deserializeJson(doc, input);

if (error) {
  
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
  return;
}

JsonObject results_0 = doc["results"][0];
JsonObject results_0_now = results_0["now"];
String results_0_now_code = results_0_now["code"]; // "4"
Serial.println(results_0_now_code);
weather_code=results_0_now_code.toInt()+1;

  
}
