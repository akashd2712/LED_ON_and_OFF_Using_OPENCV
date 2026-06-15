#include <WiFi.h>
#include <esp_now.h>

// 🔹 RECEIVER MAC ADDRESS (RX BOARD MAC)
uint8_t receiverMAC[] = {0xE4, 0xB3, 0x23, 0xC4, 0x58, 0x3C};

typedef struct message {
  char cmd[10];
} message;

message dataToSend;

// ✅ NEW SEND CALLBACK (ESP32-C3)
void onSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "SUCCESS" : "FAIL");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_STA);

  Serial.print("TX MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_send_cb(onSent);

  // 🔹 Add RECEIVER as peer
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add RX peer");
    return;
  }

  Serial.println("Transmitter Ready");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.length() > 0) {
      input.toCharArray(dataToSend.cmd, sizeof(dataToSend.cmd));

      esp_err_t result = esp_now_send(
        receiverMAC,
        (uint8_t *)&dataToSend,
        sizeof(dataToSend)
      );

      if (result == ESP_OK) {
        Serial.println("Command Sent: " + input);
      } else {
        Serial.println("Send Error");
      }
    }
  }
}
