#define XSTR(x) #x
#define STR(x) XSTR(x)

#define MAX_MSG_SIZE 5120
#define FREQUENCY 30000 // 30 seconds

// #define WIFI_SSID "KOMA-SD"
#define WIFI_SSID STR(ENV_WIFI_SSID)
#define WIFI_PASSWORD STR(ENV_WIFI_PASSWORD)
#define MQTT_SERVER STR(ENV_MQTT_SERVER)
#define MQTT_USER STR(ENV_MQTT_USER)
#define MQTT_PASSWORD STR(ENV_MQTT_PASSWORD)
#define MQTT_PORT 1883