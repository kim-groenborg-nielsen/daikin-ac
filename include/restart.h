#ifndef DAIKIN_RETART_H
#define DAIKIN_RETART_H

#include <Arduino.h>

void restartBoard()
{
#if defined(ARDUINO_ARCH_ESP8266)
    system_restart();
#else
    esp_restart();
#endif
}

#endif // DAIKIN_RETART_H