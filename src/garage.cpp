#include <Arduino.h>

#include <utils/led.h>
#include <utils/wifi_control.h>

BlinkingLed blue_led(D4, 0, 91, true);
BlinkingLed red_led(D3, 0, 123, true);

WiFiControl wifi_control(blue_led);

void setup() {
    red_led.init();
    red_led.set_pattern(0);

    Serial.begin(9600);
    Serial.println(F("Garage " __DATE__ " " __TIME__));
    wifi_control.init();

    red_led.set_pattern(0b1110);
}

void loop() {
    wifi_control.tick();
    red_led.tick();
}
