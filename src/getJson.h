#ifndef TELEGRAM_GETJSON_H
#define TELEGRAM_GETJSON_H

#include <Arduino.h>

class getJson {
public:
    getJson();

    bool status() const;
    bool available() const;

    String name() const;
    long id() const;
    long time() const;
    String message() const;

private:
    bool _status;
    bool _available;

    String _firstName;
    String _username;
    String _text;
    long _chatId;
    long _updateId;
    long _messageId;
    long _timestamp;

    friend class TelegramBot;
};

#endif