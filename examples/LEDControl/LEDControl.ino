#include <WiFi.h>
#include <ESP32TelegramBot.h>

#define WIFI_SSID     "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define BOT_TOKEN     "YOUR_TELEGRAM_BOT_TOKEN"
#define LED_PIN       2

ESP32TelegramBot bot;

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.print("Connecting to WiFi");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected.");

    bot.begin(BOT_TOKEN);
}

void loop() {
    getJson msg = bot.get();

    if (!msg.status() || !msg.available()) {
        delay(1000);
        return;
    }

    int64_t chatId = msg.chatId();
    String text = msg.message();
    String sender = msg.name();

    Serial.printf("[%s]: %s\n", sender.c_str(), text.c_str());

    text.toLowerCase();

    if (text == "/start") {
        bot.post(chatId, "Welcome! Send 'on', 'off', or 'status' to control the LED.");
    } 
    else if (text == "on") {
        digitalWrite(LED_PIN, HIGH);
        bot.post(chatId, "LED is now ON");
    } 
    else if (text == "off") {
        digitalWrite(LED_PIN, LOW);
        bot.post(chatId, "LED is now OFF");
    } 
    else if (text == "status") {
        bool state = digitalRead(LED_PIN);
        bot.post(chatId, state ? "LED is currently ON" : "LED is currently OFF");
    } 
    else {
        bot.post(chatId, "Unknown command: " + text);
    }

    delay(1000);
}
