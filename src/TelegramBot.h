#ifndef TELEGRAM_BOT_H
#define TELEGRAM_BOT_H

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "TelegramConfig.h"
#include "TelegramError.h"
#include "Transport.h"
#include "getJson.h"

class TelegramBot {
public:
    TelegramBot();

    void begin(const String& token);

    getJson get(int limit = TELEGRAM_DEFAULT_LIMIT);
    bool post(long chatId, const String& text);

    TelegramError lastError() const;
    String lastErrorMessage() const;
    bool connected() const;

private:
    String _token;
    long _lastUpdateId;
    TelegramError _lastError;

    Transport _transport;

    String _buildGetUpdatesUrl(int limit);
    String _buildSendMessageBody(long chatId, const String& text);
    void _updateOffset(JsonArray& results);
    void _parseUpdate(JsonObject& update, getJson& out);
    void _resetGetJson(getJson& out);
};

#endif