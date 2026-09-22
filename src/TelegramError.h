#ifndef TELEGRAM_ERROR_H
#define TELEGRAM_ERROR_H

#include <Arduino.h>

enum TelegramError {
    TELEGRAM_OK = 0,
    TELEGRAM_ERR_WIFI,
    TELEGRAM_ERR_HTTP,
    TELEGRAM_ERR_JSON,
    TELEGRAM_ERR_API,
    TELEGRAM_ERR_EMPTY,
    TELEGRAM_ERR_MEMORY
};

inline const char* telegramErrorToString(TelegramError err) {
    switch (err) {
        case TELEGRAM_OK:         return "OK";
        case TELEGRAM_ERR_WIFI:   return "WiFi not connected";
        case TELEGRAM_ERR_HTTP:   return "HTTP request failed";
        case TELEGRAM_ERR_JSON:   return "JSON parse failed";
        case TELEGRAM_ERR_API:    return "Telegram API error";
        case TELEGRAM_ERR_EMPTY:  return "No new messages";
        case TELEGRAM_ERR_MEMORY: return "Out of memory";
        default:                  return "Unknown error";
    }
}

#endif