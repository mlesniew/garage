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

template <uint8_t pin, bool inverted = false>
struct Input {
    void init() {
        pinMode(pin, INPUT);
    }

    bool get() const {
        return (digitalRead(pin) == HIGH) != inverted;
    }

    operator bool() const { return get(); }
};

Output<D5, true> relay_door;
Output<D6, true> relay_light;

Input<D7, true> button;

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

    relay_door.set(button);
    relay_light.set(!button);
}
