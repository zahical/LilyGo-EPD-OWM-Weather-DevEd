/*
 * Copyright (c) 2024 zahical. All rights reserved.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/**
 * @file Scheduler support: DailyTime and Scheduler.
 */
#pragma once
#include <cassert>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <tuple>

#ifdef HOST_BUILD
#include <iomanip>
#include <iostream>
#endif

/**
 * @brief Describes a time-of-day moment with a seconds resolution.
 * Implements modular "wrap at midnight" arithmetic.
 */
class DailyTime
{
    using csec = std::chrono::seconds;

  public:
    constexpr DailyTime(uint8_t hours, uint8_t minutes = 0, uint8_t seconds = 0)
        : DailyTime(std::chrono::hours{hours} + std::chrono::minutes{minutes} + std::chrono::seconds{seconds})
    {
        // assert is not constexpr in C++11
        // assert(hours >= 0 && hours < 24);
        // assert(minutes >= 0 && minutes < 60);
        // assert(seconds >= 0 && seconds < 60);
    }

    constexpr DailyTime(struct tm const& tm) : DailyTime(tm.tm_hour, tm.tm_min, tm.tm_sec) {}

    constexpr DailyTime(csec since_day_start) : _since_day_start(since_day_start % a_day()) {}

    constexpr bool is_between(DailyTime const& start, DailyTime const& endExclusive) const
    {
        return start == endExclusive ? true
                                     : (start < endExclusive ? *this >= start && *this < endExclusive
                                                             : *this >= start || *this < endExclusive);
    }

    constexpr DailyTime operator+(csec period) const { return sec_since_day_start() + period; }

    constexpr csec time_to(DailyTime const& next) const
    {
        return next >= *this ? next.sec_since_day_start() - sec_since_day_start()
                             : sec_till_day_end() + next.sec_since_day_start();
    }

    /**
     * @brief Calculates the seconds between the time-of-day stored in this instance and the next occurrence
     * of a repeating cycle, starting at `start` time-of-day and repeating every `period` seconds.
     *
     * If this instance stores a time-of-day that's an exact multiple of the cycle, 0 is returned.
     */
    csec time_to_next_multiple_of(DailyTime const& start, std::chrono::seconds period) const
    {
        assert(period.count() > 0);

        if (start == *this) return csec{0};

        csec this_sec = sec_since_day_start();

        if (*this < start) this_sec += a_day();

        assert(this_sec > start.sec_since_day_start());

        csec m_diff = this_sec - start.sec_since_day_start();

        auto over = (m_diff % period);
        if (over.count() == 0) return csec{0};

        return period - over;
    }

    constexpr csec sec_since_day_start() const { return _since_day_start; }

    constexpr csec sec_till_day_end() const { return a_day() - sec_since_day_start(); }

    constexpr bool operator<(DailyTime const& rhs) const { return compare(*this, rhs) < 0; }

    constexpr bool operator<=(DailyTime const& rhs) const { return compare(*this, rhs) <= 0; }

    constexpr bool operator>=(DailyTime const& rhs) const { return compare(*this, rhs) >= 0; }

    constexpr bool operator==(DailyTime const& rhs) const { return compare(*this, rhs) == 0; }

    constexpr static int compare(DailyTime const& lhs, DailyTime const& rhs)
    {
        return lhs.sec_since_day_start().count() - rhs.sec_since_day_start().count();
    }

    constexpr uint16_t hours() const { return _since_day_start / std::chrono::hours{1}; }

    constexpr uint16_t minutes() const { return _since_day_start % std::chrono::hours{1} / std::chrono::minutes{1}; }

    constexpr uint16_t seconds() const { return _since_day_start.count() % 60; };

  private:
    csec const _since_day_start; // always < 24h

    static constexpr csec a_day() { return csec{24 * 60 * 60}; }
};

#ifdef HOST_BUILD
// clang-format off
inline std::ostream& operator<<(std::ostream& os, DailyTime const& dt)
{
    os << std::setw(2) << std::setfill('0') << dt.hours()
        << ':' << std::setw(2) << std::setfill('0') << dt.minutes()
        << ':' << std::setw(2) << std::setfill('0') << dt.seconds();
    return os;
}

// clang-format on
#endif

