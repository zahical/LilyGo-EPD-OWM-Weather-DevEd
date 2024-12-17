/*
 * Copyright (c) David Bird 2021. All rights to this software are reserved.
 * Copyright (c) 2024 zahical. All rights reserved.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/**
 * @file Interface to the UI drawing code.
 * All methods are also safe to call in host build.
 */

#pragma once
#include <Arduino.h> // for String

/// Inits the display hardware and allocates the frame buffer. (Safe to call in host build, as well)
bool InitGraphics();

/// Get the raw framebuffer. (only exposed for host build needs)
uint8_t *Framebuffer();

/// Draws the weather UI. The data globals in the `shared` ns must be populated before calling this.
void DisplayWeather();

/// Draws the error UI.
void DisplayError(String const& message);