#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "Hotspot-ESP32";
const char* password = "password";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void connectToWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}


void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  // Konvertiere die empfangenen Daten in eine Zeichenkette
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;  // Null-terminate the string
    String message = (char*)data;
    Serial.printf("WebSocket message received: %s\n", message.c_str());
    messa
    if (message == "true") {
      digitalWrite(33,HIGH);
      // Handle the 'true' message as needed
    }
  }
}



void setup() {
  Serial.begin(115200);
  pinMode(33,OUTPUT);
  connectToWifi();
  ws.onEvent(onEvent);
  server.addHandler(&ws);
  server.begin();
}

void loop() {
  // Main code here
}





