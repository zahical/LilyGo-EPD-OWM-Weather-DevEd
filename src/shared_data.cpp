/*
 * Copyright (c) David Bird 2021. All rights to this software are reserved.
 * Copyright (c) 2024 zahical. All rights reserved.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/**
 * @file backing storage for globals shared between OWM API query code, drawing code and main.cpp.
 */

#include "shared_data.h"

namespace shared
{
Forecast_record_type WxConditions;
Forecast_record_type WxForecast[max_readings];

LocData_type LocData;
Air_quality_record WxAirQ;
struct tm CycleStart;
char TZName[] = {0};
std::chrono::seconds time_offset;
bool ActiveHours;
float voltage;
int wifi_signal;
} // namespace shared
