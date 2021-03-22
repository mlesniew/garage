#include <Arduino.h>

#include <utils/led.h>
#include <utils/wifi_control.h>

#define HOSTNAME "Garage"

#ifndef PASSWORD
#define PASSWORD "secret-" __TIME__
#endif

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

    Output<pin, inverted> & operator=(const bool value) {
        set(value);
        return *this;
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

Input<D7, true> button;
Input<D1, true> sensor_open;
Input<D2, true> sensor_closed;

Output<D5, true> relay_door;
Output<D6, true> relay_light;

void setup() {
    red_led.init();
    red_led.set_pattern(0);

    Serial.begin(9600);
    Serial.println(F(HOSTNAME " " __DATE__ " " __TIME__));

    relay_door.init();
    relay_light.init();

    button.init();
    sensor_open.init();
    sensor_closed.init();

    if (button) {
        Serial.println(F("Starting conifg AP, ssid: " HOSTNAME " password: " PASSWORD));
        wifi_control.init(button ? WiFiInitMode::setup : WiFiInitMode::saved, HOSTNAME, PASSWORD);
    } else {
        wifi_control.init(button ? WiFiInitMode::setup : WiFiInitMode::saved, HOSTNAME, PASSWORD);
    }

    red_led.set_pattern(0b1110);
}

void loop() {
    wifi_control.tick();
    red_led.tick();

    if (sensor_open) {
        if (sensor_closed)
            red_led.set_pattern(0b1);
        else
            red_led.set_pattern(0b1000);
    } else {
        if (sensor_closed)
            red_led.set_pattern(0b1110);
        else
            red_led.set_pattern(0b0);
    }

    relay_door = button;
    relay_light = !button;
}
