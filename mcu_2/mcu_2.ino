#include <esp_now.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include "time.h"
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#define WIFI_SSID "samsunga8"
#define WIFI_PASSWORD "78empatx"
#define API_KEY "AIzaSyDH_KTvGm_1Gc7jpCWkfooOo8J7ExvRlPo"
#define DB_URL "https://magang2-8c7df-default-rtdb.asia-southeast1.firebasedatabase.app"
#define USER_EMAIL "fathan.aa.01@gmail.com"
#define USER_PASSWORD "78empatx"
#define DB_SECRET "GiSLt50iuqHFmQyfaP23vkwjurcqWqub2Ibf9FY4"

FirebaseData fbdo;
FirebaseAuth fbauth;
FirebaseConfig fbconfig;

typedef struct struct_mssg {
  char a[32];
  int b;
  float c;
  bool d;
}struct_mssg;

struct_mssg data;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 25200;
const int   daylightOffset_sec = 0;

unsigned long prevMillis = 0;
int count = 0;


void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&data, incomingData, sizeof(data));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Char: ");
  Serial.println(data.a);
  Serial.print("Int: ");
  Serial.println(data.b);
  Serial.print("Float: ");
  Serial.println(data.c);
  Serial.print("Bool: ");
  Serial.println(data.d);
  Serial.println();

}

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay,10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();
  /*
    Tuesday, February 07 2023 20:15:34
    Day of week: Tuesday
    Month: February
    Day of Month: 07
    Year: 2023
    Hour: 20
    Hour (12 hour format)   : 08
    Minute: 15
    Second: 34
    Time variables
    20
    Tuesday
  */
}

void setup() {
    // Initialize Serial Monitor
  Serial.begin(115200);

    // init wifi
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
      Serial.print(".");
      delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Connected with Channel: ");
  Serial.println(WiFi.channel());
  Serial.println();
  
    // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }else{
    Serial.println("Initializing ESP-NOW");
  }
  esp_now_register_recv_cb(OnDataRecv);

  fbconfig.api_key = API_KEY;
  fbauth.user.email = USER_EMAIL;
  fbauth.user.password = USER_PASSWORD;
  fbconfig.database_url = DB_URL;
  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  String base_path = "/UsersData/";

  fbconfig.token_status_callback = tokenStatusCallback;

  Firebase.begin(&fbconfig, &fbauth);
  
  String userId = "$userId";
  String authority = 
  "($userID === fbauth.uid)";
  Firebase.setReadWriteRules(fbdo, base_path, userId, authority, authority, DB_SECRET);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
}

void loop(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  String pathUidToken = fbauth.token.uid.c_str();
  char pathYears[5];
  strftime(pathYears,5,"%Y",&timeinfo);
  char pathMonths[14];
  strftime(pathMonths,14,"%B",&timeinfo);
  char pathDaysOfMonths[3];
  strftime(pathDaysOfMonths,3,"%d",&timeinfo);
  char pathDateTime[12];
  strftime(pathDateTime,12,"%H:%M:%S",&timeinfo);


  if(millis() - 5000 > prevMillis && Firebase.ready() ){
    prevMillis = millis();
    String path = "/UsersData/"+pathUidToken + "/"+pathYears+"/"+pathMonths+"/"+pathDaysOfMonths+"/"+pathDateTime;
    
    Firebase.setInt(fbdo, path, data.b);
    printLocalTime();

  }

}