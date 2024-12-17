/*
 * Copyright (c) David Bird 2021. All rights to this software are reserved.
 * Copyright (c) 2024 zahical. All rights reserved.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/**
 * @file Entry point when running on the ESP32 LilyGo hardware.
 */

#include "app_ver.h"
#include "common.h"
#include "config.h"
#include "data_cycle.h"
#include "display.h"
#include "schedule.h"
#include "shared_data.h"
#include "timings.h"

#include <AceTime.h>   // AceTime TZ library
#include <utilities.h> // for BATT_PIN; a LilyGo-EPD47 header

#include <Arduino.h>
#include <WiFi.h>

#include <esp32-hal-adc.h>
#include <esp_sleep.h>

#include <chrono>
#include <cstring>
#include <ctime>
#include <tuple>

void StopWiFi(); // forward

[[noreturn]]
void BeginSleep(std::chrono::seconds planned_sleep)
{
    mark_event_done(TimeEvent::PowerCycle);

    StopWiFi();

    auto powered_time = get_event_duration(TimeEvent::PowerCycle);

    OPT_LOG(Log_Lifecycle, Serial.println("Refresh cycle completed."));
    OPT_LOG(Log_Timings, dump_timings());

    if (Log_Lifecycle)
    {
        Serial.printf("Heap: free(%d) max_alloc(%d) min_free(%d)\n", ESP.getFreeHeap(), ESP.getMaxAllocHeap(),
                      ESP.getMinFreeHeap());
        Serial.printf("PSRAM: free(%d) max_alloc(%d) min_free(%d)\n", ESP.getFreePsram(), ESP.getMaxAllocPsram(),
                      ESP.getMinFreePsram());

        Serial.println("Awake for : " + String(powered_time.count() / 1000.0, 3) + " seconds.");
        DailyTime hr{planned_sleep};
        Serial.printf("Preparing to sleep for %02d:%02d:%02d (%d seconds)\n", hr.hours(), hr.minutes(), hr.seconds(),
                      (uint32_t)planned_sleep.count());
    }
    OPT_LOG(Log_Lifecycle, Serial.println("Shutting down..."));

    // Some ESP32 have a RTC that is too fast to maintain accurate time, so add an offset
    long SleepTimerSec = std::chrono::seconds(planned_sleep + cfg::ExtraSleep).count();

    esp_sleep_enable_timer_wakeup(SleepTimerSec * 1000000LL); // in Secs, 1000000LL converts to Secs as unit = 1uSec
    esp_deep_sleep_start();
}

OpResult<void> SetupTime()
{
    {
        AutoTiming timing{TimeEvent::SetUpNTP};

        // pass UTC/no DST, as we'll be handling time zone stuff using the AceTime library
        configTime(0, 0, cfg::NTPServer, "time.nist.gov");
    }

    OPT_LOG(Log_Lifecycle, Serial.println("Syncing NTP time..."));
    {
        AutoTiming timing{TimeEvent::SyncTime};

        delay(50); // after NTP is configured, give it a bit of time to sync, before we start polling.

        // Wait for for time to synchronize
        if (!getLocalTime(&shared::CycleStart, std::chrono::milliseconds(cfg::MaxNTPSyncWait).count()))
        {
            OPT_LOG(Log_Lifecycle, Serial.println("Failed to obtain time"));
            return op_failed("Can't sync NTP time.");
        }
    }

    time_t now = time(nullptr);

    ace_time::ExtendedZoneProcessor zoneProcessor;
    ace_time::TimeZone tz       = ace_time::TimeZone::forZoneInfo(&cfg::TZ, &zoneProcessor);
    ace_time::ZonedDateTime zdt = ace_time::ZonedDateTime::forUnixSeconds64(now, tz);
    ace_time::ZonedExtra ze     = tz.getZonedExtra(zdt.localDateTime());

    // fill in time-related fields in shared data
    auto& tm    = shared::CycleStart;
    tm.tm_year  = zdt.year() - 1900;
    tm.tm_mon   = zdt.month() - 1;
    tm.tm_mday  = zdt.day();
    tm.tm_hour  = zdt.hour();
    tm.tm_min   = zdt.minute();
    tm.tm_sec   = zdt.second();
    tm.tm_wday  = zdt.dayOfWeek() % 7;
    tm.tm_isdst = !ze.dstOffset().isZero();

    strcpy(shared::TZName, ze.abbrev());
    shared::time_offset = std::chrono::seconds{ze.timeOffset().toSeconds()};

    OPT_LOG(Log_Lifecycle, Serial.print("Time set to: "); Serial.print(&shared::CycleStart, "%a %b %d %Y   %H:%M:%S");
            Serial.printf(" [%s:", ze.abbrev()); ze.timeOffset().printTo(Serial);
            if (!ze.dstOffset().isZero()) Serial.print(" DST"); Serial.println("]"));

    return {};
}

