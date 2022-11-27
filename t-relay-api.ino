#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define RELAY_NB 4

const uint8_t relayPin[RELAY_NB] = {21, 19, 18, 5};
const uint8_t ledPin = 25;

const char *ssid = ""; // SSID
const char *password = ""; // Password

AsyncWebServer server(80);

void initRelays() {
  pinMode(relayPin[0], OUTPUT);
  pinMode(relayPin[1], OUTPUT);
  pinMode(relayPin[2], OUTPUT);
  pinMode(relayPin[3], OUTPUT);
  pinMode(ledPin, OUTPUT);
  delay(100);
  digitalWrite(relayPin[0], LOW);
  digitalWrite(relayPin[1], LOW);
  digitalWrite(relayPin[2], LOW);
  digitalWrite(relayPin[3], LOW);
  digitalWrite(ledPin, LOW);
}

uint8_t relayState(uint8_t id) {
  return digitalRead(relayPin[id]);
}

void relayState(uint8_t id, uint8_t state) {
  digitalWrite(relayPin[id], state);
}

void indexEndpoint(AsyncWebServerRequest *req) {
  req->send(200, "application/json", "{\"message\":\"T-Relay API\"}");
}

void notFoundEndpoint(AsyncWebServerRequest *req) {
  req->send(404, "application/json", "{\"message\":\"Not found\"}");
}

void statusEndpoint(AsyncWebServerRequest *req) {
  DynamicJsonDocument res(128);
  for(int i=0; i<RELAY_NB; i++) {
    String relay = "relay" + String(i+1);
    String state = relayState(i) == HIGH ? "up" : "down";
    res[relay] = state;
  }
  req->send(200, "application/json", res.as<String>());
}

void actionEndpoint(AsyncWebServerRequest *req) {
  String relay = req->pathArg(0);
  String action = req->pathArg(1);
  uint8_t relayId = relay.toInt() - 1;

  switch(action[0]) {
    case 'u':
      relayState(relayId, HIGH);
      break;
    case 'd':
      relayState(relayId, LOW);
      break;
    case 't':
      relayState(relayId) == LOW ? relayState(relayId, HIGH) : relayState(relayId, LOW);
      break;
  }

  DynamicJsonDocument res(32);
  res["relay" + relay] = relayState(relayId) == HIGH ? "up" : "down";
  req->send(200, "application/json", res.as<String>());
}

void setup(){
  Serial.begin(115200);
  initRelays();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("WiFi setup failed!\n");
    return;
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, indexEndpoint);
  server.on("/relays", HTTP_GET, statusEndpoint);
  server.on("^/relays/([1-4])/(up|down|toggle)$", HTTP_PUT, actionEndpoint);
  server.onNotFound(notFoundEndpoint);
  server.begin();
}

void loop() {}
