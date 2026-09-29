#include "getJson.h"

getJson::getJson()
    : _status(false),
      _available(false),
      _chatId(0),
      _updateId(0),
      _messageId(0),
      _timestamp(0) {}

bool getJson::status() const {
    return _status;
}

bool getJson::available() const {
    return _available;
}

String getJson::name() const {
    if (_lastName.length() > 0) {
        String result;
        result.reserve(_firstName.length() + _lastName.length() + 1);
        result = _firstName;
        result += " ";
        result += _lastName;
        return result;
    }
    return _firstName;
}

String getJson::firstName() const {
    return _firstName;
}

String getJson::lastName() const {
    return _lastName;
}

String getJson::username() const {
    return _username;
}

int64_t getJson::id() const {
    return _chatId;
}

int64_t getJson::chatId() const {
    return _chatId;
}

long getJson::time() const {
    return _timestamp;
}

String getJson::message() const {
    return _text;
}