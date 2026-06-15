#include <WiFi.h>
#include <esp_now.h>

#define LED_PIN 2   // XIAO ESP32-C3 LED pin

// 🔹 TRANSMITTER MAC ADDRESS
uint8_t transmitterMAC[] = {0xE4, 0xB3, 0x23, 0xC4, 0x64, 0x64};

typedef struct message {
  char cmd[10];
} message;

message received;
bool blinkMode = false;

// ✅ NEW ESP-NOW RECEIVE CALLBACK (ESP32-C3)
void onReceive(const esp_now_recv_info *info,
               const uint8_t *incomingData,
               int len) {

  // 🔒 Check transmitter MAC
  if (memcmp(info->src_addr, transmitterMAC, 6) != 0) {
    Serial.println("Unknown transmitter");
    return;
  }

  memcpy(&received, incomingData, sizeof(received));
  Serial.println(received.cmd);

  if (strcmp(received.cmd, "ON") == 0) {
    blinkMode = false;
    digitalWrite(LED_PIN, HIGH);
  }
  else if (strcmp(received.cmd, "OFF") == 0) {
    blinkMode = false;
    digitalWrite(LED_PIN, LOW);
  }
  else if (strcmp(received.cmd, "BLINK") == 0) {
    blinkMode = true;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  // 🔹 Add TRANSMITTER as peer
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, transmitterMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add TX peer");
    return;
  }

  esp_now_register_recv_cb(onReceive);
  Serial.println("Receiver Ready");
}

void loop() {
  if (blinkMode) {
    digitalWrite(LED_PIN, HIGH);
    delay(300);
    digitalWrite(LED_PIN, LOW);
    delay(300);
  }
}
