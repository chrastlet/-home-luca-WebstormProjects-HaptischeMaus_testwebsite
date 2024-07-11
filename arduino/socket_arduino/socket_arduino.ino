#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "ArduinoJson.h"

const char* ssid = "Hotspot-ESP32";
const char* password = "password";

const int VIBRATORS[5] = {21, 17, 16, 19, 18};
const int unten = 16;
const int rechts = 18;
const int links = 19;
const int mitte = 17;
const int oben = 21;

uint8_t pwmValue = 255; // Set to full power

int sliderValue = 0;  // Variable to store the slider value

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void setup() {
    Serial.begin(9600);  // Initialize serial communication
    // Initialize all vibrator pins as outputs and set them to LOW
    for(uint8_t i = 0; i < 5; i++) {
        Serial.println(VIBRATORS[i]);
        pinMode(VIBRATORS[i], OUTPUT);
    }



    connectToWifi();
    ws.onEvent(onEvent);
    server.addHandler(&ws);
    server.begin();
}

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

// Funktion zur Verarbeitung der WebSocket-Nachricht
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;  // Null-terminate the string
        String message = (char*)data;

        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, message);

        if (error) {
            Serial.printf("Fehler beim Parsen der JSON-Nachricht: %s\n", error.c_str());
            return;
        }

        const char* msg = doc["message"];
        Serial.println(msg);
        if (strcmp(msg, "startdrag") == 0) {
            startDrag();
         
        } else if (strcmp(msg, "drop") == 0) {
            drop();
        } else if (strcmp(msg, "button") == 0) {
            clickableButtonPattern();
        } else if (strcmp(msg, "link") == 0) {
            clickableLinkPattern();
        } else if (strcmp(msg, "text-input") == 0) {
            textInputPattern();
        } else if (strcmp(msg, "textarea") == 0) {
            textInputPattern();
        } else if (strcmp(msg, "checkbox") == 0) {
            checkboxPattern();
        } else if (strcmp(msg, "radio") == 0) {
            radioButtonPattern();
        } else if (strcmp(msg, "select_click") == 0) {
            selectClickPattern();
        } else if (strcmp(msg, "select_change") == 0) {
            JsonArray dataArray = doc["data"];
            int data[2];
            for (size_t i = 0; i < dataArray.size() && i < 2; ++i) {
                data[i] = dataArray[i];
            }
            selectChangePattern(data[0], data[1]);
        } else if (strcmp(msg, "range") == 0) {
            int data = doc["data"];
            sliderPattern(data);
        } else if (strcmp(msg, "submit") == 0) {
            submitPattern();
        } else if (strcmp(msg, "endVertical") == 0) {
            // Code für endVertical
        } else if (strcmp(msg, "endHorizontal") == 0) {
            // Code für endHorizontal
        } else if(strcmp(msg, "zoomIn") == 0) {
            zoomInPattern();
        }else if(strcmp(msg, "zoomOut") == 0) {
            zoomOutPattern();
         }
         else if(strcmp(msg, "context")==0){
            contextMenuPattern();
         }
         else if(strcmp(msg, "hover")==0){
            hoverPattern();
         }
          else if(strcmp(msg, "tooltip")==0){
             tooltipPattern();
          }
        else {
            // Code für den Fall, dass kein anderer Fall zutrifft
        }

        Serial.printf("WebSocket JSON message received: message=%s\n", msg);
    }
}

void startDrag() {
    for (uint8_t i = 0; i < sizeof(VIBRATORS); i++) {
        analogWrite(VIBRATORS[i], pwmValue);
    }
    delay(100);
    for (uint8_t i = 0; i < sizeof(VIBRATORS); i++) {
        analogWrite(VIBRATORS[i], 0);
    }
}

void drop() {
    for (uint8_t i = 0; i < sizeof(VIBRATORS); i++) {
        analogWrite(VIBRATORS[i], pwmValue);
    }
    delay(100);
    for (uint8_t i = 0; i < sizeof(VIBRATORS); i++) {
        analogWrite(VIBRATORS[i], 0);
    }
}

void selectClickPattern() {
    for (uint8_t i = 0; i < sizeof(VIBRATORS); i++) {
        analogWrite(VIBRATORS[i], pwmValue);
    }
    delay(100);
    for (uint8_t i = 0; i < sizeof(VIBRATORS); i++) {
        analogWrite(VIBRATORS[i], 0);
    }
}

