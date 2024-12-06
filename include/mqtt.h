#include <PubSubClient.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <setup.h>
#include <restart.h>

char jsonbuff[MAX_MSG_SIZE] = "{\0";

WiFiClient espClient;
PubSubClient client(espClient);

const uint16_t kIrLed = M5_IR; // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRDaikinESP ac(kIrLed);        // Set the GPIO to be used to sending the message

void reconnectMqtt()
{
    // Loop until we're reconnected
    int i = 0;
    while (!client.connected())
    {
        Serial.printf("Attempting MQTT connection as %s...", MQTT_USER);
        String clientId = "M5Stack-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect
        if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD))
        {
            Serial.println("connected");
            // Once connected, publish an announcement...
            client.publish("daikin/status", "connected");
            // ... and resubscribe
            client.subscribe("daikin/command");
        }
        else
        {
            Serial.printf("failed, rc=%d, try again in 5 seconds", client.state());
            delay(5000);

            if (i++ == 100)
            {
                Serial.printf("Tried 100 times, restarting...");
                restartBoard();
            }
        }
    }
}

void callback(char *topic, byte *payload, unsigned int length)
{
    if (strcmp(topic, "daikin/command") == 0)
    {
        //    StaticJsonDocument<MAX_MSG_SIZE> doc;
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);
        if (error)
        {
            mqttSerial.println(error.c_str());
            mqttSerial.printf("Payload (%d): %s\n", length, payload);
            delay(5000);
            return;
        }
#if defined(ARDUINO_M5Stick_C_Plus) || defined(ARDUINO_M5Stick_C)
        if (doc["sleep"].is<u64_t>())
        {
            u64_t sleep = doc["sleep"];
            mqttSerial.printf("Sleeping for %ds\n", sleep);
            M5.Axp.DeepSleep(SLEEP_SEC(sleep));
            return;
        }
#endif
        bool on = true;
        if (doc["on"].is<bool>())
        {
            on = doc["on"];
        }
        if (on)
        {
            ac.on();
            ac.setFan(1);
            ac.setMode(kDaikinHeat);
            ac.setSwingVertical(false);
            ac.setSwingHorizontal(false);
        }
        else
        {
            ac.off();
        }

        if (doc["temperature"].is<uint8_t>())
        {
            uint8_t temperature = doc["temperature"];
            ac.setTemp(temperature);
        }
        // Switch on the LED if an 1 was received as first character
        mqttSerial.printf("Send: %s\n", ac.toString().c_str());
        ac.send();

        // client.publish("daikin/status", "IR sent");
    }
    else
    {
        mqttSerial.printf("Unknown topic: %s\n", topic);
    }
}