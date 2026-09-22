# ESP32TelegramBot

Lightweight, memory-efficient Telegram Bot library for ESP32 microcontrollers using ArduinoJson 7.x.

Designed specifically for embedded systems where heap memory and stability are critical. It parses only essential message fields using ArduinoJson filter deserialization, prevents memory fragmentation, and natively handles 64-bit Telegram chat and user IDs.

---

## Features

- **Memory Optimized**: Uses ArduinoJson 7 filter deserialization to parse incoming payloads without allocating memory for unused JSON fields.
- **64-bit ID Support**: Fully supports `int64_t` for user IDs and negative supergroup/channel IDs (preventing integer overflow).
- **Reliable Offset Tracking**: Confirms updates strictly after processing, preventing message loss during multi-message bursts.
- **Low Heap Fragmentation**: Uses string buffer pre-allocation (`reserve()`) for API URL and body assembly.
- **Clean Error Handling**: Dedicated `TelegramError` enum with human-readable error descriptions.

---

## Hardware & Software Requirements

- **Microcontroller**: ESP32 (Dev Module, NodeMCU-32S, ESP32-S2, ESP32-S3, ESP32-C3)
- **Framework**: Arduino Core for ESP32
- **Dependencies**:
  - [ArduinoJson](https://arduinojson.org/) (Version >= 7.0.0)
  - `WiFi` (Built into ESP32 Arduino Core)
  - `HTTPClient` (Built into ESP32 Arduino Core)
  - `WiFiClientSecure` (Built into ESP32 Arduino Core)

---

## Installation

### Manual Installation
1. Download or clone this repository:
   ```bash
   cd ~/Arduino/libraries/
   git clone https://github.com/mahadihasan9/ESP32TelegramBot.git
   ```
2. Open the Arduino IDE.
3. Install **ArduinoJson** (version 7.x) via **Library Manager** (`Sketch` -> `Include Library` -> `Manage Libraries...`).
4. Restart the Arduino IDE.

---

## Quick Start Example

This example connects to WiFi, listens for commands, and toggles the onboard LED (`GPIO 2`).

```cpp
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
```

---

## API Reference

### `ESP32TelegramBot` Class

| Method | Return Type | Description |
| :--- | :--- | :--- |
| `begin(const String& token)` | `void` | Configures the bot token and initializes transport settings. |
| `get(int limit = 1)` | `getJson` | Polls for incoming updates. Automatically updates offset to prevent reprocessing. |
| `post(int64_t chatId, const String& text)` | `bool` | Sends a text message to the specified chat. Returns `true` on success. |
| `lastError()` | `TelegramError` | Returns enum representing the last error status code. |
| `lastErrorMessage()` | `String` | Returns human-readable error description string. |
| `connected()` | `bool` | Returns `true` if WiFi is currently connected. |

---

### `getJson` Class (Message Container)

| Method | Return Type | Description |
| :--- | :--- | :--- |
| `status()` | `bool` | `true` if HTTP request and JSON parsing were successful. |
| `available()` | `bool` | `true` if a new message was received and contains valid data. |
| `name()` | `String` | Returns sender's first name, or "FirstName Username" if username is available. |
| `chatId()` / `id()` | `int64_t` | Returns unique 64-bit chat identifier. |
| `message()` | `String` | Returns the text content of the message. |
| `time()` | `long` | Returns message creation time (Unix timestamp). |

---

### `TelegramError` Codes

| Error Code | Value | Description |
| :--- | :--- | :--- |
| `TELEGRAM_OK` | 0 | Operation completed successfully. |
| `TELEGRAM_ERR_WIFI` | 1 | WiFi is not connected (`WL_CONNECTED` check failed). |
| `TELEGRAM_ERR_HTTP` | 2 | HTTPS request failed or HTTP status code is not 200. |
| `TELEGRAM_ERR_JSON` | 3 | Deserialization failed or response payload malformed. |
| `TELEGRAM_ERR_API` | 4 | Telegram server returned `{"ok": false}` in response. |
| `TELEGRAM_ERR_EMPTY` | 5 | Response is valid but contains no new messages. |
| `TELEGRAM_ERR_MEMORY` | 6 | Memory allocation failed. |

To convert an error enum to text:
```cpp
TelegramError err = bot.lastError();
const char* errStr = telegramErrorToString(err);
```

---

## Configuration

Default limits and timeouts are defined in `src/TelegramConfig.h`:

```cpp
#define TELEGRAM_DEFAULT_LIMIT   1      // Default updates fetched per request
#define TELEGRAM_MAX_LIMIT       100    // Telegram API limit ceiling
#define TELEGRAM_DEFAULT_TIMEOUT 10000  // HTTPS timeout in milliseconds
```

---

## Memory Architecture & Best Practices

1. **Keep Delays Reasonable**: When polling in `loop()`, include at least `delay(500)` to `delay(1000)` between calls to avoid triggering Telegram rate limits (429 Too Many Requests).
2. **Handle Big Groups**: Group chats have negative 64-bit IDs starting with `-100...`. Always use `int64_t` (never `int` or 32-bit `long`) when storing chat IDs.
3. **No Dynamic Allocations in Loop**: Do not create and destroy large String buffers inside tight polling loops to avoid heap fragmentation.

---

## License

MIT License. Free to use, modify, and distribute for personal and commercial projects.
