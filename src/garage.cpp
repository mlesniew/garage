#include <Arduino.h>

#include <utils/led.h>
#include <utils/wifi_control.h>

BlinkingLed blue_led(D4, 0, 91, true);
BlinkingLed red_led(D3, 0, 123, true);

WiFiControl wifi_control(blue_led);

template <uint8_t pin, bool inverted>
struct Output {
    void init(const bool initial_state = false) {
        pinMode(pin, OUTPUT);
        set(initial_state);
    }

    void set(bool value) {
        digitalWrite(pin, value != inverted);
    }
};

Output<D5, true> relay_door;
Output<D6, true> relay_light;

void setup() {
    red_led.init();
    red_led.set_pattern(0);

    Serial.begin(9600);
    Serial.println(F("Garage " __DATE__ " " __TIME__));

    relay_door.init();
    relay_light.init();

    wifi_control.init();

    red_led.set_pattern(0b1110);
}

void loop() {
    wifi_control.tick();
    red_led.tick();

    const auto t = millis() / 1000;
    relay_door.set(t % 3 == 0);
    relay_light.set(t % 5 == 0);
}