void selectChangePattern(int cur, int max) {
    for (uint8_t i = 0; i < sizeof(VIBRATORS); i++) {
        analogWrite(VIBRATORS[i], pwmValue);
    }
    delay(100);
    for (uint8_t i = 0; i < sizeof(VIBRATORS); i++) {
        analogWrite(VIBRATORS[i], 0);
    }
}

void smoothTransition(int motor1, int motor2, int dauer) {
  int stepDelay = dauer / 255; // Berechnung der Verzögerung pro Schritt als Ganzzahl
  unsigned long previousMillis = millis();

  for (int i = 0; i <= 255; i++) {
    while (millis() - previousMillis < stepDelay) {
      // Nichts tun, nur auf die nächste Iteration warten
      // Dies verhindert eine Blockierung der CPU
      Serial.print(".");
    }
    previousMillis = millis();
    
    analogWrite(motor1, 255 - i);
    analogWrite(motor2, i);

  }
}


void clickableButtonPattern() {
    for (uint8_t i = 0; i < sizeof(VIBRATORS); i++) {
        analogWrite(VIBRATORS[i], pwmValue);
    }
    delay(100);
    for (uint8_t i = 0; i < sizeof(VIBRATORS); i++) {
        analogWrite(VIBRATORS[i], 0);
    }
}

void clickableLinkPattern() {
    for (int i = sizeof(VIBRATORS) - 1; i >= 0; i--) {
        analogWrite(VIBRATORS[i], pwmValue);
        delay(50);
        analogWrite(VIBRATORS[i], 0);
    }
}

void textInputPattern() {
    for (int i = 0; i < 3; i++) {
        analogWrite(mitte, pwmValue);
        delay(30);
        analogWrite(mitte, 0);
        delay(30);
    }
}

void Waitcursor() {
    for (int i = sizeof(VIBRATORS) - 1; i >= 0; i--) {
        analogWrite(VIBRATORS[i], pwmValue);
        delay(100);
        analogWrite(VIBRATORS[i], 0);
    }
}

void PointerCursor() {
    for (int i = sizeof(VIBRATORS) - 1; i >= 0; i--) {
        analogWrite(VIBRATORS[i], pwmValue);
        delay(100);
        analogWrite(VIBRATORS[i], 0);
    }
}

void CrosshairCursor() {
    for (int i = sizeof(VIBRATORS) - 1; i >= 0; i--) {
        analogWrite(VIBRATORS[i], pwmValue);
        delay(100);
        analogWrite(VIBRATORS[i], 0);
    }
}

void TextCursor() {
    for (int i = sizeof(VIBRATORS) - 1; i >= 0; i--) {
        analogWrite(VIBRATORS[i], pwmValue);
        delay(100);
        analogWrite(VIBRATORS[i], 0);
    }
}

void MoveCursor() {
    for (int i = sizeof(VIBRATORS) - 1; i >= 0; i--) {
        analogWrite(VIBRATORS[i], pwmValue);
        delay(100);
        analogWrite(VIBRATORS[i], 0);
    }
}

void checkboxPattern() {
  
 
}

void SliderFeedback(int value) {
    int leftIntensity = 0;
    int midIntensity = 0;
    int rightIntensity = 0;

    if (value <= 50) {
        leftIntensity = map(value, 0, 50, pwmValue, 0);
        midIntensity = map(value, 0, 50, 0, pwmValue);
    } else {
        midIntensity = map(value, 50, 100, pwmValue, 0);
        rightIntensity = map(value, 50, 100, 0, pwmValue);
    }

    analogWrite(VIBRATORS[0], leftIntensity);
    analogWrite(VIBRATORS[1], midIntensity);
    analogWrite(VIBRATORS[2], rightIntensity);
}

void DragVibration() {
    for (int i = 0; i < sizeof(VIBRATORS); i++) {
        analogWrite(VIBRATORS[i], pwmValue);
    }
    delay(100);
    for (int i = 0; i < sizeof(VIBRATORS); i++) {
        analogWrite(VIBRATORS[i], 0);
    }
    delay(100);
}

void DropVibration() {
    for (int i = 0; i < sizeof(VIBRATORS); i++) {
        analogWrite(VIBRATORS[i], pwmValue);
    }
    delay(200);
    for (int i = 0; i < sizeof(VIBRATORS); i++) {
        analogWrite(VIBRATORS[i], 0);
    }
}

