/*
 * Copyright (c) David Bird 2021. All rights to this software are reserved.
 * Copyright (c) 2024 zahical. All rights reserved.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/**
 * @file Declarations of user-configurable values.
 */
#include "aqi_metric.h"
#include "schedule.h"

#ifndef HOST_BUILD
#include <zoneinfo/infos.h>
#endif

constexpr bool Log_Lifecycle = true;
constexpr bool Log_Timings   = true;
constexpr bool Log_HttpErrs  = false;
constexpr bool Log_HttpReq   = false;
constexpr bool Log_HttpBody  = false;
constexpr bool Log_Payloads  = false;
/// Draws the timing in the UI
constexpr bool Log_DrawTimings = false;

namespace cfg
{
extern char const WiFiSSID[];
extern char const WiFiPassword[];
extern char const ApiKey[];
extern char const Latitude[];
extern char const Longitude[];
extern char const City[];
#ifndef HOST_BUILD
extern ace_time::extended::ZoneInfo const& TZ;
#endif
extern bool const UseMetricUnits;

extern DailyTime const OnTime;
extern DailyTime const OffTime;
extern std::chrono::seconds const RefreshPeriod;
extern AQIndicator const AQIComponents[3];

extern char const NTPServer[];
extern std::chrono::seconds const MaxDrift;
extern std::chrono::seconds const ExtraSleep;
extern std::chrono::seconds const MaxNTPSyncWait;
extern unsigned const MaxWiFiConnectAttempts;
extern unsigned const MaxHTTPRetries;
extern char const ApiServer[];

extern bool const UseHTTPS;
extern char const *const OWM_ROOT_CA;

} // namespace cfg