#include <ESP8266WiFi.h>
#include <PubSubClient.h>
const char *ssid = "xxxx";  // Nhap ten Wifi
const char *password = "xxxx";  // Nhap mat khau Wifi

const char *mqtt_broker = "xxxx"; // Nhap dia chi broker
const char *sensorStatus = "moisture";
const char *pump = "pump";
const int mqtt_port = 1883;
WiFiClient espClient;
PubSubClient client(espClient);

#define sensor A0
#define waterPump D3

int sensor_value_last;

void callback(char *topic, byte *payload, unsigned int length) {
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("Message:");
 
 for (int i = 0; i < length; i++) {
  Serial.print((char) payload[i]);
 }
 Serial.println();
 Serial.println(" - - - - - - - - - - - -");
 digitalWrite(waterPump, LOW);
 delay(5000);
 digitalWrite(waterPump, HIGH);
}

void mqttConnect(){
  WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting to WiFi..");
    }
  
  Serial.println("Connected to the WiFi network");
  
  //connecting to a mqtt broker
  
}

void reconnect() {
  while (!client.connected()) {
    String client_id = "esp8266-client-";
    client_id += String(WiFi.macAddress());
  
    Serial.printf("The client %s connects to mosquitto mqtt broker\n", client_id.c_str());
  
    if (client.connect(client_id.c_str())) {
      Serial.println("Public emqx mqtt broker connected");
      client.subscribe("pump");
    } else {
    Serial.print("failed with state ");
    Serial.print(client.state());
    delay(2000);
    }
  }
}

void publish_status() {
  // publish status to MQTT server
  client.publish(sensorStatus, get_status().c_str());
}

String get_status() {
  // puts together status in JSON format
  String status_string;
  status_string = "{\"deviceId\":\"Device1\",\"moisture\":" + String(sensor_value_last) + "}";
  return status_string;
}

void setup() {
  Serial.begin(9600);
  pinMode(waterPump, OUTPUT);
  digitalWrite(waterPump, HIGH);
  mqttConnect();
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  // client.subscribe(pump);
}


//Get the soil moisture values
int soilMoistureSensor() {
  int value = analogRead(sensor);
  value = map(value, 0, 1024, 0, 100);
  value = (value - 100) * -1;
  return value;
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  sensor_value_last = soilMoistureSensor();
  Serial.println("Moisture: " + String(sensor_value_last));
  publish_status();
  delay(2000);
  client.loop();
}