void ScrollMovement() {
    for (int i = 0; i < sizeof(VIBRATORS); i++) {
        analogWrite(VIBRATORS[i], pwmValue);
        delay(50);
        analogWrite(VIBRATORS[i], 0);
        delay(50);
    }
    for (int i = sizeof(VIBRATORS) - 1; i >= 0; i--) {
        analogWrite(VIBRATORS[i], pwmValue);
        delay(50);
        analogWrite(VIBRATORS[i], 0);
        delay(50);
    }
}

void StrongVibration(int direction) {
    if (direction == 1) {
        for (int i = 0; i < sizeof(VIBRATORS); i++) {
            analogWrite(VIBRATORS[i], pwmValue);
            delay(100);
            analogWrite(VIBRATORS[i], 0);
        }
    } else if (direction == -1) {
        for (int i = sizeof(VIBRATORS) - 1; i >= 0; i--) {
            analogWrite(VIBRATORS[i], pwmValue);
            delay(100);
            analogWrite(VIBRATORS[i], 0);
        }
    }
}

void radioButtonPattern() {
  smoothTransition(links, oben, 100);
  smoothTransition(oben, rechts, 100);
  smoothTransition(rechts, unten, 100);
  smoothTransition(unten, links, 100);
}

void dropdownPattern() {
    for (int i = 0; i < sizeof(VIBRATORS); i++) {
        analogWrite(VIBRATORS[i], pwmValue);
        delay(50);
        analogWrite(VIBRATORS[i], 0);
    }
}

void sliderPattern(int value) {
    if(value<33){
      analogWrite(links, pwmValue);

    }
    if(value>33 && value<66){
      analogWrite(mitte, pwmValue);

    }
    if(value>66 && value<100){
      analogWrite(rechts, pmwValue);
    }
}

void submitPattern() {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < sizeof(VIBRATORS); j++) {
            analogWrite(VIBRATORS[j], pwmValue);
        }
        delay(i == 0 ? 100 : 300);
        for (int j = 0; j < sizeof(VIBRATORS); j++) {
            analogWrite(VIBRATORS[j], 0);
        }
        delay(100);
    }
}

void scrollPattern() {
    for (int i = 0; i < sizeof(VIBRATORS); i++) {
        analogWrite(VIBRATORS[i], pwmValue);
        delay(50);
        analogWrite(VIBRATORS[i], 0);
    }
}

void endScrollPattern() {
    analogWrite(VIBRATORS[4], pwmValue);
    delay(300);
    analogWrite(VIBRATORS[4], 0);
}

void contextMenuPattern() {
    analogWrite(links, pwmValue);
    delay(500);
    analogWrite(links, 0);
    delay(500);

    analogWrite(mitte, pwmValue);
    delay(500);
    analogWrite(mitte, 0);
    delay(500);

    analogWrite(rechts, pwmValue);
    delay(500);
    analogWrite(mitte, 0);
    delay(500);
}

void tooltipPattern() {
    int obenIntensity = pwmValue;
    int transitionThreshold = 120;

    while (obenIntensity >= 0) {
        analogWrite(oben, obenIntensity);

        if (obenIntensity <= transitionThreshold) {
            int sidesIntensity = pwmValue - obenIntensity;
            analogWrite(mitte, sidesIntensity);
            analogWrite(links, sidesIntensity);
            analogWrite(rechts, sidesIntensity);
            analogWrite(unten, sidesIntensity);
        }

        obenIntensity--;
        delay(5);
    }

    analogWrite(oben, 0);
    analogWrite(mitte, 0);
    analogWrite(links, 0);
    analogWrite(rechts, 0);
    analogWrite(unten, 0);
}

void dragOver(int x, int y, int max_x, int max_y) {
    float l = pwmValue;
    float o = pwmValue;
    float u = pwmValue;
    float r = pwmValue;
    float m = pwmValue;

    float norm_x = (float)x / max_x;
    float norm_y = (float)y / max_y;

    float center_x = 0.5;
    float center_y = 0.5;

    float dist_to_center = sqrt(pow(norm_x - center_x, 2) + pow(norm_y - center_y, 2));
    float max_dist = sqrt(0.5 * 0.5 + 0.5 * 0.5);
    float normalized_dist = dist_to_center / max_dist;
    float inverted_value = (1 - normalized_dist);

    m = m * inverted_value;
    l = l * norm_x;
    r = r * (1 - norm_x);
    o = o * norm_y;
    u = u * (1 - norm_y);

    analogWrite(links, l);
    analogWrite(rechts, r);
    analogWrite(oben, o);
    analogWrite(unten, u);
    analogWrite(mitte, m);
    delay(200);

    Serial.print("Left: ");
    Serial.print(l);
    Serial.print(", Right: ");
    Serial.print(r);
    Serial.print(", Top: ");
    Serial.print(o);
    Serial.print(", Bottom: ");
    Serial.print(u);
    Serial.print(", Middle: ");
    Serial.println(m);
}

