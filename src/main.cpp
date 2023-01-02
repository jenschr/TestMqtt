// This example uses an ESP32 Development Board
// to connect to shiftr.io.
//
// You can check on your device after a successful
// connection here: https://www.shiftr.io/try.
//
// by Joël Gähwiler
// https://github.com/256dpi/arduino-mqtt

#include <WiFi.h>
#include <MQTT.h>

const char ssid[] = "ssid";
const char pass[] = "pass";
const char mqtt_username[] = "YourUserName";
const char mqtt_password[] = "YourPassword";
const char mqtt_server[]   = "mqtt_server_address";
String yourPersonalTopic;

WiFiClient networkClient;
MQTTClient mqttClient;

unsigned long lastMillis = 0;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  String clientId = "ESP8266Client-"; // Create a random client ID
    clientId += String(random(0xffff), HEX);

  Serial.print("\nConnecting to Wifi...");
  while (!mqttClient.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
    Serial.print(".");
    delay(1000);
  }

  yourPersonalTopic = "students/"; // Create a topic path based on your username
  yourPersonalTopic += String(mqtt_username);
  Serial.print("\nConnected to Wifi! Setting up Subscription to the topic: ");
  Serial.println( yourPersonalTopic );

  mqttClient.subscribe( yourPersonalTopic.c_str() );
}

// Note: Do not publish, subscribe or unsubscribe in this method
void messageReceived(String &topic, String &payload) {
  Serial.println("incoming message: " + topic + " - " + payload);
}

void setup() {
  Serial.begin(115200);
  delay(500);

  WiFi.begin(ssid, pass);

  mqttClient.begin(mqtt_server, networkClient);
  mqttClient.onMessage(messageReceived);

  connect();
}

void loop() {
  mqttClient.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!mqttClient.connected()) {
    connect();
  }

  // publish a message every 5 second.
  if (millis() - lastMillis > 5000) {
    lastMillis = millis();
    mqttClient.publish(yourPersonalTopic.c_str(), "hello");
  }
}