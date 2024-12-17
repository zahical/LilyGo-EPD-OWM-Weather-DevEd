/*
 * Copyright (c) David Bird 2021. All rights to this software are reserved.
 * Copyright (c) 2024 zahical. All rights reserved.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/**
 * @file Definition of globals shared between OWM API query code, drawing code and main.cpp.
 */
#pragma once

#include <Arduino.h> // for String
#include <chrono>
#include <ctime>

typedef struct
{
    /// UTC; UNIX timestamp seconds
    int Sunrise;
    /// UTC; UNIX timestamp seconds
    int Sunset;
} LocData_type;

/**
 * @brief Air Pollution API data
 * See https://openweathermap.org/api/air-pollution
 */
typedef struct
{
    int Dt;  // Date and time, Unix timestamp, UTC
    int AQI; // Air Quality Index; 1=Good...5=Very poor

    // all amounts in ug/m^3
    float CO;    // Carbon monoxide
    float NO;    // Nitrogen monoxide
    float NO2;   // Nitrogen dioxide
    float O3;    // Ozone
    float SO2;   // Sulphur dioxide
    float PM2_5; // Fine particles matter
    float PM10;  // Coarse particulate matter
    float NH3;   // Ammonia
} Air_quality_record;

/**
 * @brief Current and forecast weather API data.
 * See https://openweathermap.org/current
 * and https://openweathermap.org/forecast5
 */
typedef struct
{
    /// Date and time, Unix timestamp, UTC
    int Dt;

    /// See https://openweathermap.org/weather-conditions
    String Icon;

    /// Atmospheric pressure trend: "+", "-" or "0"
    String Trend;

    /// Actually, a description. Localized. See https://openweathermap.org/weather-conditions
    String Forecast0;
    float Temperature;
    float FeelsLike;
    float DewPoint;
    float Humidity;
    float High;
    float Low;
    float Winddir;
    float Windspeed;
    float Rainfall;
    float Snowfall;
    float Pressure;
    int Cloudcover;
    int Visibility;
    /// Ultra Violet Index; currently unused (not provided by API)
    float UVI;
} Forecast_record_type;

constexpr auto max_readings = 24; // Limited to 3-days here, but could go to 5-days = 40 as the data is issued

namespace shared
{
// Data provided by the OWM API -----------

/// Current weather
extern Forecast_record_type WxConditions;
/// 3 hour forecast for the next 3 days
extern Forecast_record_type WxForecast[max_readings];
// Sunrise & Sunset
extern LocData_type LocData;
// Air Quality
extern Air_quality_record WxAirQ;

// Data populated by main.cpp ------------------

/// The start of the current refresh cycle, in the local time zone, as specified by cfg::TZ in config.cpp
extern struct tm CycleStart;

/// The short name, e.g. EEST, PST, of the current time time
extern char TZName[7]; // 7 = ace_time::internal::kAbbrevSize but it's not available in host build

/// The time offset, in seconds of the local time zone. Includes DST, if in effect.
/// (can't use struct tm::tm_gmtoff for that, as it's a GNU ext, not present on Arduino)
extern std::chrono::seconds time_offset;

/// Whether the station is in "On" mode, e.g. it will refresh according to the cfg::RefreshPeriod.
extern bool ActiveHours;

/// the wifi power level, in dBm
/// see: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/wifi.html#station-basic-configuration
// from -127 to 0 (or slightly positive)
// see
// https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/network/esp_wifi.html#structwifi__ap__record__t//
extern int wifi_signal;

/// Voltage, in V, on BATT_PIN.
extern float voltage;

} // namespace shared