OpResult<void> StartWiFi()
{
    OPT_LOG(Log_Lifecycle, Serial.println("Connecting to: " + String(cfg::WiFiSSID)));
    {
        AutoTiming timing{TimeEvent::ConnectWiFi};

        WiFi.mode(WIFI_STA);
        WiFi.setAutoConnect(true);
        WiFi.setAutoReconnect(true);
        WiFi.begin(cfg::WiFiSSID, cfg::WiFiPassword);

        int attempts = cfg::MaxWiFiConnectAttempts;

        while (WiFi.waitForConnectResult() != WL_CONNECTED)
        {
            if (--attempts > 0) break;
            OPT_LOG(Log_Lifecycle, Serial.println("STA: Failed (" + String(WiFi.status()) +
                                                  ")! Attempts remaining: " + String(attempts)));

            WiFi.disconnect(false);
            delay(500);
            WiFi.begin(cfg::WiFiSSID, cfg::WiFiPassword);
        }
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        // Get Wifi Signal strength now, because the WiFi will be turned off to save power!
        shared::wifi_signal = WiFi.RSSI();
        OPT_LOG(Log_Lifecycle, Serial.println("WiFi connected at: " + WiFi.localIP().toString() +
                                              " RSSI: " + String(shared::wifi_signal)));
        return {};
    }
    else
    {
        OPT_LOG(Log_Lifecycle, Serial.println("WiFi connection *** FAILED *** " + String(WiFi.status())););
        return op_failed(String(cfg::WiFiSSID) + " WiFi failed: " + String(WiFi.status()));
    }
}

void StopWiFi()
{
    OPT_LOG(Log_Lifecycle, Serial.print("Switching off WiFi. Current mode is "); Serial.println(WiFi.getMode()));
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}

void InitializeSystem()
{
    mark_event(TimeEvent::PowerCycle);

    Serial.begin(115200);
    while (!Serial)
        delay(1);

    if (Log_Lifecycle)
    {
        Serial.println("Starting...");
        Serial.println(String("Ver: ") + shared::app_ver);
        Serial.println(String("Build: ") + String(__FILE__));
        Serial.println(String("SDK Ver: ") + ESP.getSdkVersion());
        Serial.println(String("ChipID: ") + ESP.getChipModel() + " rev: " + ESP.getChipRevision());
        Serial.printf("Heap: size(%d) free(%d)\n", ESP.getHeapSize(), ESP.getFreeHeap());
        Serial.printf("PSRAM: size(%d) free(%d)\n", ESP.getPsramSize(), ESP.getFreePsram());
    }

    if (!InitGraphics())
    {
        // Can't init display, no point in continuing
        Serial.println("Failed to init EPD, shutting down.");
        esp_deep_sleep_start(); // does not return
    }
}

void MeasureBattery()
{
    analogSetPinAttenuation(BATT_PIN, ADC_11db);

    // According to the schematics, the BATT_PIN is connected to the middle of a 1:1 voltage divider,
    // however, instead of doubling the result, we take two readings, as a rudimentary multi-sampling.
    // analogReadMilliVolts() is documented to return an already calibrated result.
    auto cal_attn1 = analogReadMilliVolts(BATT_PIN);
    delay(10);
    auto cal_attn2 = analogReadMilliVolts(BATT_PIN);

    auto cal_avg = (cal_attn1 + cal_attn2) / 1000.0;

    shared::voltage = cal_avg;

    OPT_LOG(Log_Lifecycle, Serial.println(String("Battery: ADC S1 = ") + cal_attn1 + " ADC S2 =" + cal_attn2 +
                                          " Voltage = " + String(shared::voltage)));
}

void loop()
{
    // Nothing to do here
}

void setup()
{
    InitializeSystem();

    std::chrono::seconds planned_sleep{cfg::RefreshPeriod};

    {
        auto has_wifi = StartWiFi();
        if (!has_wifi)
        {
            DisplayError(has_wifi.error());
            goto done;
        }
    }

    {
        auto has_time = SetupTime();
        if (!has_time)
        {
            DisplayError(has_time.error());
            goto done;
        }
    }

    std::tie(planned_sleep, shared::ActiveHours) =
        Scheduler{cfg::OnTime, cfg::OffTime, cfg::RefreshPeriod, cfg::MaxDrift}.plan_sleep(shared::CycleStart);

    {
        auto data_ok = do_data_cycle();
        StopWiFi(); // Reduces power consumption

        if (data_ok)
        {
            MeasureBattery();
            DisplayWeather();
        }
        else { DisplayError(data_ok.error()); }
    }

done:

    BeginSleep(planned_sleep);
}
