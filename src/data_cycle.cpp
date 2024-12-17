/*
 * Copyright (c) David Bird 2021. All rights to this software are reserved.
 * Copyright (c) 2024 zahical. All rights reserved.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/**
 * @file Client for the OpenWeatherMap API.
 */
#include "data_cycle.h"
#include "common.h"
#include "config.h"
#include "data_fetcher.h"
#include "shared_data.h"
#include "timings.h"

#include <ArduinoJson.h>
#include <ArduinoJson/Deserialization/Reader.hpp>

#include <cassert>
#include <cmath>

namespace
{
bool has_forecast = false;
bool has_weather  = false;
bool has_onecall  = false;

enum class ApiCall : byte
{
    OneCall  = 0,
    Weather  = 1,
    Forecast = 2,
    AQI      = 3
};

constexpr char const *api_names[] = {"onecall", "weather", "forecast", "air_pollution"};

OpResult<String> call_owm_api(ApiCall api)
{
    String Language = "EN"; // currently not configurable

    String uri = "/data/2.5/" + String(api_names[(byte)api]) + "?lat=" + cfg::Latitude + "&lon=" + cfg::Longitude +
                 "&appid=" + cfg::ApiKey;

    if (api != ApiCall::AQI)
        uri += "&mode=json&units=" + String(cfg::UseMetricUnits ? "metric" : "imperial") + "&lang=" + Language;
    if (api == ApiCall::OneCall) uri += "&exclude=minutely,hourly,alerts,daily";

    int attempt = 0;

    do
    {
        auto result = get_url(cfg::ApiServer, uri);
        if (result || (++attempt == cfg::MaxHTTPRetries)) return result;
    } while (true);
}

float mm_to_inches(float value_mm) { return 0.0393701 * value_mm; }

float hPa_to_inHg(float value_hPa) { return 0.02953 * value_hPa; }

void Convert_Readings_to_Imperial()
{
    // TODO bug? WxConditions vs WxForecast[0]
    using namespace shared;

    // Only the first 3-hours are used
    WxConditions.Pressure  = hPa_to_inHg(WxConditions.Pressure);
    WxForecast[0].Rainfall = mm_to_inches(WxForecast[0].Rainfall);
    WxForecast[0].Snowfall = mm_to_inches(WxForecast[0].Snowfall);
}

void Log_Entry(bool current, String const& name, Forecast_record_type const& e)
{
    Serial.println("Period: " + name + " -----");

    if (current)
    {
        Serial.println("Temp: " + String(e.Temperature));
        Serial.println("FLik: " + String(e.FeelsLike));
        Serial.println("Pres: " + String(e.Pressure));
        Serial.println("Humi: " + String(e.Humidity));
        // Serial.println("DPoi: " + String(e.DewPoint));
        // Serial.println("UVin: " + String(e.UVI));
        Serial.println("CCov: " + String(e.Cloudcover));
        Serial.println("Visi: " + String(e.Visibility));
        Serial.println("WSpd: " + String(e.Windspeed));
        Serial.println("WDir: " + String(e.Winddir));
        Serial.println("Fore: " + String(e.Forecast0));
        Serial.println("Icon: " + String(e.Icon));
        Serial.println("Rain: " + String(e.Rainfall)); //
    }
    else
    {
        Serial.println("DtUx: " + String(e.Dt));
        Serial.println("Temp: " + String(e.Temperature));
        Serial.println("TLow: " + String(e.Low));  //
        Serial.println("THig: " + String(e.High)); //
        Serial.println("Pres: " + String(e.Pressure));
        Serial.println("Humi: " + String(e.Humidity));
        Serial.println("Icon: " + String(e.Icon));
        Serial.println("Rain: " + String(e.Rainfall)); //
        Serial.println("Snow: " + String(e.Snowfall)); //
    }
}

void Log_Entry(Air_quality_record const& e)
{
    Serial.println("Dt  : " + String(e.Dt));
    Serial.println("AQI : " + String(e.AQI));
    Serial.println("CO  : " + String(e.CO));
    Serial.println("NO  : " + String(e.NO));
    Serial.println("NO2 : " + String(e.NO2));
    Serial.println("O3  : " + String(e.O3));
    Serial.println("SO2 : " + String(e.SO2));
    Serial.println("PM25: " + String(e.PM2_5));
    Serial.println("PM10: " + String(e.PM10));
    Serial.println("NH3 : " + String(e.NH3));
}

void Log_Entry(LocData_type const& e)
{
    Serial.println("SRis: " + String(e.Sunrise));
    Serial.println("SSet: " + String(e.Sunset));
}

} // namespace

