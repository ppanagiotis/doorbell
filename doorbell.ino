#include <NTPClient.h>

// Look for all "CHANGE_ME" before uploading the code.
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>

// wifi
const char* ssid = "CHANGE_ME_WIFI_SSID"; //type your WIFI information inside the quotes
const char* password = "CHANGE_ME_WIFI_PASSWORD";
WiFiClient espClient;

#define INPUTPIN D1

// OTA
#define SENSORNAME "CHANGE_ME_SENSORNAME" //change this to whatever you want to call your device
#define OTApassword "CHANGE_ME_OTA_PASSWORD" //the password you will need to enter to upload remotely via the ArduinoIDE yourOTApassword
int OTAport = 8266;

// MQTT
const char* mqtt_server = "CHANGE_ME_MQTT_SERVER"; // IP address or dns of the mqtt
const char* mqtt_username = "CHANGE_ME_MQTT_USERNAME"; //
const char* mqtt_password = "CHANGE_ME_MQTT_PASSWORD";
const int mqtt_port = 1883; //REPLACEME, usually not?

PubSubClient client(espClient);
const char* doorbell_topic = "CHANGE_ME_MQTT_TOPIC";

void setup() {
  // Serial setup
  Serial.begin(115200);
  
  // pins setup
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LEDs  pin as an output
  pinMode(2, OUTPUT); // ^ other led
  pinMode(INPUTPIN, INPUT_PULLUP);
  
  // Wifi connection setup
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);

  // Turn OFF builtin leds
  digitalWrite(BUILTIN_LED, HIGH);
  digitalWrite(2, HIGH);

  // OTA setup
  ArduinoOTA.setHostname(SENSORNAME);
  ArduinoOTA.setPort(OTAport);
  ArduinoOTA.setPassword((const char *)OTApassword);
  ArduinoOTA.begin();
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.hostname("DoorBell");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void doorbell() {

  client.publish(doorbell_topic, "1");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(SENSORNAME, mqtt_username, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("MQTT failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  ArduinoOTA.handle();
  // Input pin is LOW run doorbell()
  if (digitalRead(INPUTPIN) == LOW) {
    doorbell();
  }
  if (!client.connected()) {
    reconnect();
  }

  client.loop();
  delay(200);
}
