#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h> 
#include <PubSubClient.h>
#include <ArduinoJson.h>
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>
///////////////////////初始化/////////////////////
void encodeJson();

//定义针脚
#define DHTPIN 5
#define SMOKEPIN A0
#define FANPIN 4

#define BLYNK_MAX_READBYTES 4096
//定义类型，DHT11或者其它
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

///////widget/////////////
WidgetLED fan_led(V3);
//BlynkTimer timer;

///////widget/////////////

/////////////////////API & NTP //////////////////
String privateKey = "SSMOD7i7sMQYkBPXi";
String requireCity = "hangzhou";
String reqUnit = "c";
const char* host = "api.seniverse.com";
String reqRes = "https://api.seniverse.com/v3/weather/now.json?key=SSMOD7i7sMQYkBPXi&location=hangzhou&language=zh-Hans&unit=c" ; 

//建立http请求信息
String httpRequest = String("GET ") + reqRes + " HTTP/1.1\r\n" + 
                              "Host: " + host + "\r\n" + 
                              "Connection: close\r\n\r\n";

///////////////////////全局变量/////////////////////
double humidity,temperature;
int smoke;
String strHumidity,strTemperature,strSmoke;
char msgHumTem[80];   //发送的数据
char msgWeather[100];
String response;

const char* results_0_location_name;
const char* results_0_daily_item_text_day;

int fan_control=0;
//int weather_code=0;
///////////////////客户端设置////////////////

char ssid[] = "615 vip 专线";
char pass[] = "zjsu1904615";
const char* mqtt_server = "test.ranye-iot.net";
char auth[] = "8-rU4hTX0MYtAuanvaGSXPsWivV2YgfJ";

//WiFiClient espClient;
//WiFiClient espClient2;
//PubSubClient client(espClient);
//HTTPClient httpClient;  //开始登陆 
#define MSG_BUFFER_SIZE  (80)                      

//void setup_wifi() {     
//  delay(10);
//  // We start by connecting to a WiFi network
//  Serial.println();
//  Serial.print("Connecting to ");
//  Serial.println(ssid);
//
//  WiFi.mode(WIFI_STA);
//  WiFi.begin(ssid, password);
//
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(500);
//    Serial.print(".");
//  }
//
//  randomSeed(micros());
//
//  Serial.println("");
//  Serial.println("WiFi connected");
//  Serial.println("IP address: ");
//  Serial.println(WiFi.localIP());
//}

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

////////////////////////////////JSON///////////////
//
//void encodeJson(){                //发送数据 json 编码
//  StaticJsonDocument<2000> doc;
//  doc["Humidity"] = strHumidity;
//  doc["Temperature"] = strTemperature;
////  doc["Smoke"]=strSmoke;
//  serializeJson(doc, msgHumTem);   //绑定至待发数据，全局变量 msgHumTem
//
//  
//}
//
//void encodeJsonWeather(){
//  StaticJsonDocument<2000> docWeather;
//  docWeather["name"] = results_0_location_name;
//  docWeather["text_day"] = results_0_daily_item_text_day;
//  serializeJson(docWeather, msgWeather);
//}

//////////////////////////decodeJSON////////////////
//void decodeJson(String input){
//  // Stream& input;
//StaticJsonDocument<768> doc;
//
//DeserializationError error = deserializeJson(doc, input);
//
//if (error) {
//  
//  Serial.print(F("deserializeJson() failed: "));
//  Serial.println(error.f_str());
//  return;
//}
//
//JsonObject results_0 = doc["results"][0];
//JsonObject results_0_now = results_0["now"];
//String results_0_now_code = results_0_now["code"]; // "4"
//Serial.println(results_0_now_code);
//weather_code=results_0_now_code.toInt()+1;
//
//  
//}
//////////////////////////////主体////////////////////////////////

void setup() {
  Serial.begin(9600);
  dht.begin(); //DHT开始工作
  pinMode(SMOKEPIN, INPUT);
  pinMode(FANPIN, OUTPUT);
  //Blynk
Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 31, 160), 8080);
   //连接wifi
//  setup_wifi();

  //mqtt_server初始化
//  client.setServer(mqtt_server, 1883);
//  if (!client.connected()) {
//    reconnect();
//  }
//timer.setInterval(60000L,getWeather );
}

void loop() {
  readTemHum();
  readSmoke(fan_control);
  
//  getWeather();
//  decodeJson(response);
//  encodeJson(); //包装
  Blynk.run();
//  timer.run();
  

//  Serial.println("Publish message");
//  client.publish("pms", msgHumTem);
//  client.publish("pms_weather", msgWeather);
//  Serial.println(msgHumTem);
//  Serial.println(msgWeather);
//  if (!client.connected()) {
//    reconnect();
//  }
//  client.loop();
  delay(1000);
}

//////////////////读取环境温湿度/////////////////////

void readTemHum(){
  humidity = dht.readHumidity();//读湿度
  temperature = dht.readTemperature();//读温度，默认为摄氏度
//  strHumidity = String(humidity)+"%";
//  strTemperature = String(temperature)+"℃";
//  Serial.println(humidity);
//  Serial.println(temperature);
Blynk.virtualWrite(V0,humidity);
Blynk.virtualWrite(V1,temperature);
}

///////////////////读取环境气体浓度/////////////////////

void readSmoke(int Signal){
  smoke = analogRead(SMOKEPIN);
   Blynk.virtualWrite(V2,smoke);
  if (Signal==0 && smoke < 500){
    digitalWrite(FANPIN, HIGH);
    fan_led.on();
  }
   if (Signal==0 && smoke >=500){
    digitalWrite(FANPIN, LOW);
    fan_led.off();
  }


//  strSmoke = String(smoke);
//  Serial.println("smoke:"+strSmoke);
//  Serial.println();
//  if (smoke < 600) digitalWrite(FANPIN, HIGH);
//  else digitalWrite(FANPIN, LOW);

}

////////////////////获取天气////////////////////////////
//void getWeather() {
////  httpClient.begin(espClient2,"http://api.seniverse.com"+reqRes);
////  int httpget_now = httpClient.GET(); //赋值
//////  Serial.println("httpget_now:");
//////  Serial.println(httpget_now);
////  if (httpget_now == HTTP_CODE_OK) { //检查一下是否为0，应该是去检查缓存区是否为空
//////    Serial.println("that is ok");
////    response =  httpClient.getString();
//////    Serial.println(response);
////  }
//Serial.println("weather");
//Blynk.virtualWrite(V6,reqRes);
////Blynk.virtualWrite(V7,weather_code);
//}

  BLYNK_WRITE(V4){
  int value = param[0].asInt();
  if (value==1){
       fan_control=1;
   digitalWrite(FANPIN, HIGH);
    fan_led.on();
  }
  else{
    digitalWrite(FANPIN, LOW);
    fan_led.off();
    fan_control=0;
  }
}
//BLYNK_WRITE(V6){
// 
//  Serial.println(param.asStr());
//  decodeJson(param.asStr());
//  Blynk.virtualWrite(V7,weather_code);
//}
