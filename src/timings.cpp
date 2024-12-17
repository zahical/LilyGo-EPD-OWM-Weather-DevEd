/*
 * Copyright (c) 2024 zahical. All rights reserved.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/**
 * @file Timings implementation.
 */ 

#include "timings.h"
#include <Arduino.h>
#include <cassert>
#include <chrono>
#include <type_traits>

using std::chrono::milliseconds;

namespace
{

// not available in Arduino's C++ version
template <typename E>
constexpr typename std::underlying_type<E>::type to_val(E e) noexcept
{
    return static_cast<typename std::underlying_type<E>::type>(e);
}

using timing_t = unsigned long; // ret type of millis()

struct Timing
{
    char const *const name;
    timing_t start;
    timing_t end;
};

// clang-format off
static Timing timings[] =
{
    {.name = "Power cycle"},
    {.name = "Connect WiFi"},
    {.name = "Set up NTP"},
    {.name = "Sync time"},
    {.name = "Fetch weather"},
    {.name = "Parse weather"},
    {.name = "Fetch forecast"},
    {.name = "Parse forecast"},
    {.name = "Fetch AQI"},
    {.name = "Parse AQI"},
    {.name = "Close HTTP"},
    {.name = "Draw UI"},
    {.name = "Power-on screen"},
    {.name = "Clear screen"},
    {.name = "Update screen"},
};
// clang-format on

static_assert(sizeof(timings) / sizeof(timings[0]) == to_val(TimeEvent::_MAX),
              "timings[] should be the same size (and in the same order) as enum class TimeEvent.");

} // namespace

void mark_event(TimeEvent e)
{
    assert(timings[to_val(e)].start == 0);
    timings[to_val(e)].start = millis();
}

void mark_event_done(TimeEvent e)
{
    assert(timings[to_val(e)].start != 0);
    timings[to_val(e)].end = millis();
}

milliseconds get_event_duration(TimeEvent e)
{
    auto start = timings[to_val(e)].start;
    auto end   = timings[to_val(e)].end;

    return start == 0 || end == 0 ? milliseconds::zero() : milliseconds{end - start};
}

void dump_timings()
{
    for (size_t i = 0; i < sizeof(timings) / sizeof(timings[0]); i++)
    {
        auto start = timings[i].start;
        auto end   = timings[i].end;
        if (start == 0 || end == 0) continue;

        // printf() is not mocked
        Serial.print(timings[i].name);
        Serial.print(": ");
        Serial.print(String(end - start));
        Serial.println(" millis");
    }
}