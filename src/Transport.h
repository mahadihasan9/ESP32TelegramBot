#ifndef TELEGRAM_TRANSPORT_H
#define TELEGRAM_TRANSPORT_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "TelegramConfig.h"
#include "TelegramError.h"

class Transport {
public:
    Transport();
    void setToken(const String& token);
    void setTimeout(unsigned long ms);

    bool get(const String& pathAndQuery, String& response);
    bool postJson(const String& path, const String& jsonBody, String& response);

    int lastHttpCode() const;
    TelegramError lastError() const;

private:
    String _token;
    unsigned long _timeout;
    int _lastHttpCode;
    TelegramError _lastError;
    WiFiClientSecure _client;

    String _buildUrl(const String& pathAndQuery);
};

#endif