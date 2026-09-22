#include "ESP32TelegramBot.h"

ESP32TelegramBot::ESP32TelegramBot()
    : _lastUpdateId(0),
      _lastError(TELEGRAM_OK) {}

void ESP32TelegramBot::begin(const String& token) {
    _token = token;
    _transport.setToken(token);
    _transport.setTimeout(TELEGRAM_DEFAULT_TIMEOUT);
    _lastUpdateId = 0;
    _lastError = TELEGRAM_OK;
}

bool ESP32TelegramBot::connected() const {
    return WiFi.status() == WL_CONNECTED;
}

TelegramError ESP32TelegramBot::lastError() const {
    return _lastError;
}

String ESP32TelegramBot::lastErrorMessage() const {
    return String(telegramErrorToString(_lastError));
}

void ESP32TelegramBot::_resetGetJson(getJson& out) {
    out._status = false;
    out._available = false;
    out._firstName = "";
    out._username = "";
    out._text = "";
    out._chatId = 0;
    out._updateId = 0;
    out._messageId = 0;
    out._timestamp = 0;
}

String ESP32TelegramBot::_buildGetUpdatesUrl(int limit) {
    String url;
    url.reserve(64);
    url = "getUpdates?offset=";
    char buf[32];
    snprintf(buf, sizeof(buf), "%lld", (long long)(_lastUpdateId + 1));
    url += buf;

    if (limit > 0) {
        if (limit > TELEGRAM_MAX_LIMIT) {
            limit = TELEGRAM_MAX_LIMIT;
        }
        url += "&limit=";
        url += String(limit);
    }
    return url;
}

void ESP32TelegramBot::_updateOffset(int64_t updateId) {
    if (updateId >= _lastUpdateId) {
        _lastUpdateId = updateId;
    }
}

void ESP32TelegramBot::_parseUpdate(JsonObject& update, getJson& out) {
    out._updateId = update["update_id"] | 0LL;

    JsonObject msg = update["message"];
    if (msg.isNull()) {
        return;
    }

    out._messageId = msg["message_id"] | 0L;
    out._timestamp = msg["date"] | 0L;
    out._text = msg["text"] | "";

    JsonObject chat = msg["chat"];
    if (!chat.isNull()) {
        out._chatId = chat["id"] | 0LL;
    }

    JsonObject from = msg["from"];
    if (!from.isNull()) {
        out._firstName = from["first_name"] | "";
        out._username = from["username"] | "";
    }

    out._status = true;
    out._available = true;
}

getJson ESP32TelegramBot::get(int limit) {
    getJson result;
    _resetGetJson(result);
    _lastError = TELEGRAM_OK;

    if (WiFi.status() != WL_CONNECTED) {
        _lastError = TELEGRAM_ERR_WIFI;
        return result;
    }

    String url = _buildGetUpdatesUrl(limit);
    String response;

    if (!_transport.get(url, response)) {
        _lastError = _transport.lastError();
        return result;
    }

    JsonDocument filter;
    filter["ok"] = true;
    filter["result"][0]["update_id"] = true;
    filter["result"][0]["message"]["message_id"] = true;
    filter["result"][0]["message"]["date"] = true;
    filter["result"][0]["message"]["text"] = true;
    filter["result"][0]["message"]["chat"]["id"] = true;
    filter["result"][0]["message"]["from"]["first_name"] = true;
    filter["result"][0]["message"]["from"]["username"] = true;

    JsonDocument doc;

    DeserializationError err = deserializeJson(
        doc, response,
        DeserializationOption::Filter(filter)
    );

    if (err) {
        _lastError = TELEGRAM_ERR_JSON;
        return result;
    }

    bool ok = doc["ok"] | false;
    if (!ok) {
        _lastError = TELEGRAM_ERR_API;
        return result;
    }

    result._status = true;

    JsonArray results = doc["result"].as<JsonArray>();
    if (results.isNull() || results.size() == 0) {
        _lastError = TELEGRAM_ERR_EMPTY;
        return result;
    }

    JsonObject first = results[0];
    int64_t uid = first["update_id"] | 0LL;
    _updateOffset(uid);
    _parseUpdate(first, result);

    return result;
}

String ESP32TelegramBot::_buildSendMessageBody(int64_t chatId, const String& text) {
    JsonDocument doc;
    doc["chat_id"] = chatId;
    doc["text"] = text;

    String body;
    body.reserve(128 + text.length());
    serializeJson(doc, body);
    return body;
}

bool ESP32TelegramBot::post(int64_t chatId, const String& text) {
    _lastError = TELEGRAM_OK;

    if (WiFi.status() != WL_CONNECTED) {
        _lastError = TELEGRAM_ERR_WIFI;
        return false;
    }

    String body = _buildSendMessageBody(chatId, text);
    String response;

    if (!_transport.postJson("sendMessage", body, response)) {
        _lastError = _transport.lastError();
        return false;
    }

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, response);

    if (err) {
        _lastError = TELEGRAM_ERR_JSON;
        return false;
    }

    bool ok = doc["ok"] | false;
    if (!ok) {
        _lastError = TELEGRAM_ERR_API;
        return false;
    }

    return true;
}

