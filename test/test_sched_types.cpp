/*
 * Copyright (c) 2024 zahical. All rights reserved.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/**
 * @file Unit tests for DailyTime.
 */

#include "../src/schedule.h"
#include "unity.h"
#include <chrono>

void setUp(void)
{
    // unity
}

void tearDown(void)
{
    // unity
}

// ----------
using namespace std::chrono_literals;
using csec = std::chrono::seconds;

void test_daily_time_ctor1()
{
    DailyTime dt{0};
    TEST_ASSERT(dt.sec_since_day_start() == 0s);
    TEST_ASSERT(dt.sec_till_day_end() == 24h);
    TEST_ASSERT_EQUAL(0, dt.hours());
    TEST_ASSERT_EQUAL(0, dt.minutes());
    TEST_ASSERT_EQUAL(0, dt.seconds());
}

void test_daily_time_ctor2()
{
    DailyTime dt{1, 2, 3};
    TEST_ASSERT(dt.sec_since_day_start() == 1h + 2min + 3s);
    TEST_ASSERT(dt.sec_till_day_end() == 24h - (1h + 2min + 3s));
    TEST_ASSERT_EQUAL(1, dt.hours());
    TEST_ASSERT_EQUAL(2, dt.minutes());
    TEST_ASSERT_EQUAL(3, dt.seconds());
}

void test_daily_time_ctor3()
{
    // wrap around
    DailyTime dt{24h + 1s};
    TEST_ASSERT(dt.sec_since_day_start() == 1s);
    TEST_ASSERT(dt.sec_till_day_end() == 24h - (1s));
    TEST_ASSERT_EQUAL(0, dt.hours());
    TEST_ASSERT_EQUAL(0, dt.minutes());
    TEST_ASSERT_EQUAL(1, dt.seconds());
}

void test_daily_time_is_between_norm()
{
    DailyTime start{10h + 20min + 30s};
    DailyTime end{11h + 10min + 0s};

    DailyTime dt{11h + 5min + 30s};

    TEST_ASSERT(dt.is_between(start, end));
    TEST_ASSERT(start.is_between(start, end));
    TEST_ASSERT(!end.is_between(start, end));
}

void test_daily_time_is_between_rev()
{
    DailyTime start{23h + 20min + 30s};
    DailyTime end{11h + 10min + 0s};

    DailyTime dt{5h + 5min + 35s};

    TEST_ASSERT(dt.is_between(start, end));
    TEST_ASSERT(start.is_between(start, end));
    TEST_ASSERT(!end.is_between(start, end));
}

void test_daily_time_is_between_all_same()
{
    DailyTime dt{11h + 5min + 30s};

    TEST_ASSERT(dt.is_between(dt, dt));
}

void test_daily_time_plus()
{
    DailyTime dt{23h + 30min + 50s};
    TEST_ASSERT(dt + 10s == DailyTime{23h + 31min + 0s});
    TEST_ASSERT(dt + 1h == DailyTime{0h + 30min + 50s}); // wrap around
}

void test_daily_time_to()
{
    TEST_ASSERT(DailyTime{23h + 31min + 0s}.time_to(DailyTime{23h + 31min + 0s}) == 0s);
    TEST_ASSERT(DailyTime{23h + 31min + 0s}.time_to(DailyTime{23h + 32min + 10s}) == 1min + 10s);
    TEST_ASSERT(DailyTime{23h + 31min + 0s}.time_to(DailyTime{1h + 32min + 10s}) == 2h + 1min + 10s);
    TEST_ASSERT(DailyTime{23h + 31min + 0s}.time_to(DailyTime{23h + 30min + 59s}) == 24h - 1s);
}

void test_daily_time_cmp()
{
    TEST_ASSERT(DailyTime{23h + 31min + 0s} == DailyTime{23h + 31min + 0s});
    TEST_ASSERT(!(DailyTime{23h + 31min + 0s} == DailyTime{23h + 31min + 1s}));

    TEST_ASSERT(DailyTime{23h + 31min + 0s} < DailyTime{23h + 31min + 1s});
    TEST_ASSERT(!(DailyTime{23h + 31min + 0s} < DailyTime{23h + 31min + 0s}));
    TEST_ASSERT(!(DailyTime{23h + 31min + 1s} < DailyTime{23h + 31min + 0s}));

    TEST_ASSERT(DailyTime{23h + 31min + 0s} <= DailyTime{23h + 31min + 1s});
    TEST_ASSERT(DailyTime{23h + 31min + 0s} <= DailyTime{23h + 31min + 0s});
    TEST_ASSERT(!(DailyTime{23h + 31min + 1s} <= DailyTime{23h + 31min + 0s}));

    TEST_ASSERT(DailyTime{23h + 31min + 1s} >= DailyTime{23h + 31min + 0s});
    TEST_ASSERT(DailyTime{23h + 31min + 0s} >= DailyTime{23h + 31min + 0s});
    TEST_ASSERT(!(DailyTime{23h + 31min + 0s} >= DailyTime{23h + 31min + 1s}));
}