#ifdef HOST_BUILD
// The ArduinoJson does not immediately recognize the mocked String type we use in host build, so
// several adapters are necessary
void convertFromJson(JsonVariantConst src, String& dst) { dst = String{src.as<char const *>()}; }

namespace ARDUINOJSON_NAMESPACE
{
template <>
struct Reader<::String> : BoundedReader<char const *>
{
    explicit Reader(::String const& s) : BoundedReader<char const *>(s.c_str(), s.length()) {}
};
} // namespace ARDUINOJSON_NAMESPACE
#endif

void postprocess_weather_data()
{
    assert(has_forecast && (has_onecall || has_weather));
    static_assert(max_readings >= 8);

    using namespace shared;
    // clang-format off

    for (byte r = 0; r < 8; r++) { // Check next 3 x 8 Hours = 1 day
      if (WxForecast[r].High > WxConditions.High) WxConditions.High = WxForecast[r].High; // Get Highest temperature for next 24Hrs
      if (WxForecast[r].Low  < WxConditions.Low)  WxConditions.Low  = WxForecast[r].Low;  // Get Lowest  temperature for next 24Hrs
    }

    float pressure_trend = WxForecast[0].Pressure - WxForecast[2].Pressure; // Measure pressure slope between ~now and later
    pressure_trend = ((int)(pressure_trend * 10)) / 10.0; // Remove any small variations less than 0.1
    WxConditions.Trend = "=";
    if (pressure_trend > 0)  WxConditions.Trend = "+";
    if (pressure_trend < 0)  WxConditions.Trend = "-";
    if (pressure_trend == 0) WxConditions.Trend = "0";

    if (!cfg::UseMetricUnits) Convert_Readings_to_Imperial();
    // clang-format on
}

OpResult<void> populate_from_onecall_api_data(String const& json)
{
    DynamicJsonDocument doc(64 * 1024);

    DeserializationError error = deserializeJson(doc, json);
    if (error) return op_failed(String("Bad onecall JSON: ") + error.c_str());

    has_onecall = true;
    auto& obj   = shared::WxConditions;

    JsonObject root = doc.as<JsonObject>();

    obj.High = -50; // Minimum forecast low
    obj.Low  = 50;  // Maximum Forecast High

    JsonObject current = doc["current"];

    shared::LocData.Sunrise = current["sunrise"];
    shared::LocData.Sunset  = current["sunset"];

    obj.Temperature = current["temp"];
    obj.FeelsLike   = current["feels_like"];
    obj.Pressure    = current["pressure"];
    obj.Humidity    = current["humidity"];
    obj.DewPoint    = current["dew_point"];
    obj.UVI         = current["uvi"];
    obj.Cloudcover  = current["clouds"];
    obj.Visibility  = current["visibility"];
    obj.Windspeed   = current["wind_speed"];
    obj.Winddir     = current["wind_deg"];

    JsonObject current_weather = current["weather"][0];

    String Description = current_weather["description"]; // "scattered clouds"
    String Icon        = current_weather["icon"];        // "01n"
    obj.Forecast0      = Description;
    obj.Icon           = Icon;

    return {};
}

OpResult<void> populate_from_weather_api_data(String const& json)
{
    DynamicJsonDocument doc(64 * 1024);

    DeserializationError error = deserializeJson(doc, json);
    if (error) return op_failed(String("Bad weather JSON: ") + error.c_str());

    has_weather = true;
    auto& obj   = shared::WxConditions;

    JsonObject root = doc.as<JsonObject>();

    obj.High     = -1000; // Minimum forecast low
    obj.Low      = 1000;  // Maximum Forecast High
    obj.UVI      = NAN;
    obj.DewPoint = NAN;

    shared::LocData.Sunrise = doc["sys"]["sunrise"];
    shared::LocData.Sunset  = doc["sys"]["sunset"];

    JsonObject main = doc["main"];
    obj.Temperature = main["temp"];
    obj.FeelsLike   = main["feels_like"];
    obj.Pressure    = main["pressure"];
    obj.Humidity    = main["humidity"];

    obj.Cloudcover = doc["clouds"]["all"];
    obj.Visibility = doc["visibility"];
    obj.Windspeed  = doc["wind"]["speed"];
    obj.Winddir    = doc["wind"]["deg"];
    obj.Rainfall   = doc["rain"]["1h"];

    JsonObject weather = doc["weather"][0];
    obj.Forecast0      = weather["description"].as<String>();
    obj.Icon           = weather["icon"].as<String>();

    OPT_LOG(Log_Payloads, Log_Entry(true, "Current", shared::WxConditions));
    OPT_LOG(Log_Payloads, Log_Entry(shared::LocData));

    return {};
}

