/*
 * Copyright (c) 2024 zahical. All rights reserved.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/**
 * @file HTTP(S) client implementation, using WiFiClient, WiFiClientSecure and HttpClient.
 */

#include "data_fetcher.h"

#include "common.h"
#include "config.h"

#include <HTTPClient.h>

namespace
{
WiFiClient *transport = nullptr;
HTTPClient *client    = nullptr;

HTTPClient& init_http(String const& server, String const& uri)
{
    if (transport == nullptr)
    {
        if (cfg::UseHTTPS)
        {
            auto c = new WiFiClientSecure();
            c->setCACert(cfg::OWM_ROOT_CA);
            transport = c;
        }
        else { transport = new WiFiClient{}; }

        client = new HTTPClient();
        // Attempt to reuse connections if possible, as each TLS handshake incurs a noticeable cost (~800 ms)
        client->setReuse(true);
    }

    if (cfg::UseHTTPS)
        client->begin(*transport, server, 443, uri, true);
    else
        client->begin(*transport, server, 80, uri, false);

    return *client;
}

} // namespace

void reset_http()
{
    if (client != nullptr)
    {
        client->end();
        transport->stop();
    }
}

OpResult<String> get_url(String const& server, String const& uri)
{
    OPT_LOG(Log_HttpReq, Serial.println("REQ>" + String(cfg::UseHTTPS ? "https" : "http") + "://" + server + uri));

    auto& http = init_http(server, uri);

    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
        OPT_LOG(Log_HttpReq, Serial.println("RSP> OK"));

        auto r = http.getString();
        OPT_LOG(Log_HttpBody, Serial.print("Body> "); Serial.println(r));

        return r;
    }
    else
    {

        // these actually are HTTPC_ERROR_*
        if (httpCode < 0)
        {
            OPT_LOG(Log_HttpErrs, Serial.println("RSP> CERR " + HTTPClient::errorToString(httpCode)));
            reset_http();
            return op_failed("HTTP Client ERR " + HTTPClient::errorToString(httpCode));
        }
        else
        {
            // http error -- there might be a body
            auto r = http.getString();

            OPT_LOG(Log_HttpErrs, Serial.println("RSP> ERR " + String(httpCode) + " " + r));
            reset_http();
            return op_failed("HTTP ERR " + String(httpCode) + ": " + r);
        }
    }
}