/**
 * @brief Sleep scheduler.
 *
 * A schedule consists of an "OnTime", an "OffTime" and a "RefreshCycle" period duration.
 * The cycle defined by that schedule has an event exactly at OnTime, then another event every RefreshCycle seconds, and
 * then one final event exactly at OffTime. The OffTime does _not_ need to fall on OnTime + (n * RefreshCycle).
 *
 * This type, given a schedule and a current time, calculates the second until the next event of the schedule's cycle.
 *
 * What complicates things a bit is that, as an optimization, the scheduler also supports "event tolerance zone"
 * (defined by MaxDrift). An event tolerance zone is the time period, MaxDrift seconds long, preceding it. So, if
 * the current time is in the tolerance zone of some cycle event, that event is considered to have already occurred, and
 * the schedule calculations are carried out for the next event after it.
 *
 * Note 1: OffTime does not need to be numerically greater than OnTime, i.e. OnTime=23:30 OffTime=00:30 is a valid
 * cycle, starting half an hour before midnight and ending half an hour after midnight.
 *
 * Note 2: OnTime==OffTime defines an "always on" cycle. In that case, the period of RefreshCycle should add up to a
 * full day (24h) otherwise the schedule will be irregular.
 */
struct Scheduler
{

    DailyTime const OnTime;
    DailyTime const OffTime;
    std::chrono::seconds const RefreshCycle;
    std::chrono::seconds const MaxDrift;

    Scheduler(DailyTime OnTime, DailyTime OffTime, std::chrono::seconds RefreshCycle, std::chrono::seconds MaxDrift)
        : OnTime(OnTime), OffTime(OffTime), RefreshCycle(RefreshCycle), MaxDrift(MaxDrift)
    {
    }

    /**
     * @brief Given a schedule and a current time, calculates the time until the next event of the cycle.
     * Also returns whether the time until the next event is "active", that is, after the OnTime but before the OffTime.
     *
     * @param now the current time.
     * @return std::tuple<std::chrono::seconds, bool> a tuple with the time until the next event and whether the cycle
     * is active.
     */
    std::tuple<std::chrono::seconds, bool> plan_sleep(DailyTime now)
    {
        constexpr auto zero = std::chrono::seconds::zero();

        if (OnTime == OffTime)
        {
            // no off-time
            auto next_sleep = now.time_to_next_multiple_of(OnTime, RefreshCycle);

            // we already handled this cycle, so skip it
            if (next_sleep < MaxDrift) next_sleep += RefreshCycle;
            return {next_sleep, true};
        }
        else if (now.is_between(OnTime, OffTime))
        {
            // currently in "on" time
            auto time_to_turn_off = now.time_to(OffTime);
            if (time_to_turn_off < MaxDrift)
            {
                // we are before OffTime but in its tolerance zone;
                // do a combined sleep with the next OffTime
                auto next_cycle = plan_sleep(OffTime);
                return {time_to_turn_off + std::get<0>(next_cycle), std::get<1>(next_cycle)};
            }

            auto time_to_next_cycle = now.time_to_next_multiple_of(OnTime, RefreshCycle);
            if (time_to_next_cycle == zero) time_to_next_cycle = RefreshCycle;

            auto next_cycle_start = now + time_to_next_cycle;

            // std::cout << next_cycle_start << "\n";
            if (next_cycle_start >= OffTime)
            {
                // we need to interrupt the regular schedule as we need a run at the start of OffTime
                return {time_to_turn_off, true};
            }

            if (time_to_next_cycle < MaxDrift)
            {
                // we are in the next cycle's tolerance zone, so consider it completed
                // and do a combined wait with it
                auto next_cycle = plan_sleep(next_cycle_start);
                return {time_to_next_cycle + std::get<0>(next_cycle), std::get<1>(next_cycle)};
            }
            else
            {
                // we are well before the next cycle start
                return {time_to_next_cycle, true};
            }
        }
        else
        {
            auto time_to_on_time = now.time_to(OnTime);
            if (time_to_on_time < MaxDrift)
            {
                // we are before OnTime, but in its tolerance zone;
                // do a combined wait with the first on-time period
                auto next_cycle = plan_sleep(OnTime);
                return {time_to_on_time + std::get<0>(next_cycle), std::get<1>(next_cycle)};
            }
            else
            {
                // we are well before the OnTime
                return {time_to_on_time, false};
            }
        }
    }
};
