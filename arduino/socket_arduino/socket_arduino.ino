#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "ArduinoJson.h"

const char* ssid = "Hotspot-ESP32";
const char* password = "password";

const int VIBRATORS[5] = {13, 14, 33, 15, 32};
const int unten = 32;
const int rechts = 14;
const int links = 13;
const int mitte = 33;
const int oben = 15;

uint8_t pwmValue = 250; // 50% duty cycle

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
        JsonVariant msgData = doc["data"];

        if (strcmp(msg, "startdrag") == 0) {
            startDrag();
        } else if (strcmp(msg, "dragover") == 0) {
            JsonArray dataArray = doc["data"];
            int data[4];
            for (size_t i = 0; i < dataArray.size() && i < 4; ++i) {
                data[i] = dataArray[i];
            }
            dragOver(data[0], data[1], data[2], data[3]);
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
            const int value = msgData["value"];
            selectChangePattern(value);
        } else if (strcmp(msg, "range") == 0) {
            const int value = msgData.as<int>();
            sliderPattern(value);
        } else if (strcmp(msg, "submit") == 0) {
            submitPattern();
        } else if (strcmp(msg, "endVertical") == 0) {
            // Code for endVertical
        } else if (strcmp(msg, "endHorizontal") == 0) {
            // Code for endHorizontal
        }

        Serial.printf("WebSocket JSON message received: message=%s\n", msg);
    }
}

void startDrag() {
    // Code for startDrag
}

void drop() {
    // Code for drop
}

void selectClickPattern() {
    dropdownPattern();
}

void selectChangePattern(int value) {
    // Code for directional vibration based on select change
    if (value == 0) {
        smoothTransition(mitte, links, 500); // example: transition to left
    } else if (value == 1) {
        smoothTransition(mitte, rechts, 500); // example: transition to right
    } else {
        smoothTransition(mitte, oben, 500); // example: transition to top
    }
}

void smoothTransition(int motor1, int motor2, int dauer) {
    for (int i = 0; i <= 255; i++) {
        analogWrite(motor1, 255 - i);  // Decrease motor1
        analogWrite(motor2, i);        // Increase motor2
        delay(dauer / 255);            // Delay to evenly distribute the duration
    }
}

void clickableButtonPattern() {
    for (uint8_t i = 0; i < sizeof(VIBRATORS); i++) {
        analogWrite(VIBRATORS[i], pwmValue);
    }
    delay(100); // Buzz for 100 ms
    for (uint8_t i = 0; i < sizeof(VIBRATORS); i++) {
        analogWrite(VIBRATORS[i], 0); // Turn off vibration
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
        analogWrite(mitte, pwmValue); // Mid vibrator (port 33)
        delay(30);
        analogWrite(mitte, 0);
        delay(30);
    }
}

void checkboxPattern() {
    smoothTransition(links, unten, 500); // Transition from left to bottom
    smoothTransition(unten, rechts, 500); // Transition from bottom to right
    smoothTransition(rechts, links, 500); // Transition from right to left (reset)
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
    int position = map(value, 0, 100, 1, 3);
    analogWrite(VIBRATORS[position], pwmValue);
    delay(100);
    analogWrite(VIBRATORS[position], 0);
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

void dragOver(int x, int y, int max_x, int max_y) {
    float l = 255;
    float o = 255;
    float u = 255;
    float r = 255;
    float m = 255;

    float norm_x = (float)x / max_x;
    float norm_y = (float)y / max_y;

    float center_x = 0.5;
    float center_y = 0.5;

    float dist_to_center = sqrt(pow(norm_x - center_x, 2) + pow(norm_y - center_y, 2));
    float max_dist = sqrt(0.5 * 0.5 + 0.5 * 0.5); // Maximum possible distance in a normalized 1x1 grid
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

void loop() {
    // Main code here
    ws.cleanupClients();
}