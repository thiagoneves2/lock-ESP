#include <Arduino.h>
#include <Wifi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

//Net Setup
#define net_ssid "LabMicros"
#define net_password "seluspeesc@"

//MQTT Setup
#define MQTT_ID ""
#define MQTT_BROKER "igbt.eesc.usp.br"
#define MQTT_PORT 1883
#define MQTT_TOPIC1 "/mqtt/sensor_porta"
#define MQTT_USERNAME "mqtt"
#define MQTT_PASSWORD "mqtt_123_abc"
WiFiClient espClient;
PubSubClient MQTT(espClient);

char topic_str[10] = "";
long currentTime, lastTime = 0;

void setupWifi();
void setupMQTT();
void callback(char *topic, byte *payload,unsigned int length);
void topicsSubscribe();
void reconnectWifi();
void reconnectMQTT();


void setup(){

Serial.begin(115200);
setupWifi();
setupMQTT();

}

void loop() {

  if(!MQTT.connected())
      reconnectMQTT();

  if(WiFi.status() != WL_CONNECTED)
      reconnectWifi();
  
  currentTime = millis();

  if(currentTime - lastTime > 2000)
  {
    MQTT.publish(MQTT_TOPIC1,);
    lastTime = millis();
  }


}

void setupWifi() {
  //Configura conexão à rede WiFi
  if(WiFi.status() == WL_CONNECTED) 
    return;
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(net_ssid);

  WiFi.begin(net_ssid,net_password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void  setupMQTT() {
  //Configura a conexão MQTT
  MQTT.setServer(MQTT_BROKER,MQTT_PORT);
  MQTT.setCallback(callback);

  while (!MQTT.connected())
  {
    Serial.print("Tentando se conectar ao Broker MQTT ");
    Serial.println(MQTT_BROKER);

    if(MQTT.connect(MQTT_ID,MQTT_USERNAME,MQTT_PASSWORD))
    {
      Serial.println("Conectado com sucesso ao Broker MQTT");
      topicsSubscribe();
    }
    else
    {
      Serial.println("Falha ao conectar ao Broker");
      Serial.println("Nova tentativa em 2s");
      delay(2000);
    }
  }
}

void callback(char *topic, byte * payload, unsigned int length){
  Serial.print("Mensagem recebida no topico: ");
  Serial.println(topic);
  Serial.print("Mensagem: ");
  for(unsigned int i = 0; i < length; i++)
  {
    Serial.print((char) payload[i]);
  }
  Serial.println();
  Serial.print("-----------------------");
  Serial.println();

  if(String(topic) == MQTT_TOPIC1)
  {
	  Serial.print("Tópico recebido: ");
    Serial.print(MQTT_TOPIC1);
    Serial.println();
  }

}

void topicsSubscribe(){
	MQTT.subscribe(MQTT_TOPIC1);
}

void reconnectWifi(){
  while (WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin(net_ssid,net_password);
    delay(500);
    Serial.println(".");
  }
  Serial.println(" ");
  Serial.println("Conectado a: ");
  Serial.println(net_ssid);
}

void reconnectMQTT(){
  while (!MQTT.connected())
  {
    Serial.print("Tentando se conectar ao Broker MQTT ");
    Serial.println(MQTT_BROKER);

    if(MQTT.connect(MQTT_ID,MQTT_USERNAME,MQTT_PASSWORD))
    {
      Serial.println("Conectado com sucesso ao Broker MQTT");
      topicsSubscribe();
    }
    else
    {
      Serial.println("Falha ao conectar ao Broker");
      Serial.println("Nova tentativa em 2s");
      delay(2000);
    }
}