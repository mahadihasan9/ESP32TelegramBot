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
    if (_username.length() > 0) {
        String result;
        result.reserve(_firstName.length() + _username.length() + 1);
        result = _firstName;
        result += " ";
        result += _username;
        return result;
    }
    return _firstName;
}

long getJson::id() const {
    return _chatId;
}

long getJson::time() const {
    return _timestamp;
}

String getJson::message() const {
    return _text;
}