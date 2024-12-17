/*
 * Copyright (c) 2024 zahical. All rights reserved.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/**
 * @file Definition of OWM's Air Quality Index components.
 */
#pragma once
#include <cstdint>

/// Air Quality Index components
enum class AQIndicator : uint8_t
{
    /// Sulphur dioxide
    SO2,
    // Nitrogen dioxide
    NO2,
    /// Coarse particulate matter
    PM10,
    /// Fine particles matter
    PM2_5,
    /// Ozone
    O3,
    /// Carbon monoxide
    CO,
    /// Ammonia (not part of OWM's AQI)
    NH3,
    /// Nitrogen monoxide (not part of OWM's AQI)
    NO,
};