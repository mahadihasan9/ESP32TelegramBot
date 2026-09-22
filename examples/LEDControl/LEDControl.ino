#include <ESP32TelegramBot.h>

#define LED_PIN   2
#define BOT_TOKEN "YOUR_BOT_TOKEN_HERE"

ESP32TelegramBot myBot;

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.print("Connecting WiFi");
    WiFi.begin("YOUR_SSID", "YOUR_PASSWORD");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    Serial.print("WiFi OK, IP: ");
    Serial.println(WiFi.localIP());

    myBot.begin(BOT_TOKEN);
}

void loop() {
    getJson myjson = myBot.get(1);

    if (!myjson.status()) {
        delay(1000);
        return;
    }

    if (!myjson.available()) {
        delay(1000);
        return;
    }

    String text = myjson.message();
    String name = myjson.name();
    long chatId = myjson.id();

    Serial.print("From: ");
    Serial.print(name);
    Serial.print(" | Text: ");
    Serial.println(text);

    text.toLowerCase();

    if (text == "on") {
        digitalWrite(LED_PIN, HIGH);
        myBot.post(chatId, "LED ON");
    }
    else if (text == "off") {
        digitalWrite(LED_PIN, LOW);
        myBot.post(chatId, "LED OFF");
    }
    else if (text == "status") {
        int state = digitalRead(LED_PIN);
        String reply = state ? "LED is ON" : "LED is OFF";
        myBot.post(chatId, reply);
    }
    else {
        myBot.post(chatId, "Unknown command: " + text);
    }

    delay(1000);
}
