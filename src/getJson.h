#ifndef TELEGRAM_GETJSON_H
#define TELEGRAM_GETJSON_H

#include <Arduino.h>

class getJson {
public:
    getJson();

    bool status() const;
    bool available() const;

    String name() const;
    int64_t id() const;
    int64_t chatId() const;
    long time() const;
    String message() const;

private:
    bool _status;
    bool _available;

    String _firstName;
    String _username;
    String _text;
    int64_t _chatId;
    int64_t _updateId;
    long _messageId;
    long _timestamp;

    friend class ESP32TelegramBot;
};

#endif