#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

#include <PicoUtils.h>

String hostname = "garage";
String password = "secret-" __TIME__;

PicoUtils::PinOutput wifi_led(D4, true);
PicoUtils::Blink wifi_led_blinker(wifi_led, 0, 91);

PicoUtils::PinOutput red_led(D3, true);
PicoUtils::Blink red_led_blinker(red_led, 0, 123);

PicoUtils::PinInput button(D7, true);
PicoUtils::PinInput sensor_open(D1, true);
PicoUtils::PinInput sensor_closed(D2, true);

PicoUtils::PinOutput relay_door(D5, true);
PicoUtils::PinOutput relay_light(D6, true);

void setup_wifi() {
    WiFi.hostname(hostname);
    WiFi.setAutoReconnect(true);

    Serial.println(F("Press button now to enter SmartConfig."));
    wifi_led_blinker.set_pattern(1);
    const PicoUtils::Stopwatch stopwatch;
    bool smart_config = false;
    {
        while (!smart_config && (stopwatch.elapsed_millis() < 3 * 1000)) {
            smart_config = button;
            delay(100);
        }
    }

    if (smart_config) {
        wifi_led_blinker.set_pattern(0b100100100 << 9);

        Serial.println(F("Entering SmartConfig mode."));
        WiFi.beginSmartConfig();
        while (!WiFi.smartConfigDone() && (stopwatch.elapsed_millis() < 5 * 60 * 1000)) {
            delay(100);
        }

        if (WiFi.smartConfigDone()) {
            Serial.println(F("SmartConfig success."));
        } else {
            Serial.println(F("SmartConfig failed.  Reboot."));
            ESP.reset();
        }
    } else {
        WiFi.softAPdisconnect(true);
        WiFi.begin();
    }

    wifi_led_blinker.set_pattern(0b10);
}

void setup() {
    relay_door.init();
    relay_door.set(false);

    relay_light.init();
    relay_light.set(false);

    wifi_led_blinker.init();
    PicoUtils::BackgroundBlinker bb(wifi_led_blinker);
    wifi_led_blinker.set_pattern(0b10);

    Serial.begin(9600);
    Serial.println(F("Garage " __DATE__ " " __TIME__));

    red_led_blinker.init();
    red_led_blinker.set_pattern(0);

    button.init();
    sensor_open.init();
    sensor_closed.init();

    setup_wifi();

    red_led_blinker.set_pattern(0b1110);

    Serial.println(F("Starting up ArduinoOTA..."));
    ArduinoOTA.setHostname(hostname.c_str());
    if (password.length()) {
        ArduinoOTA.setPassword(password.c_str());
    }
    ArduinoOTA.begin();

    Serial.println(F("Setup complete."));
}

void update_wifi_led() {
    if (WiFi.status() == WL_CONNECTED) {
        wifi_led_blinker.set_pattern(uint64_t(0b1) << 60);
    } else {
        wifi_led_blinker.set_pattern(0b1100);
    }
    wifi_led_blinker.tick();
};

void loop() {
    red_led_blinker.tick();
    update_wifi_led();

    if (sensor_open) {
        if (sensor_closed) {
            red_led_blinker.set_pattern(0b1);
        } else {
            red_led_blinker.set_pattern(0b1000);
        }
    } else {
        if (sensor_closed) {
            red_led_blinker.set_pattern(0b1110);
        } else {
            red_led_blinker.set_pattern(0b0);
        }
    }

    relay_door.set(button);
    relay_light.set(!button);
}
