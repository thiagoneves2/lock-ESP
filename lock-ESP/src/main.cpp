#include <Arduino.h>
#include <Wifi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

//Net Setup
char net_ssid[] = "LabMicros";
char net_password[] = "seluspeesc@";

//MQTT Setup
#define MQTT_ID "12345"
#define MQTT_BROKER "igbt.eesc.usp.br"
#define MQTT_PORT 1883
#define MQTT_TOPIC1 "/smartlock/sensor_porta"
#define MQTT_TOPIC2 "/smartlock/desbloqueio_porta"
#define MQTT_USERNAME "mqtt"
#define MQTT_PASSWORD "mqtt_123_abc"
#define pinSensorPorta 17
#define pinRele 13
#define id_porta1 
#define id_porta2
WiFiClient espClient;
PubSubClient MQTT(espClient);

long currentTime, lastTime = 0;
bool statusPorta;

void setupWifi();
void setupMQTT();
void callback(char *topic, byte *payload,unsigned int length);
void topicsSubscribe();
void reconnectWifi();
void reconnectMQTT();

void setup(){

Serial.begin(9600);
setupWifi();
setupMQTT();

pinMode(pinRele, INPUT);
pinMode(pinSensorPorta, INPUT);

}

void loop() {

  JsonDocument doc;
  char output[80];

  if(!MQTT.connected())
      reconnectMQTT();

  if(WiFi.status() != WL_CONNECTED)
      reconnectWifi();
  
  currentTime = millis();

  if(currentTime - lastTime > 2000)
  {
    statusPorta = digitalRead(pinSensorPorta);
    if (statusPorta == HIGH)
    {
      doc["statusporta"] = "Aberta";
    }
    else 
    {
      doc["statusporta"] = "Fechada";
    }
    serializeJson(doc,output);
    //char output[] = "Enviado via MQTT";
    MQTT.publish(MQTT_TOPIC1,output);
    lastTime = millis();
  }

  //Serial.println("ESP em funcionamento");
  MQTT.loop();
  delay(500);

}

void setupWifi() {
  //Configura conexão à rede WiFi
  Serial.print("Conectando a ");
  Serial.println(net_ssid);
  if(WiFi.status() == WL_CONNECTED) 
      return;

  WiFi.begin(net_ssid,net_password);
  Serial.print("Conectando a ");
  Serial.println(net_ssid);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println(".");
  }

  Serial.println(" ");
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
  /*Serial.print("Mensagem recebida no topico: ");
  Serial.println(topic);
  Serial.print("Mensagem: ");
  for(unsigned int i = 0; i < length; i++)
  {
    Serial.print((char) payload[i]);
  }
  Serial.println(" ");
  Serial.print("-----------------------");
  Serial.println(" ");*/

  JsonDocument doc1;
  char input[80];
  char comando[15];
  int id_porta;
  int id_esp;


  if(String(topic) == MQTT_TOPIC2)
  {
    deserializeJson(doc1,payload);
    id_porta = doc1["id"]; 
    id_esp = doc1["id_esp"];
    if(id_porta == 123 && id_esp == 456)
    {
      //if(doc1["comando"] == "Abrir")
      //{
        pinMode(pinRele,OUTPUT);
        digitalWrite(pinRele,HIGH);
	      Serial.print("Porta ");
        Serial.print(id_porta);
        Serial.println(" desbloqueada");
     }
      //if(doc1["comando"] == "Fechar")
      else
      {
        pinMode(pinRele,INPUT);
	      Serial.print("Porta ");
        Serial.print(id_porta);
        Serial.println(" bloqueada");
      }
   }
}

void topicsSubscribe(){
  if(MQTT.subscribe(MQTT_TOPIC1))
  {
    Serial.print("Inscrição bem sucedida no tópico: ");
    Serial.println(MQTT_TOPIC1);
  }
  if(MQTT.subscribe(MQTT_TOPIC2))
  {
    Serial.print("Inscrição bem sucedida no tópico: ");
    Serial.println(MQTT_TOPIC2);
  }
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
}
