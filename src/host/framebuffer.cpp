/*
 * Copyright (c) 2024 zahical. All rights reserved.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/**
 * @file Frame buffer helpers using the rpng library.
 */

#include "framebuffer.h"
#include "display.h"

#include "libs/rpng/rpng.h"
#include <epd_driver.h>

#include <cstdint>
#include <cstring>

static_assert(EPD_WIDTH % 2 == 0, "even width screen buffer");
static_assert(EPD_HEIGHT % 2 == 0, "even height screen buffer");

void zebra() { memset(Framebuffer(), 0xF0, EPD_WIDTH * EPD_HEIGHT / 2); }

void checkers()
{
    uint8_t *buf = Framebuffer();

    for (int y = 0; y < EPD_HEIGHT; y += 2)
    {
        memset(buf, 0xF0, EPD_WIDTH / 2);
        buf += EPD_WIDTH / 2;
        memset(buf, 0x0F, EPD_WIDTH / 2);
        buf += EPD_WIDTH / 2;
    }
}

void save_fb()
{
    uint8_t *png_buf = (uint8_t *)calloc(sizeof(uint8_t), EPD_WIDTH * EPD_HEIGHT);
    uint8_t *in      = Framebuffer();
    uint8_t *out     = png_buf;

    for (int i = 0; i < EPD_WIDTH * EPD_HEIGHT / 2; i++)
    {
        uint8_t n_hi = (*in) >> 4;
        uint8_t n_lo = (*in) & 0x0F;

        n_hi |= (n_hi << 4);
        n_lo |= (n_lo << 4);

        // note these two are swapped
        *out++ = n_lo;
        *out++ = n_hi;

        in++;
    }

    int r = rpng_save_image("output.png", reinterpret_cast<char *>(png_buf), EPD_WIDTH, EPD_HEIGHT, 1, 8);
}