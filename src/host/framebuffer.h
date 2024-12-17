/*
 * Copyright (c) 2024 zahical. All rights reserved.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/**
 * @file Frame buffer helpers.
 */

/// Fills the framebuffer with 1-pix vertical stripes.
void zebra();

/// Fills the framebuffer with a checkers pattern.
void checkers();

/// Saves the framebuffer to a grayscale PNG file called output.png in the current folder.
void save_fb();