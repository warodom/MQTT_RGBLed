#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#define R_LED   13
#define G_LED   12
#define B_LED   14
#define Dim_LED 16
//#define CommonA

const char* ssid = "Pi_AP";
const char* password = "Raspberry"; 
char* server = "192.168.42.1";

String red_s,green_s,blue_s,dim_s;
char R_Set,G_Set,B_Set,Dim_Set,PWM_Count;

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  int i = 0; 
  int select_color = 0;
  char message_buff[100];
  
  for(i = 0; i < length; i++) { message_buff[i] = payload[i]; }
  message_buff[i] = '\0';
  
  String msgString = String(message_buff);
  
  Serial.print("Subscribe "); Serial.write(topic); Serial.print(" : "); Serial.println(msgString);

  if(String(topic) == "color") {
    for(i = 0; i < (sizeof(msgString) - 1); i++) {
      if(msgString[i] != ',') {
        switch(select_color) {
          case 0: red_s = red_s + msgString[i]; break;
          case 1: green_s = green_s + msgString[i]; break;
          case 2: blue_s = blue_s + msgString[i]; break;
        }
      }
      else select_color++;
    }
    R_Set = red_s.toInt(); G_Set = green_s.toInt(); B_Set  = blue_s.toInt();
    Serial.print("Red : "); Serial.println(int(R_Set));
    Serial.print("Green : "); Serial.println(int(G_Set));
    Serial.print("Blue : "); Serial.println(int(B_Set));
    red_s = ""; green_s = ""; blue_s = "";
  }
  else if(String(topic) == "dimmer") {
    for(i = 0; i < (sizeof(msgString) - 1); i++) {
          dim_s = dim_s + msgString[i];
    }
    Dim_Set = dim_s.toInt();
    Serial.print("Dimmer : "); Serial.println(int(Dim_Set)); dim_s = ""; 
  }
} 

WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);
 
void setup()
{
  // Setup console
  pinMode(Dim_LED, OUTPUT); pinMode(R_LED, OUTPUT); 
  pinMode(G_LED, OUTPUT); pinMode(B_LED, OUTPUT); 

  #ifdef CommonA
    digitalWrite(Dim_LED, HIGH); digitalWrite(R_LED, HIGH);
    digitalWrite(G_LED, HIGH); digitalWrite(B_LED, HIGH);
  #else
    digitalWrite(Dim_LED, LOW); digitalWrite(R_LED, LOW);
    digitalWrite(G_LED, LOW); digitalWrite(B_LED, LOW);
  #endif
  
  Serial.begin(115200); delay(10); Serial.println();
  WiFi.begin(ssid, password);
 
  int retries = 0;
  while ((WiFi.status() != WL_CONNECTED) && (retries < 10)) {
    retries++; delay(500); Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(""); Serial.println("WiFi connected");
  }
 
  if (client.connect("192.168.42.1")) {
    client.subscribe("color"); client.subscribe("dimmer");
  }
}
 
void loop()
{
  #ifdef CommonA
    if(PWM_Count < R_Set) digitalWrite(R_LED, LOW);
    else digitalWrite(R_LED, HIGH);
    if(PWM_Count < G_Set) digitalWrite(G_LED, LOW);
    else digitalWrite(G_LED, HIGH);
    if(PWM_Count < B_Set) digitalWrite(B_LED, LOW);
    else digitalWrite(B_LED, HIGH);
    if(PWM_Count < Dim_Set) digitalWrite(Dim_LED, LOW);
    else digitalWrite(Dim_LED, HIGH);
  #else
    if(PWM_Count < R_Set) digitalWrite(R_LED, HIGH);
    else digitalWrite(R_LED, LOW);
    if(PWM_Count < G_Set) digitalWrite(G_LED, HIGH);
    else digitalWrite(G_LED, LOW);
    if(PWM_Count < B_Set) digitalWrite(B_LED, HIGH);
    else digitalWrite(B_LED, LOW);
    if(PWM_Count < Dim_Set) digitalWrite(Dim_LED, HIGH);
    else digitalWrite(Dim_LED, LOW);
  #endif
  PWM_Count++; client.loop();
}
