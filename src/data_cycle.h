/*
 * Copyright (c) David Bird 2021. All rights to this software are reserved.
 * Copyright (c) 2024 zahical. All rights reserved.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/**
 * @file Main interface to the OpenWeatherMap API.
 */
#pragma once
#include "common.h"

/**
 * Calls the weather provider API and populates the variables in the `shared` namespace.
 */
OpResult<void> do_data_cycle();