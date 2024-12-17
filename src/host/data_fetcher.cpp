/*
 * Copyright (c) 2024 zahical. All rights reserved.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/**
 * @file HTTP(S) client implementation, using https://github.com/yhirose/cpp-httplib lib.
 */
#include "data_fetcher.h"

#include "common.h"
#include "config.h"
#include "test_data.h"

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "libs/httplib/httplib.h"

#include <cstring>

namespace
{
bool use_mock_data = false;

OpResult<String> do_api_call(String const& server, String const& uri)
{
    using namespace httplib;

    auto scheme_host = (cfg::UseHTTPS ? "https://" : "http://") + server;

    httplib::Client cli(scheme_host);
    if (cfg::UseHTTPS) cli.load_ca_cert_store(cfg::OWM_ROOT_CA, std::strlen(cfg::OWM_ROOT_CA));

    OPT_LOG(Log_HttpReq, Serial.println("REQ> " + scheme_host + uri));

    auto r = cli.Get(uri);

    OPT_LOG(Log_HttpReq, Serial.println("RSP> " + to_string(r.error()) + " " +
                                        (r.error() == Error::Success ? String{r->status} : "")));

    if (r.error() != Error::Success) return op_failed("HTTP Client ERR " + to_string(r.error()));
    if (r->status != 200) return op_failed("HTTP ERR " + String(r->status) + ": " + r->body);

    OPT_LOG(Log_HttpBody, Serial.print("Body> "); Serial.println(r->body));

    return String(r->body);
}

OpResult<String> fetch_mock_data(String const&, String const& uri)
{
    if (uri.startsWith("/data/2.5/weather?"))
        return String{Sample_Weather};
    else if (uri.startsWith("/data/2.5/forecast?"))
        return String{Sample_Forecast};
    else if (uri.startsWith("/data/2.5/air_pollution?"))
        return String{Sample_AQI};
    else
        return op_failed{"404 Not Found"};
}

} // namespace

void http_use_mock_data() { use_mock_data = true; }

void reset_http()
{
    // no-op in host emulator
}

OpResult<String> get_url(String const& server, String const& uri)
{
    return use_mock_data ? fetch_mock_data(server, uri) : do_api_call(server, uri);
}