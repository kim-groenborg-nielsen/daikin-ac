// #include <Arduino.h>
#ifdef ARDUINO_M5Stick_C_Plus
#include <M5StickCPlus.h>
#elif ARDUINO_M5Stick_C
#include <M5StickC.h>
#else
#include <Arduino.h>
#endif

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Daikin.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "setup.h"
#include "mqttserial.h"
#include "mqtt.h"
#include "restart.h"

// const char *ssid = STR(WIFI_SSID);
// const char *password = STR(WIFI_PASSWORD);
// const char *mqtt_server = STR(MQTT_SERVER);
// const char *mqtt_user = STR(MQTT_USER);
// const char *mqtt_password = STR(MQTT_PASSWORD);

#if defined(ARDUINO_M5Stick_C_Plus) || defined(ARDUINO_M5Stick_C)
long LCDTimeout = 40000;
bool screenOn = true;
#endif

// WiFiClient espClient;
// PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (500)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void extraLoop()
{
  client.loop();

#if defined(ARDUINO_M5Stick_C_Plus) || defined(ARDUINO_M5Stick_C)
  if (M5.BtnA.wasPressed())
  {
    M5.Axp.ScreenSwitch(true);
    screenOn = true;
    LCDTimeout = millis() + 30000;
    Serial.println("Button A pressed");
    client.publish("daikin/status", "Button A pressed");
  }
  else if (LCDTimeout < millis())
  {
    if (screenOn)
    {
      M5.Axp.ScreenSwitch(false);
      screenOn = false;
      Serial.println("Turning off screen");
      client.publish("daikin/status", "Turning off screen");
    }
  }
  M5.update();
#endif
}

void checkWifi()
{
  int i = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");

    if (i++ == 240)
    {
      Serial.print("Tried connecting to 2 minutes, restarting");
      // ESP.restart();
      restartBoard();
    }
  }
}

void setupWifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  mqttSerial.printf("Connecting to %s\n", WIFI_SSID);

  // WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  checkWifi();
  mqttSerial.printf("Connected. IP address: %s\n", WiFi.localIP().toString().c_str());

  randomSeed(micros());
}

void setupScreen()
{
#if defined(ARDUINO_M5Stick_C_Plus) || defined(ARDUINO_M5Stick_C)
  M5.begin();
  M5.Axp.EnableCoulombcounter();
  M5.Lcd.setRotation(1);
  M5.Axp.ScreenSwitch(true);
  M5.Lcd.fillScreen(TFT_WHITE);
  M5.Lcd.setFreeFont(&FreeSansBold12pt7b);
  m5.Lcd.setTextDatum(MC_DATUM);
  int xpos = m5.Lcd.width() / 2;
  int ypos = m5.Lcd.height() / 2;
  M5.Lcd.setTextColor(TFT_DARKGREY);
  M5.Lcd.drawString("Daikin Heat", xpos, ypos, 1);
  delay(5000);
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextFont(1);
  M5.Lcd.setTextColor(TFT_GREEN);
#endif
}

void setup()
{
  Serial.begin(115200);
  setupScreen();
  mqttSerial.print("Setting up WiFi...");
  setupWifi();

  client.setServer(MQTT_SERVER, 1883);
  client.setBufferSize(MAX_MSG_SIZE);
  client.setCallback(callback);
  mqttSerial.print("Connecting to MQTT...");
  mqttSerial.begin(&client, "daikin/status");
  reconnectMqtt();
  ac.begin();
  M5.Lcd.println("Ready");
}

void waitLoop(uint ms)
{
  unsigned long start = millis();
  while (millis() < start + ms)
  {
    extraLoop();
  }
}

void loop()
{
  unsigned long start = millis();
  if (WiFi.status() != WL_CONNECTED)
  {
    checkWifi();
  }
  if (!client.connected())
  {
    reconnectMqtt();
  }

  waitLoop(FREQUENCY - millis() + start);
}
