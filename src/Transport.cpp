#include "Transport.h"

Transport::Transport()
    : _timeout(TELEGRAM_DEFAULT_TIMEOUT),
      _lastHttpCode(0),
      _lastError(TELEGRAM_OK) {}

void Transport::setToken(const String& token) {
    _token = token;
}

void Transport::setTimeout(unsigned long ms) {
    _timeout = ms;
}

int Transport::lastHttpCode() const {
    return _lastHttpCode;
}

TelegramError Transport::lastError() const {
    return _lastError;
}

String Transport::_buildUrl(const String& pathAndQuery) {
    String url;
    url.reserve(96 + _token.length() + pathAndQuery.length());
    url = "https://api.telegram.org/bot";
    url += _token;
    url += "/";
    url += pathAndQuery;
    return url;
}

bool Transport::get(const String& pathAndQuery, String& response) {
    _lastError = TELEGRAM_OK;
    _lastHttpCode = 0;

    if (WiFi.status() != WL_CONNECTED) {
        _lastError = TELEGRAM_ERR_WIFI;
        return false;
    }

    String url = _buildUrl(pathAndQuery);

    _client.setInsecure();
    _client.setTimeout(_timeout / 1000);

    HTTPClient http;
    http.setTimeout(_timeout);
    http.useHTTP10(true);

    if (!http.begin(_client, url)) {
        _lastError = TELEGRAM_ERR_HTTP;
        return false;
    }

    _lastHttpCode = http.GET();

    if (_lastHttpCode != 200) {
        _lastError = TELEGRAM_ERR_HTTP;
        http.end();
        return false;
    }

    response = http.getString();
    http.end();

    if (response.length() == 0) {
        _lastError = TELEGRAM_ERR_EMPTY;
        return false;
    }

    return true;
}

bool Transport::postJson(const String& path, const String& jsonBody, String& response) {
    _lastError = TELEGRAM_OK;
    _lastHttpCode = 0;

    if (WiFi.status() != WL_CONNECTED) {
        _lastError = TELEGRAM_ERR_WIFI;
        return false;
    }

    String url = _buildUrl(path);

    _client.setInsecure();
    _client.setTimeout(_timeout / 1000);

    HTTPClient http;
    http.setTimeout(_timeout);
    http.useHTTP10(true);

    if (!http.begin(_client, url)) {
        _lastError = TELEGRAM_ERR_HTTP;
        return false;
    }

    http.addHeader("Content-Type", "application/json");

    _lastHttpCode = http.POST(jsonBody);

    if (_lastHttpCode != 200) {
        _lastError = TELEGRAM_ERR_HTTP;
        http.end();
        return false;
    }

    response = http.getString();
    http.end();
    return true;
}