OpResult<void> populate_from_forecast_api_data(String const& json)
{
    DynamicJsonDocument doc(64 * 1024);

    DeserializationError error = deserializeJson(doc, json);
    if (error) return op_failed(String("Bad forecast JSON: ") + error.c_str());

    has_forecast = true;

    JsonObject root = doc.as<JsonObject>();

    JsonArray list = root["list"];

    for (byte r = 0; r < max_readings; r++)
    {
        auto en   = list[r];
        auto& obj = shared::WxForecast[r];

        obj.Dt          = en["dt"].as<int>();
        obj.Temperature = en["main"]["temp"].as<float>();
        obj.Low         = en["main"]["temp_min"].as<float>();
        obj.High        = en["main"]["temp_max"].as<float>();
        obj.Pressure    = en["main"]["pressure"].as<float>();
        obj.Humidity    = en["main"]["humidity"].as<float>();
        obj.Icon        = en["weather"][0]["icon"].as<char const *>();
        obj.Rainfall    = en["rain"]["3h"].as<float>();
        obj.Snowfall    = en["snow"]["3h"].as<float>();

        OPT_LOG(Log_Payloads, Log_Entry(false, "Forecast-" + String(r) + " " + en["dt_txt"].as<char const *>(), obj));
    }

    return {};
}

OpResult<void> populate_from_aqi_api_data(String const& json)
{
    DynamicJsonDocument doc(64 * 1024);

    DeserializationError error = deserializeJson(doc, json);
    if (error) return op_failed(String("Bad AQI JSON: ") + error.c_str());

    auto& obj = shared::WxAirQ;

    JsonObject root = doc.as<JsonObject>();

    JsonArray list = root["list"];

    auto en = list[0];

    obj.Dt  = en["dt"];
    obj.AQI = en["main"]["aqi"];

    auto c = en["components"].as<JsonObject>();

    obj.CO    = c["co"];
    obj.NO    = c["no"];
    obj.NO2   = c["no2"];
    obj.O3    = c["o3"];
    obj.SO2   = c["so2"];
    obj.PM2_5 = c["pm2_5"];
    obj.PM10  = c["pm10"];
    obj.NH3   = c["nh3"];

    OPT_LOG(Log_Payloads, Log_Entry(obj));

    return {};
}

namespace
{
OpResult<void> do_data_cycle_core()
{
    OPT_LOG(Log_Lifecycle, Serial.println("Fetching current weather data..."););

    mark_event(TimeEvent::FetchWeather);
    auto weather = call_owm_api(ApiCall::Weather);
    mark_event_done(TimeEvent::FetchWeather);
    if (!weather) return op_failed(weather.error());

    mark_event(TimeEvent::ParseWeather);
    auto parse_weather = populate_from_weather_api_data(*std::move(weather));
    mark_event_done(TimeEvent::ParseWeather);
    if (!parse_weather) return parse_weather;

    OPT_LOG(Log_Lifecycle, Serial.println("Fetching weather forecast data..."););

    mark_event(TimeEvent::FetchForecast);
    auto forecast = call_owm_api(ApiCall::Forecast);
    mark_event_done(TimeEvent::FetchForecast);
    if (!forecast) return op_failed(forecast.error());

    mark_event(TimeEvent::ParseForecast);
    auto parse_forecast = populate_from_forecast_api_data(*std::move(forecast));
    mark_event_done(TimeEvent::ParseForecast);
    if (!parse_forecast) return parse_forecast;

    OPT_LOG(Log_Lifecycle, Serial.println("Fetching current AQI data..."););

    mark_event(TimeEvent::FetchAQI);
    auto aqi = call_owm_api(ApiCall::AQI);
    mark_event_done(TimeEvent::FetchAQI);
    if (!aqi) return op_failed(aqi.error());

    mark_event(TimeEvent::ParseAQI);
    auto parse_aqi = populate_from_aqi_api_data(*std::move(aqi));
    mark_event_done(TimeEvent::ParseAQI);
    if (!parse_aqi) return parse_aqi;

    postprocess_weather_data();

    return {};
}
} // namespace

OpResult<void> do_data_cycle()
{
    auto r = do_data_cycle_core();

    {
        AutoTiming timer{TimeEvent::CloseHttp};
        reset_http(); // be a good citizen and send tcp close to the server
    }

    return r;
}
