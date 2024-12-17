/*
 * Copyright (c) David Bird 2021. All rights to this software are reserved.
 * Copyright (c) 2024 zahical. All rights reserved.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/**
 * @file User-configurable values.
 */

#include "config.h"
#include <chrono>
#ifndef HOST_BUILD
#include "zonedbx/zone_infos.h"
#endif

namespace cfg
{
// if you will be doing active dev work, better use _dev_config.h for secrets. See the file for details.
#if __has_include("dev_config.h")
#include "dev_config.h"
#else

/* === Part I: Things you MUST change ======================= */
// 1. Wifi credentials ------
constexpr char const WiFiSSID[]     = "";
constexpr char const WiFiPassword[] = "";


// 2. OWM API Key ------
// You'll need your own API key for the OpenWeatherMap API.
// You can obtain one by signing up here: https://home.openweathermap.org/users/sign_up
// Note, we DON'T need the "One Call API 3.0" that has a separate subscription.
// A simple free-tier key for the "Professional collections" is sufficient.
constexpr char const ApiKey[] = "";
#endif


// 3. Geo location ------------
// The most reliable way to get these is to go to https://openweathermap.org/, enter your location in the 'Search city'
// box and press 'Search'. In the result drop down that will appear, the last two items are the latitude and longitude
// that you need.
constexpr char const Latitude[]  = "42.69"; // Latitude in decimal degrees; negatives are in the southern hemisphere
constexpr char const Longitude[] = "23.32"; // Longitude in decimal degrees


// 4. The name of your location --------
// This is only used for display; you may enter anything here but non-latin chars might not display properly.
constexpr char const City[] = "Sofia";


// 5. Time zone --------------
// Visit https://github.com/bxparks/AceTime/blob/v2.3.2/src/zonedbx/zone_infos.h and find your time zone.
// It should look something like kZone[Continent]_[City].
// Set it below, after the `zonedbx::` part.
#ifndef HOST_BUILD
ace_time::extended::ZoneInfo const& TZ = ace_time::zonedbx::kZoneEurope_Sofia;
#endif


// 6. Units -----------------
// true, to use metric units: degree Celsius, meters, etc; false for imperial unit: degree Fahrenheit, inches, etc.
constexpr bool UseMetricUnits = true;



/* === Part II: Things you might want to change ======================= */
// 7. Working hours and refresh period.
// To save power, the station will only update between `OnTime` and `OffTime` every `RefreshPeriod`.
constexpr DailyTime OnTime{8, 00};
constexpr DailyTime OffTime{23, 0};
constexpr std::chrono::seconds RefreshPeriod = std::chrono::hours{1};


// 8. Air Pollutants
// Choose which 3 Air Quality components to display (see aqi_metric.h for possible values)
constexpr AQIndicator AQIComponents[3] = {AQIndicator::PM2_5, AQIndicator::PM10, AQIndicator::NO2};



/* === Part III: Things you probably don't want to change ======================= */
// 9. Network time server
// The default is good. See https://www.ntppool.org/en/use.html for details.
// Another option is continental zones ('europe.pool.ntp.org', 'north-america.pool.ntp.org', 'asia.pool.ntp.org', etc.)
constexpr char const NTPServer[] = "pool.ntp.org";


// 10. Scheduling tolerance
// If the station wakes up `MaxDrift` time before a regular update, it will consider it completed and will not wake up
// again to do it. For example, is the station should update at 10:00 but for some reason wakes up at 9:58, it will do
// an update, but will not wake up again to do the 10:00 update, but instead will directly sleep until 11:00 (assuming
// 1h refresh cycle)
constexpr std::chrono::seconds MaxDrift = std::chrono::minutes{5};


// 11. Extra sleep
// This amount of time is always added to all sleep operations to safe-guard against the possibility that ESP's RTC
// clock might be running a little faster, so instead of at XX:00:00 the station tends to wake up at XX:59:YY. The
// `MaxDrift` setting above already takes care of not waking up a second time in that case, so this is purely for
// aesthetics.
constexpr std::chrono::seconds ExtraSleep = std::chrono::seconds{10};


// 11. Max NTP Time Sync wait
// The maximum amount of time the station will wait for the time to synchronize with the NTP server.
// If the time is still not synced after that, the update cycle will fail.
constexpr std::chrono::seconds MaxNTPSyncWait = std::chrono::seconds{5};


// 12. Max WiFi connect attempts
// The number of WiFi connection attempts.
// If the WiFi is still not connected after that, the update cycle will fail.
constexpr unsigned MaxWiFiConnectAttempts = 2;


// 13. Max HTTP request retries
// The number of time a failed HTTP API request will be retried.
// If the is still not successful after that, the update cycle will fail.
constexpr unsigned MaxHTTPRetries = 2;


// 14. API Server
// Host name only, no scheme or port
constexpr char const ApiServer[] = "api.openweathermap.org";


// 15. Use HTTPS for OWM API requests
constexpr bool UseHTTPS = true;


// 16. CA used to verify OWM's API end-point cert
// CN: USERTrust RSA Certification Authority
// Serial: 01:FD:6D:30:FC:A3:CA:51:A8:1B:BC:64:0E:35:03:2D
// SHA-256: E7:93:C9:B0:2F:D8:AA:13:E2:1C:31:22:8A:CC:B0:81:19:64:3B:74:9C:89:89:64:B1:74:6D:46:C3:D4:CB:D2
// Not After: Mon, 18 Jan 2038 23:59:59 GMT
constexpr char const *OWM_ROOT_CA =
    R"(-----BEGIN CERTIFICATE-----
MIIF3jCCA8agAwIBAgIQAf1tMPyjylGoG7xkDjUDLTANBgkqhkiG9w0BAQwFADCB
iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl
cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV
BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTAw
MjAxMDAwMDAwWhcNMzgwMTE4MjM1OTU5WjCBiDELMAkGA1UEBhMCVVMxEzARBgNV
BAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0plcnNleSBDaXR5MR4wHAYDVQQKExVU
aGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNVBAMTJVVTRVJUcnVzdCBSU0EgQ2Vy
dGlmaWNhdGlvbiBBdXRob3JpdHkwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIK
AoICAQCAEmUXNg7D2wiz0KxXDXbtzSfTTK1Qg2HiqiBNCS1kCdzOiZ/MPans9s/B
3PHTsdZ7NygRK0faOca8Ohm0X6a9fZ2jY0K2dvKpOyuR+OJv0OwWIJAJPuLodMkY
tJHUYmTbf6MG8YgYapAiPLz+E/CHFHv25B+O1ORRxhFnRghRy4YUVD+8M/5+bJz/
Fp0YvVGONaanZshyZ9shZrHUm3gDwFA66Mzw3LyeTP6vBZY1H1dat//O+T23LLb2
VN3I5xI6Ta5MirdcmrS3ID3KfyI0rn47aGYBROcBTkZTmzNg95S+UzeQc0PzMsNT
79uq/nROacdrjGCT3sTHDN/hMq7MkztReJVni+49Vv4M0GkPGw/zJSZrM233bkf6
c0Plfg6lZrEpfDKEY1WJxA3Bk1QwGROs0303p+tdOmw1XNtB1xLaqUkL39iAigmT
Yo61Zs8liM2EuLE/pDkP2QKe6xJMlXzzawWpXhaDzLhn4ugTncxbgtNMs+1b/97l
c6wjOy0AvzVVdAlJ2ElYGn+SNuZRkg7zJn0cTRe8yexDJtC/QV9AqURE9JnnV4ee
UB9XVKg+/XRjL7FQZQnmWEIuQxpMtPAlR1n6BB6T1CZGSlCBst6+eLf8ZxXhyVeE
Hg9j1uliutZfVS7qXMYoCAQlObgOK6nyTJccBz8NUvXt7y+CDwIDAQABo0IwQDAd
BgNVHQ4EFgQUU3m/WqorSs9UgOHYm8Cd8rIDZsswDgYDVR0PAQH/BAQDAgEGMA8G
A1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEMBQADggIBAFzUfA3P9wF9QZllDHPF
Up/L+M+ZBn8b2kMVn54CVVeWFPFSPCeHlCjtHzoBN6J2/FNQwISbxmtOuowhT6KO
VWKR82kV2LyI48SqC/3vqOlLVSoGIG1VeCkZ7l8wXEskEVX/JJpuXior7gtNn3/3
ATiUFJVDBwn7YKnuHKsSjKCaXqeYalltiz8I+8jRRa8YFWSQEg9zKC7F4iRO/Fjs
8PRF/iKz6y+O0tlFYQXBl2+odnKPi4w2r78NBc5xjeambx9spnFixdjQg3IM8WcR
iQycE0xyNN+81XHfqnHd4blsjDwSXWXavVcStkNr/+XeTWYRUc+ZruwXtuhxkYze
Sf7dNXGiFSeUHM9h4ya7b6NnJSFd5t0dCy5oGzuCr+yDZ4XUmFF0sbmZgIn/f3gZ
XHlKYC6SQK5MNyosycdiyA5d9zZbyuAlJQG03RoHnHcAP9Dc1ew91Pq7P8yF1m9/
qS3fuQL39ZeatTXaw2ewh0qpKJ4jjv9cJ2vhsE/zB+4ALtRZh8tSQZXq9EfX7mRB
VXyNWQKV3WKdwrnuWih0hKWbt5DHDAff9Yk2dDLWKMGwsAvgnEzDHNb842m1R0aB
L6KCq9NjRHDEjf8tM7qtj3u1cIiuPhnPQCjY/MiQu12ZIvVS5ljFH4gxQ+6IHdfG
jjxDah2nGN59PRbxYvnKkKj9
-----END CERTIFICATE-----)";

// ensure configuration has been set
#define REQUIRE_SET(var, msg) static_assert(sizeof(var) / sizeof(var[0]) > 1, msg);

REQUIRE_SET(WiFiSSID, "Set your WiFi networks name in WiFiSSID");
REQUIRE_SET(WiFiPassword, "Set your WiFi  password in WiFiPassword");

REQUIRE_SET(ApiKey, "Specify OWM API key");

REQUIRE_SET(City, "Specify City name");
REQUIRE_SET(Latitude, "Specify Latitude coord");
REQUIRE_SET(Longitude, "Specify Longitude coord");
REQUIRE_SET(NTPServer, "NTP server must be set");
REQUIRE_SET(ApiServer, "API Server must be set");
static_assert(MaxDrift < RefreshPeriod, "MaxDrift should be strictly less than RefreshPeriod");

} // namespace cfg