void test_daily_time_multiple()
{
    TEST_ASSERT(DailyTime{10h + 0min + 0s}.time_to_next_multiple_of(DailyTime{10h + 0min + 0s}, 10s) == 0s);
    TEST_ASSERT(DailyTime{10h + 0min + 0s}.time_to_next_multiple_of(DailyTime{9h + 0min + 0s}, 1h) == 0s);

    TEST_ASSERT(DailyTime{10h + 0min + 0s}.time_to_next_multiple_of(DailyTime{9h + 30min + 0s}, 1h) == 30min);
    TEST_ASSERT(DailyTime{10h + 0min + 0s}.time_to_next_multiple_of(DailyTime{10h + 30min + 0s}, 1h) == 30min);

    // wrapping
    TEST_ASSERT(DailyTime{0h + 10min + 0s}.time_to_next_multiple_of(DailyTime{23h + 30min + 0s}, 1h) == 20min);
    TEST_ASSERT(DailyTime{23h + 40min + 0s}.time_to_next_multiple_of(DailyTime{23h + 30min + 0s}, 1h) == 50min);
}

// -------------

void test_scheduler_combine_with_offtime_sched()
{
    auto [sleep, active] = Scheduler{DailyTime{7, 30}, DailyTime{23, 0}, 60min, 5min}
        .plan_sleep(DailyTime{22, 26});

    TEST_ASSERT(sleep == 30min + 4min);
    TEST_ASSERT(active == true);
}

void test_scheduler_no_combine_with_offtime_sched()
{
    auto [sleep, active] = Scheduler{DailyTime{7, 30}, DailyTime{23, 0}, 60min, 1min}
        .plan_sleep(DailyTime{22, 26});

    TEST_ASSERT(sleep == 4min);
    TEST_ASSERT(active == true);
}

void test_scheduler_combine_with_first_on()
{
    auto [sleep, active] = Scheduler{DailyTime{7, 30}, DailyTime{23, 0}, 60min, 5min}
        .plan_sleep(DailyTime{7, 26});

    TEST_ASSERT(sleep == 4min + 60min);
    TEST_ASSERT(active == true);
}

void test_scheduler_off_already_in_off_zone()
{
    auto [sleep, active] = Scheduler{DailyTime{7, 30}, DailyTime{7, 31}, 60min, 5min}
        .plan_sleep(DailyTime{7, 26});

    TEST_ASSERT(sleep == 4min + 24h);
    TEST_ASSERT(active == false);
}

void test_scheduler_regular_on_time()
{
    auto [sleep, active] = Scheduler{DailyTime{7, 30}, DailyTime{23, 0}, 60min, 5min}
        .plan_sleep(DailyTime{8, 0});

    TEST_ASSERT(sleep == 30min);
    TEST_ASSERT(active == true);
}

void test_scheduler_regular_on_time_exact_multiple()
{
    auto [sleep, active] = Scheduler{DailyTime{7, 30}, DailyTime{23, 0}, 60min, 5min}
        .plan_sleep(DailyTime{8, 30});

    TEST_ASSERT(sleep == 1h);
    TEST_ASSERT(active == true);
}

void test_scheduler_always_on()
{
    auto [sleep, active] = Scheduler{DailyTime{7, 30}, DailyTime{7, 30}, 60min, 5min}
        .plan_sleep(DailyTime{7, 29});

    TEST_ASSERT(sleep == 1h + 1min);
    TEST_ASSERT(active == true);
}

// ----------

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_daily_time_ctor1);
    RUN_TEST(test_daily_time_ctor2);
    RUN_TEST(test_daily_time_ctor3);
    RUN_TEST(test_daily_time_is_between_norm);
    RUN_TEST(test_daily_time_is_between_rev);
    RUN_TEST(test_daily_time_is_between_all_same);
    RUN_TEST(test_daily_time_plus);
    RUN_TEST(test_daily_time_to);
    RUN_TEST(test_daily_time_cmp);
    RUN_TEST(test_daily_time_multiple);

    RUN_TEST(test_scheduler_combine_with_offtime_sched);
    RUN_TEST(test_scheduler_no_combine_with_offtime_sched);
    RUN_TEST(test_scheduler_combine_with_first_on);
    RUN_TEST(test_scheduler_off_already_in_off_zone);
    RUN_TEST(test_scheduler_regular_on_time);
    RUN_TEST(test_scheduler_regular_on_time_exact_multiple);
    RUN_TEST(test_scheduler_always_on);

    UNITY_END();
}