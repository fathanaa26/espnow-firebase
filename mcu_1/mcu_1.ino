#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

// receiver MAC
uint8_t broadcastAddress[] = {0x0C, 0xB8, 0x15, 0xF7, 0x21, 0x30};
// 0x0C, 0xA8, 0xA7, 0x03, 0x25, 0xB8 HP
// 0x0C, 0xB8, 0x15, 0xF7, 0x21, 0x30
typedef struct struct_mssg {
  char a[32];
  int b;
  float c;
  bool d;
}struct_mssg;

struct_mssg data;

esp_now_peer_info_t peerStat;

constexpr char WIFI_SSID[] = "samsunga8";
int32_t getWiFiChannel(const char *ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
      for (uint8_t i=0; i<n; i++) {
          if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
              return WiFi.channel(i);
          }
      }
  }
  return 0;
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status){
  Serial.println("");
  Serial.print("Last Packet Send Status:");
  Serial.println(
    status == ESP_NOW_SEND_SUCCESS ? 
    "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  int32_t channel = getWiFiChannel(WIFI_SSID);

  WiFi.printDiag(Serial); // Uncomment to verify channel number before
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
  WiFi.printDiag(Serial); // Uncomment to verify channel change after

  if(esp_now_init() != ESP_OK){
    Serial.println("Initializing Failed, ESP-NOW Error");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  memcpy(peerStat.peer_addr, broadcastAddress, 6);
  peerStat.channel = 0;
  peerStat.encrypt = false;

  if(esp_now_add_peer (&peerStat) != ESP_OK){
    Serial.println("Failed to add peer");
    return; 
  }

}   

void loop() {
  strcpy(data.a, "THIS iS a ChAr !1!!");
  data.b = random(1,20);
  data.c = 1.09424;
  data.d = false;

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&data, sizeof(data));

  if(result == ESP_OK) {
    Serial.println("Sent with success");
    Serial.println("||||||||||||||||||||");
    Serial.println(data.a);
    Serial.println(data.b);
    Serial.println(data.c,6);
    Serial.println(data.d);
    Serial.println("||||||||||||||||||||");
  }else{
    Serial.println("Error sending the data");
  }

  delay(5000);    

}