void hoverPattern() {
    analogWrite(links, pwmValue);
    analogWrite(rechts, pwmValue);
    analogWrite(oben, pwmValue);
    analogWrite(unten, pwmValue);
    analogWrite(mitte, pwmValue);
    delay(200);
    analogWrite(links, 0);
    analogWrite(rechts, 0);
    analogWrite(oben, 0);
    analogWrite(unten, 0);
    analogWrite(mitte, 0);
}

void grabPattern() {
    int stepDelay = 5;
    int intensity = 1;
    int innen = pwmValue;
    int außen = 100;

    while (innen > 0 && außen < pwmValue) {
        innen -= intensity;
        außen += intensity;
        analogWrite(oben, außen);
        analogWrite(unten, außen);
        analogWrite(links, außen);
        analogWrite(rechts, außen);
        analogWrite(mitte, innen);
        delay(stepDelay);
    }
    analogWrite(oben, 0);
    analogWrite(unten, 0);
    analogWrite(links, 0);
    analogWrite(rechts, 0);
}

void noDropPattern() {
    analogWrite(oben, pwmValue);
    analogWrite(unten, pwmValue);
    analogWrite(links, pwmValue);
    analogWrite(rechts, pwmValue);
    analogWrite(mitte, pwmValue);

    delay(1000);

    analogWrite(oben, 0);
    analogWrite(unten, 0);
    analogWrite(links, 0);
    analogWrite(rechts, 0);
    analogWrite(mitte, 0);
}

void zoomInPattern() {
    int stepDelay = 10;
    int intensity = 1;
    int innen = pwmValue;
    int außen = 100;

    while (innen > 0 && außen < pwmValue) {
        innen -= intensity;
        außen += intensity;
        analogWrite(oben, außen);
        analogWrite(unten, außen);
        analogWrite(links, außen);
        analogWrite(rechts, außen);
        analogWrite(mitte, innen);
        delay(stepDelay);
    }
    analogWrite(oben, 0);
    analogWrite(unten, 0);
    analogWrite(links, 0);
    analogWrite(rechts, 0);
}

void zoomOutPattern() {
    int stepDelay = 10;
    int intensity = 1;
    int innen = 0;
    int außen = pwmValue;

    while (innen < pwmValue && außen > 0) {
        innen += intensity;
        außen -= intensity;
        analogWrite(oben, außen);
        analogWrite(unten, außen);
        analogWrite(links, außen);
        analogWrite(rechts, außen);
        analogWrite(mitte, innen);
        delay(stepDelay);

        // Reset the watchdog timer to prevent it from timing out
    }
    analogWrite(oben, 0);
    analogWrite(unten, 0);
    analogWrite(links, 0);
    analogWrite(rechts, 0);
}


void waitPattern() {
    int stepDelay = 100;
    int maxIntensity = pwmValue;
    int minIntensity = 0;
    int steps = 20;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < steps; j++) {
            int intensity = (maxIntensity * j) / steps;

            analogWrite(oben, intensity);
            delay(stepDelay);
            analogWrite(oben, minIntensity);

            analogWrite(rechts, intensity);
            delay(stepDelay);
            analogWrite(rechts, minIntensity);

            analogWrite(unten, intensity);
            delay(stepDelay);
            analogWrite(unten, minIntensity);

            analogWrite(links, intensity);
            delay(stepDelay);
            analogWrite(links, minIntensity);
        }
    }
}

void notAllowedPattern() {
    int pulseDuration = 500;
    int intensity = pwmValue;

    analogWrite(oben, intensity);
    analogWrite(unten, intensity);
    analogWrite(links, intensity);
    analogWrite(rechts, intensity);
    analogWrite(mitte, intensity);

    delay(pulseDuration);

    analogWrite(oben, 0);
    analogWrite(unten, 0);
    analogWrite(links, 0);
    analogWrite(rechts, 0);
    analogWrite(mitte, 0);
}

void loop() {
  // Main code here
}