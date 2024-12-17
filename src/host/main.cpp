/*
 * Copyright (c) 2024 zahical. All rights reserved.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/**
 * @file Entrypoint when running the on-host emulator.
 */

#include "config.h"
#include "data_cycle.h"
#include "display.h"
#include "shared_data.h"

#include "framebuffer.h"
#include "timings.h"

#include <cassert>
#include <chrono>

#include <cstdlib>
#include <cstring>
#include <ctime>

// provided by host data_fetcher
void http_use_mock_data();

void run_with_live_data()
{
    // init shared time-related data from local time and time-zone.
    time_t timer = time(NULL);
    localtime_r(&timer, &shared::CycleStart);

    strncpy(shared::TZName, shared::CycleStart.tm_zone, (sizeof(shared::TZName) / sizeof(shared::TZName[0])) - 1);
    shared::time_offset = std::chrono::seconds{shared::CycleStart.tm_gmtoff};

    // run Scheduler for shared::ActiveHours
    DailyTime now{shared::CycleStart};
    auto [sleep, active] = Scheduler{cfg::OnTime, cfg::OffTime, cfg::RefreshPeriod, cfg::MaxDrift}.plan_sleep(now);

    shared::ActiveHours = active;
    Serial.println("Planned sleep = " + String(std::chrono::seconds{sleep}.count()) + " seconds.");
}

void run_with_mock_data()
{
    shared::CycleStart = {
        .tm_sec  = 19,
        .tm_min  = 31,
        .tm_hour = 2,
        .tm_mday = 20,
        .tm_mon  = 10,
        .tm_year = 124,

        .tm_wday  = 6,
        .tm_isdst = true,

        // the rest of tm's fields are not used by ui code
    };
    strcpy(shared::TZName, "EEST");
    shared::time_offset = std::chrono::hours{2};
    shared::ActiveHours = false;

    http_use_mock_data();
}

int main()
{
    // if false, mock data from test_data.cpp is used;
    // if true, data is fetched by actually calling the OWM API.
    bool do_live = false;

    InitGraphics();
    // zebra();

    shared::voltage     = 3.9; // V
    shared::wifi_signal = -45; // dbm

    if (do_live)
        run_with_live_data();
    else
        run_with_mock_data();

    auto r = do_data_cycle();

    if (r)
        DisplayWeather();
    else
        DisplayError(r.error());

    save_fb();

    if (do_live) dump_timings();
}