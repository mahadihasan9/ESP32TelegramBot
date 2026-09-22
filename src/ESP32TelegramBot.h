#ifndef ESP32_TELEGRAM_BOT_H
#define ESP32_TELEGRAM_BOT_H

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "TelegramConfig.h"
#include "TelegramError.h"
#include "Transport.h"
#include "getJson.h"

class ESP32TelegramBot {
public:
    ESP32TelegramBot();

    void begin(const String& token);

    getJson get(int limit = TELEGRAM_DEFAULT_LIMIT);
    bool post(int64_t chatId, const String& text);

    TelegramError lastError() const;
    String lastErrorMessage() const;
    bool connected() const;

private:
    String _token;
    int64_t _lastUpdateId;
    TelegramError _lastError;

    Transport _transport;

    String _buildGetUpdatesUrl(int limit);
    String _buildSendMessageBody(int64_t chatId, const String& text);
    void _updateOffset(int64_t updateId);
    void _parseUpdate(JsonObject& update, getJson& out);
    void _resetGetJson(getJson& out);
};

#endif

