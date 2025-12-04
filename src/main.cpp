#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// ---------- CONFIGURACIÓN DE RED -----------

const char* ssid = "MOVISTAR-WIFI6-EA00";        // WIFI en tu casa
const char* password = "LgBzNhkfyxJ8v6HKHohx";

const char* mqtt_server = "192.168.1.50"; // IP de tu Raspberry Pi en casa (cámbiala por la tuya)

// Credenciales MQTT (las mismas en casa y en el club)
const char* mqtt_user = "iotus";
const char* mqtt_pass = "iotus@pi";

// ---------- OBJETOS MQTT -----------

WiFiClient espClient;
PubSubClient client(espClient);

String device_id = "esp8266_test_01";

// ---------- FUNCIÓN: CONECTAR A MQTT -----------

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando a MQTT... ");
    if (client.connect(device_id.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("Conectado!");

      // Publicamos mensaje online
      client.publish(("homeassistant/sensor/" + device_id + "/state").c_str(), "online");
    } else {
      Serial.print("Fallo, rc=");
      Serial.print(client.state());
      Serial.println(" reintentando en 3 segundos...");
      delay(3000);
    }
  }
}

// ---------- FUNCIÓN: CONFIGURACIÓN DISCOVERY -----------

void sendDiscovery() {

  // Topic de configuración para Home Assistant
  String config_topic = "homeassistant/sensor/" + device_id + "/config";

  // JSON con el sensor
  String payload = "{";
  payload += "\"name\": \"ESP8266 Test Temperature\",";
  payload += "\"state_topic\": \"homeassistant/sensor/" + device_id + "/temperature\",";
  payload += "\"unit_of_measurement\": \"°C\",";
  payload += "\"device\": {\"identifiers\":[\"" + device_id + "\"], \"name\": \"ESP8266 Test\"}";
  payload += "}";

  client.publish(config_topic.c_str(), payload.c_str(), true);
}

// ---------- SETUP -----------

void setup() {
  Serial.begin(115200);

  // Conectar al WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");

  client.setServer(mqtt_server, 1883);
}

// ---------- LOOP PRINCIPAL -----------

void loop() {
  if (!client.connected()) {
    reconnect();
    sendDiscovery();
  }
  client.loop();

  // Valor de temperatura simulado
  float temp = random(200, 300) / 10.0; // 20.0 - 30.0 ºC

  // Enviamos la temperatura al topic
  String temp_topic = "homeassistant/sensor/" + device_id + "/temperature";
  String temp_str = String(temp);

  client.publish(temp_topic.c_str(), temp_str.c_str());

  Serial.println("Temperatura enviada: " + temp_str + " ºC");

  delay(5000);
}


//prueba de rama
