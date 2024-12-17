/*
 * Copyright (c) 2024 zahical. All rights reserved.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/**
 * @file Support for measuring the durations of refresh cycle phases.
 */

#pragma once
#include <chrono>

/**
 * @brief The type of the phase.
 */
enum class TimeEvent : unsigned char
{
    /// e2e timing
    PowerCycle,
    ConnectWiFi,
    SetUpNTP,
    SyncTime,
    FetchWeather,
    ParseWeather,
    FetchForecast,
    ParseForecast,
    FetchAQI,
    ParseAQI,
    CloseHttp,
    DrawUI,
    PowerOnScreen,
    ClearScreen,
    UpdateScreen,
    _MAX
};

/**
 * @brief Marks the start of phase.
 *
 * Should only be called once per event type.
 */
void mark_event(TimeEvent e);

/**
 * @brief Marks the end of phase.
 *
 * Should only be called once after mark_event() has already been called for the same event.
 */
void mark_event_done(TimeEvent e);

/**
 * @brief Get the duration of a phase, in millis resolution.
 *
 * Returns 0 of mark_event() or mark_event_done() has not been called for that event.
 */
std::chrono::milliseconds get_event_duration(TimeEvent e);

/**
 * @brief Dumps all recorded (non-zero) duration using Serial.print();
 */
void dump_timings();

/**
 * @brief Helper type to automatically mark()/mark_done() an event for a C++ scope.
 *
 * Remember to give the instance a name, e.g. `AutoTiming time{TimeEvent::DrawUI}`, and
 * _not_ `AutoTiming{TimeEvent::DrawUI}` so that the temporary is not immediately destroyed.
 */
class AutoTiming
{
  public:
    AutoTiming(TimeEvent e) : _e(e) { mark_event(e); }

    ~AutoTiming() { mark_event_done(_e); }

  private:
    TimeEvent _e;
};