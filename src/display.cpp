/*
 * Copyright (c) David Bird 2021. All rights to this software are reserved.
 * Copyright (c) 2024 zahical. All rights reserved.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/**
 * @file Main weather UI drawing code.
 *
 * The bulk of the content of this file comes from the original OWM_EPD47_epaper_v2.72.ino.
 * The coding style, naming convention, comments and source code formatting were preserved as much as possible except
 * that methods were re-arranged to avoid the need for forward declarations.
 *
 * Also, anonymous namespaces are now used to group methods with related functionality.
 */

// clang-format off
#include <Arduino.h>
#include <epd_driver.h>

#include "app_ver.h"
#include "aqi_metric.h"
#include "display.h"
#include "shared_data.h"
#include "config.h"
#include "common.h"
#include "timings.h"
#include "lang/lang.h"

#include "fonts/opensans5cb_special2.h"
#include "fonts/opensans6b.h"
#include "fonts/opensans8b.h"
#include "fonts/opensans10b.h"
#include "fonts/opensans12b.h"
#include "fonts/opensans18b.h"
#include "fonts/opensans24b.h"

#include "imgs/moon.h"
#include "imgs/sunrise.h"
#include "imgs/sunset.h"
#include "imgs/uvi.h"
#include "imgs/TempHi.h"
#include "imgs/TempLo.h"
#include "imgs/TempFL.h"
#include "imgs/DST.h"
#include "imgs/AirTree.h"
#include "imgs/Gauge0.h"
#include "imgs/Gauge1.h"
#include "imgs/Gauge2.h"
#include "imgs/Gauge3.h"
#include "imgs/Gauge4.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <bitset>
#include <utility>
#include <tuple>

// clang-format on

#define SCREEN_WIDTH  EPD_WIDTH
#define SCREEN_HEIGHT EPD_HEIGHT

#define White     0xFF
#define LightGrey 0xBB
#define Grey      0x88
#define DarkGrey  0x44
#define Black     0x00

constexpr bool LargeIcon = true;
constexpr bool SmallIcon = false;

#define Large 20 // For icon drawing
#define Small 10 // For icon drawing

#define autoscale_on  true
#define autoscale_off false
#define barchart_on   true
#define barchart_off  false

// module globals
namespace
{
GFXfont const *currentFont = nullptr;
uint8_t *framebuffer       = nullptr;
} // namespace

// screen ops
namespace
{
// clang-format on

bool init_epd_alloc_fb()
{
#ifndef HOST_BUILD
    epd_init();
    framebuffer = (uint8_t *)ps_calloc(sizeof(uint8_t), EPD_WIDTH * EPD_HEIGHT / 2);
#else
    framebuffer = (uint8_t *)calloc(sizeof(uint8_t), EPD_WIDTH * EPD_HEIGHT / 2);
#endif

    if (!framebuffer) return false;

    memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);

    return true;
}

void clear_epd_flush_fb_and_power_off(Rect_t const *rect = nullptr)
{
#ifndef HOST_BUILD
    {
        AutoTiming timing{TimeEvent::PowerOnScreen};
        epd_poweron();
    }

    {
        AutoTiming timing{TimeEvent::ClearScreen};
        // Clear the screen
        if (rect == nullptr)
            epd_clear();
        else
            epd_clear_area(*rect);
    }

    {
        AutoTiming timing{TimeEvent::UpdateScreen};
        epd_draw_grayscale_image(epd_full_screen(), framebuffer);
    }

    epd_poweroff_all();
#endif
}

// clang-format on
} // namespace

// drawing primitives
namespace
{
// clang-format on

int text_width(char const *str)
{
    int x = 0, y = 0, x1, y1, w, h;
    get_text_bounds(currentFont, str, &x, &y, &x1, &y1, &w, &h, nullptr);
    return w;
}

// clang-format off
enum alignment {LEFT, RIGHT, CENTER};

int drawString(int x, int y, String const& text, alignment align, uint8_t color = Black) {

    FontProperties props;
    FontProperties *propsPtr;
    if (color != Black)
    {
        props = {.fg_color = color, .bg_color = 0xF, .fallback_glyph = 0, .flags = 0 //(uint32_t)DRAW_BACKGROUND
        };
        propsPtr = &props;
    }
    else
        propsPtr = nullptr;

    int x1, y1; // the bounds of x,y and w and h of the variable 'text' in pixels.
    int w, h;
    int xx = x, yy = y;
    get_text_bounds(currentFont, text.c_str(), &xx, &yy, &x1, &y1, &w, &h, propsPtr);
    if (align == RIGHT) x = x - w;
    if (align == CENTER) x = x - w / 2;
    int cursor_y = y + h;

    write_mode(currentFont, text.c_str(), &x, &cursor_y, framebuffer, (DrawMode_t)0 /*unused*/, propsPtr);

    return w;
}

void drawString_multiline(int x, int y, String const& text) {
  write_string(currentFont, text.c_str(), &x, &y, framebuffer);
}

void fillCircle(int x, int y, int r, uint8_t color) {
  epd_fill_circle(x, y, r, color, framebuffer);
}

void drawFastHLine(int16_t x0, int16_t y0, int length, uint16_t color) {
  epd_draw_hline(x0, y0, length, color, framebuffer);
}

void drawFastVLine(int16_t x0, int16_t y0, int length, uint16_t color) {
  epd_draw_vline(x0, y0, length, color, framebuffer);
}

void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
  epd_write_line(x0, y0, x1, y1, color, framebuffer);
}

void drawCircle(int x0, int y0, int r, uint8_t color) {
  epd_draw_circle(x0, y0, r, color, framebuffer);
}

void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  epd_draw_rect(x, y, w, h, color, framebuffer);
}

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  epd_fill_rect(x, y, w, h, color, framebuffer);
}

void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                  int16_t x2, int16_t y2, uint16_t color) {
  epd_fill_triangle(x0, y0, x1, y1, x2, y2, color, framebuffer);
}

void drawPixel(int x, int y, uint8_t color) {
  epd_draw_pixel(x, y, color, framebuffer);
}

void drawGrayscaleImage(Rect_t const& area, uint8_t const * data) {
  // epd_draw_grayscale_image(area, (uint8_t *) data);

  // unlike the original code, this method does not flush the image the screen,
  // but only copies it to the framebuffer
  epd_copy_to_framebuffer(area, (uint8_t *)data, framebuffer);
}

void setFont(GFXfont const & font) {
  currentFont = &font;
}

// clang-format on

void drawDottedLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint8_t color, std::bitset<8> pattern)
{
    int32_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep)
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int32_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int32_t err   = dx / 2;
    int32_t ystep = y0 < y1 ? 1 : -1;

    int p_idx = 0;

    for (; x0 <= x1; x0++)
    {
        if (pattern[p_idx])
        {
            if (steep)
                drawPixel(y0, x0, color);
            else
                drawPixel(x0, y0, color);
        }
        p_idx++;
        if (p_idx == pattern.size()) p_idx = 0;

        err -= dy;
        if (err < 0)
        {
            y0 += ystep;
            err += dx;
        }
    }
}

/**
 * @brief Executes the provided `FCall` callback for all pixels in the rectangular area defined by rect
 * passing it the current pixel color and then replacing the pixel color with the value returned by the callback.
 *
 * @tparam FCall a callable with `uint8_t(uint8_t)` sig.
 * @param rect framebuffer rect to modify
 * @param mod the callback; color is passed and returned as 8-bit
 */
template <typename FCall>
void modifyRect(Rect_t const rect, FCall&& mod)
{
    for (int32_t hline = 0; hline < rect.height; hline++)
    {
        auto y = rect.y + hline;

        uint8_t *pix_ptr = &framebuffer[y * SCREEN_WIDTH / 2 + rect.x / 2];
        bool even_start  = (rect.x % 2) == 0;
        bool even_width  = (rect.width % 2) == 0;
        bool even_end    = even_start == even_width;

        int width = rect.width;

        if (!even_start)
        {
            uint8_t pix = *pix_ptr;

            uint8_t c_half = pix & 0xF0;
            c_half |= c_half >> 4;

            uint8_t new_val = mod(c_half);

            pix &= 0x0F;
            pix |= new_val & 0xF0;
            *pix_ptr = pix;

            pix_ptr++;
            width--;
        }

        for (int i = width / 2; i > 0; i--)
        {

            uint8_t pix = *pix_ptr;

            uint8_t l_half = pix & 0x0F;
            l_half |= l_half << 4;

            uint8_t u_half = pix & 0xF0;
            u_half |= u_half >> 4;

            l_half = mod(l_half);
            u_half = mod(u_half);

            pix      = (u_half & 0xF0) | (l_half >> 4);
            *pix_ptr = pix;
            pix_ptr++;
        }

        if (!even_end)
        {
            uint8_t pix = *pix_ptr;

            uint8_t c_half = pix & 0x0F;
            c_half |= c_half << 4;

            uint8_t new_val = mod(c_half);

            pix &= 0xF0;
            pix |= new_val >> 4;
            *pix_ptr = pix;
        }
    }
}

void invertRect(Rect_t const& rect)
{
    modifyRect(rect, [](uint8_t c) { return 255 - c; });
}

/// changes the color of the specified HLine (x1,y)-(x2,y) by adding (with min/max clipping) the signed value
/// specified in `change` to the current pixel color
void sumColorHLine(uint16_t x1, uint16_t x2, uint16_t y, int16_t change)
{
    Rect_t r{x1, y, x2 - x1, 1};

    modifyRect(r, [change](uint8_t c) {
        int16_t c_new = c + change;
        if (c_new > 255)
            c_new = 255;
        else if (c_new < 0)
            c_new = 0;

        return c_new;
    });
}
} // namespace

// helpers
namespace
{
// clang-format off

String ConvertUnixTime(int unix_time) {
  // Returns either '21:12  ' or ' 09:12pm' depending on Units mode
  time_t tm = unix_time;
  tm += shared::time_offset.count();
  struct tm *now_tm = localtime(&tm);
  char output[40];
  if (cfg::UseMetricUnits) {
    strftime(output, sizeof(output), "%H:%M %d/%m/%y", now_tm);
  }
  else {
    strftime(output, sizeof(output), "%I:%M%P %m/%d/%y", now_tm);
  }
  return output;
}

String TitleCase(String text) {
  if (text.length() > 0) {
    String temp_text = text.substring(0, 1);
    temp_text.toUpperCase();
    return temp_text + text.substring(1); // Title-case the string
  }
  else return text;
}

// clang-format on

template <typename T>
constexpr T max_of(T a, T b, T c)
{
    return a > b ? (a > c ? a : c) : (b > c ? b : c);
}

std::tuple<int, int> circle_pos(int x, int y, float angle_deg, float radius)
{
    return {round(x + cos((angle_deg)*PI / 180) * radius), round(y + sin((angle_deg)*PI / 180) * radius)};
}

std::tuple<float, float> circle_unit_pos(float angle_deg)
{
    return {cos((angle_deg)*PI / 180), sin((angle_deg)*PI / 180)};
}

// clang-format on
} // namespace

// Icon Primitives
namespace
{
// clang-format off

void addmoon(int x, int y, bool IconSize) {
  int xOffset = 65;
  int yOffset = 12;
  if (IconSize == LargeIcon) {
    xOffset = 130;
    yOffset = -10;
  }
  fillCircle(x - 28 + xOffset, y - 37 + yOffset, uint16_t(Small * 1.0), Black);
  fillCircle(x - 16 + xOffset, y - 37 + yOffset, uint16_t(Small * 1.6), White);
}

// Symbols are drawn on a relative 10x10grid and 1 scale unit = 1 drawing unit
void addcloud(int x, int y, int scale, int linesize) {
  fillCircle(x - scale * 3, y, scale, Black);                                                              // Left most circle
  fillCircle(x + scale * 3, y, scale, Black);                                                              // Right most circle
  fillCircle(x - scale, y - scale, scale * 1.4, Black);                                                    // left middle upper circle
  fillCircle(x + scale * 1.5, y - scale * 1.3, scale * 1.75, Black);                                       // Right middle upper circle
  fillRect(x - scale * 3 - 1, y - scale, scale * 6, scale * 2 + 1, Black);                                 // Upper and lower lines
  fillCircle(x - scale * 3, y, scale - linesize, White);                                                   // Clear left most circle
  fillCircle(x + scale * 3, y, scale - linesize, White);                                                   // Clear right most circle
  fillCircle(x - scale, y - scale, scale * 1.4 - linesize, White);                                         // left middle upper circle
  fillCircle(x + scale * 1.5, y - scale * 1.3, scale * 1.75 - linesize, White);                            // Right middle upper circle
  fillRect(x - scale * 3 + 2, y - scale + linesize - 1, scale * 5.9, scale * 2 - linesize * 2 + 2, White); // Upper and lower lines
}

void addrain(int x, int y, int scale, bool IconSize) {
  if (IconSize == SmallIcon) {
    setFont(OpenSans8B);
    drawString(x - 25, y + 12, "///////", LEFT);
  }
  else
  {
    setFont(OpenSans18B);
    drawString(x - 60, y + 25, "///////", LEFT);
  }
}

void addsnow(int x, int y, int scale, bool IconSize) {
  if (IconSize == SmallIcon) {
    setFont(OpenSans8B);
    drawString(x - 25, y + 15, "* * * *", LEFT);
  }
  else
  {
    setFont(OpenSans18B);
    drawString(x - 60, y + 30, "* * * *", LEFT);
  }
}

void addtstorm(int x, int y, int scale) {
  y = y + scale / 2;
  for (int i = 1; i < 5; i++) {
    drawLine(x - scale * 4 + scale * i * 1.5 + 0, y + scale * 1.5, x - scale * 3.5 + scale * i * 1.5 + 0, y + scale, Black);
    drawLine(x - scale * 4 + scale * i * 1.5 + 1, y + scale * 1.5, x - scale * 3.5 + scale * i * 1.5 + 1, y + scale, Black);
    drawLine(x - scale * 4 + scale * i * 1.5 + 2, y + scale * 1.5, x - scale * 3.5 + scale * i * 1.5 + 2, y + scale, Black);
    drawLine(x - scale * 4 + scale * i * 1.5, y + scale * 1.5 + 0, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5 + 0, Black);
    drawLine(x - scale * 4 + scale * i * 1.5, y + scale * 1.5 + 1, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5 + 1, Black);
    drawLine(x - scale * 4 + scale * i * 1.5, y + scale * 1.5 + 2, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5 + 2, Black);
    drawLine(x - scale * 3.5 + scale * i * 1.4 + 0, y + scale * 2.5, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5, Black);
    drawLine(x - scale * 3.5 + scale * i * 1.4 + 1, y + scale * 2.5, x - scale * 3 + scale * i * 1.5 + 1, y + scale * 1.5, Black);
    drawLine(x - scale * 3.5 + scale * i * 1.4 + 2, y + scale * 2.5, x - scale * 3 + scale * i * 1.5 + 2, y + scale * 1.5, Black);
  }
}

void DrawAngledLine(int x, int y, int x1, int y1, int size, int color) {
  int dx = (size / 2.0) * (x - x1) / sqrt(sq(x - x1) + sq(y - y1));
  int dy = (size / 2.0) * (y - y1) / sqrt(sq(x - x1) + sq(y - y1));
  fillTriangle(x + dx, y - dy, x - dx,  y + dy,  x1 + dx, y1 - dy, color);
  fillTriangle(x - dx, y + dy, x1 - dx, y1 + dy, x1 + dx, y1 - dy, color);
}

void addsun(int x, int y, int scale, bool IconSize) {
  int linesize = 5;
  fillRect(x - scale * 2, y, scale * 4, linesize, Black);
  fillRect(x, y - scale * 2, linesize, scale * 4, Black);
  DrawAngledLine(x + scale * 1.4, y + scale * 1.4, (x - scale * 1.4), (y - scale * 1.4), linesize * 1.5, Black); // Actually sqrt(2) but 1.4 is good enough
  DrawAngledLine(x - scale * 1.4, y + scale * 1.4, (x + scale * 1.4), (y - scale * 1.4), linesize * 1.5, Black);
  fillCircle(x, y, scale * 1.3, White);
  fillCircle(x, y, scale, Black);
  fillCircle(x, y, scale - linesize, White);
}

void addfog(int x, int y, int scale, int linesize, bool IconSize) {
  if (IconSize == SmallIcon) linesize = 3;
  for (int i = 0; i < 6; i++) {
    fillRect(x - scale * 3, y + scale * 1.5, scale * 6, linesize, Black);
    fillRect(x - scale * 3, y + scale * 2.0, scale * 6, linesize, Black);
    fillRect(x - scale * 3, y + scale * 2.5, scale * 6, linesize, Black);
  }
}

// clang-format on
} // namespace

// Graphs Section
namespace
{
// clang-format on
float pressure_readings[max_readings]    = {0};
float temperature_readings[max_readings] = {0};
float humidity_readings[max_readings]    = {0};
float rain_readings[max_readings]        = {0};
float snow_readings[max_readings]        = {0};

// clang-format off

/* (C) D L BIRD
    This function will draw a graph on a ePaper/TFT/LCD display using data from an array containing data to be graphed.
    The variable 'max_readings' determines the maximum number of data elements for each array. Call it with the following parametric data:
    x_pos-the x axis top-left position of the graph
    y_pos-the y-axis top-left position of the graph, e.g. 100, 200 would draw the graph 100 pixels along and 200 pixels down from the top-left of the screen
    width-the width of the graph in pixels
    height-height of the graph in pixels
    Y1_Max-sets the scale of plotted data, for example 5000 would scale all data to a Y-axis of 5000 maximum
    data_array1 is parsed by value, externally they can be called anything else, e.g. within the routine it is called data_array1, but externally could be temperature_readings
    auto_scale-a logical value (TRUE or FALSE) that switches the Y-axis autoscale On or Off
    barchart_on-a logical value (TRUE or FALSE) that switches the drawing mode between barhcart and line graph
    barchart_colour-a sets the title and graph plotting colour
    If called with Y!_Max value of 500 and the data never goes above 500, then autoscale will retain a 0-500 Y scale, if on, the scale increases/decreases to match the data.
    auto_scale_margin, e.g. if set to 1000 then autoscale increments the scale by 1000 steps.
*/
void DrawGraph(int x_pos, int y_pos, int gwidth, int gheight, float Y1Min, float Y1Max, String title, float DataArray[], int readings, boolean auto_scale, boolean barchart_mode) {
#define auto_scale_margin 0 // Sets the autoscale increment, so axis steps up fter a change of e.g. 3
#define y_minor_axis 5      // 5 y-axis division markers
  setFont(OpenSans10B);
  int maxYscale = -10000;
  int minYscale =  10000;
  int last_x, last_y;
  float x2, y2;
  if (auto_scale == true) {
    for (int i = 1; i < readings; i++ ) {
      if (DataArray[i] >= maxYscale) maxYscale = DataArray[i];
      if (DataArray[i] <= minYscale) minYscale = DataArray[i];
    }
    maxYscale = round(maxYscale + auto_scale_margin); // Auto scale the graph and round to the nearest value defined, default was Y1Max
    Y1Max = round(maxYscale + 0.5);
    if (minYscale != 0) minYscale = round(minYscale - auto_scale_margin); // Auto scale the graph and round to the nearest value defined, default was Y1Min
    Y1Min = round(minYscale);
  }
  // Draw the graph
  last_x = x_pos + 1;
  last_y = y_pos + (Y1Max - constrain(DataArray[1], Y1Min, Y1Max)) / (Y1Max - Y1Min) * gheight;
  drawRect(x_pos, y_pos, gwidth + 3, gheight + 2, Grey);
  drawString(x_pos - 20 + gwidth / 2, y_pos - 28, title, CENTER);
  for (int gx = 0; gx < readings; gx++) {
    x2 = x_pos + gx * gwidth / (readings - 1.0) - 1 ; // max_readings is the global variable that sets the maximum data that can be plotted
    y2 = y_pos + (Y1Max - constrain(DataArray[gx], Y1Min, Y1Max)) / (Y1Max - Y1Min) * gheight + 1;
    if (barchart_mode) {
      fillRect(last_x + 2, y2, (gwidth / readings) - 1, y_pos + gheight - y2 + 2, Black);
    } else {
      drawLine(last_x, last_y - 1, x2, y2 - 1, Black); // Two lines for hi-res display
      drawLine(last_x, last_y, x2, y2, Black);
    }
    last_x = x2;
    last_y = y2;
  }
  //Draw the Y-axis scale
#define number_of_dashes 20
  for (int spacing = 0; spacing <= y_minor_axis; spacing++) {
    for (int j = 0; j < number_of_dashes; j++) { // Draw dashed graph grid lines
      if (spacing < y_minor_axis) drawFastHLine((x_pos + 3 + j * gwidth / number_of_dashes), y_pos + (gheight * spacing / y_minor_axis), gwidth / (2 * number_of_dashes), Grey);
    }
    if ((Y1Max - (float)(Y1Max - Y1Min) / y_minor_axis * spacing) < 5 || title == TXT_PRESSURE_IN) {
      drawString(x_pos - 10, y_pos + gheight * spacing / y_minor_axis - 5, String((Y1Max - (float)(Y1Max - Y1Min) / y_minor_axis * spacing + 0.01), 1), RIGHT);
    }
    else
    {
      if (Y1Min < 1 && Y1Max < 10) {
        drawString(x_pos - 3, y_pos + gheight * spacing / y_minor_axis - 5, String((Y1Max - (float)(Y1Max - Y1Min) / y_minor_axis * spacing + 0.01), 1), RIGHT);
      }
      else {
        drawString(x_pos - 7, y_pos + gheight * spacing / y_minor_axis - 5, String((Y1Max - (float)(Y1Max - Y1Min) / y_minor_axis * spacing + 0.01), 0), RIGHT);
      }
    }
  }
  for (int i = 0; i < 3; i++) {
    drawString(20 + x_pos + gwidth / 3 * i, y_pos + gheight + 10, String(i) + "d", LEFT);
    if (i < 2) drawFastVLine(x_pos + gwidth / 3 * i + gwidth / 3, y_pos, gheight, LightGrey);
  }
}

float SumOfPrecip(float DataArray[], int readings) {
  float sum = 0;
  for (int i = 0; i <= readings; i++) sum += DataArray[i];
  return sum;
}

void DisplayGraphSection(int x, int y) {
  int r = 0;

  do { // Pre-load temporary arrays with with data - because C parses by reference and remember that[1] has already been converted to I units
    auto const& wx_obj = shared::WxForecast[r];

    if (!cfg::UseMetricUnits) pressure_readings[r] = wx_obj.Pressure * 0.02953;   else pressure_readings[r] = wx_obj.Pressure;
    if (!cfg::UseMetricUnits) rain_readings[r]     = wx_obj.Rainfall * 0.0393701; else rain_readings[r]     = wx_obj.Rainfall;
    if (!cfg::UseMetricUnits) snow_readings[r]     = wx_obj.Snowfall * 0.0393701; else snow_readings[r]     = wx_obj.Snowfall;
    temperature_readings[r]                = wx_obj.Temperature;
    humidity_readings[r]                   = wx_obj.Humidity;
    r++;
  } while (r < max_readings);
  int gwidth = 175, gheight = 100;
  int gx = (SCREEN_WIDTH - gwidth * 4) / 5 + 8;
  int gy = (SCREEN_HEIGHT - gheight - 30);
  int gap = gwidth + gx;
  // (x,y,width,height,MinValue, MaxValue, Title, Data Array, AutoScale, ChartMode)
  DrawGraph(gx + 0 * gap, gy, gwidth, gheight, 900, 1050, cfg::UseMetricUnits ? TXT_PRESSURE_HPA : TXT_PRESSURE_IN, pressure_readings, max_readings, autoscale_on, barchart_off);
  DrawGraph(gx + 1 * gap, gy, gwidth, gheight, 10, 30,    cfg::UseMetricUnits ? TXT_TEMPERATURE_C : TXT_TEMPERATURE_F, temperature_readings, max_readings, autoscale_on, barchart_off);
  DrawGraph(gx + 2 * gap, gy, gwidth, gheight, 0, 100,   TXT_HUMIDITY_PERCENT, humidity_readings, max_readings, autoscale_off, barchart_off);
  if (SumOfPrecip(rain_readings, max_readings) >= SumOfPrecip(snow_readings, max_readings))
    DrawGraph(gx + 3 * gap + 5, gy, gwidth, gheight, 0, 30, cfg::UseMetricUnits ? TXT_RAINFALL_MM : TXT_RAINFALL_IN, rain_readings, max_readings, autoscale_on, barchart_on);
  else
    DrawGraph(gx + 3 * gap + 5, gy, gwidth, gheight, 0, 30, cfg::UseMetricUnits ? TXT_SNOWFALL_MM : TXT_SNOWFALL_IN, snow_readings, max_readings, autoscale_on, barchart_on);
}

// clang-format on
} // namespace

// Conditions Section (uses Icon Primitives)
namespace
{
// clang-format off

void ClearSky(int x, int y, bool IconSize) {
  int scale = Small;
  if (IconSize == LargeIcon) scale = Large;
  y += (IconSize ? 0 : 10);
  addsun(x, y, scale * (IconSize ? 1.7 : 1.2), IconSize);
}


void FewClouds(int x, int y, bool IconSize) {
  int scale = Small, linesize = 5;
  y += 15;
  if (IconSize == LargeIcon) scale = Large;
  addcloud(x + (IconSize ? 10 : 0), y, scale * (IconSize ? 0.9 : 0.8), linesize);
  addsun((x + (IconSize ? 10 : 0)) - scale * 1.8, y - scale * 1.6, scale, IconSize);
}

void ScatteredClouds(int x, int y, bool IconSize) {
  int scale = Small, linesize = 5;
  y += 15;
  if (IconSize == LargeIcon) scale = Large;
  addcloud(x - (IconSize ? 35 : 0), y * (IconSize ? 0.75 : 0.93), scale / 2, linesize); // Cloud top left
  addcloud(x, y, scale * 0.9, linesize);                                                // Main cloud
}


void BrokenClouds(int x, int y, bool IconSize) {
  int scale = Small, linesize = 5;
  y += 15;
  if (IconSize == LargeIcon) scale = Large;
  addsun(x - scale * 1.8, y - scale * 1.8, scale, IconSize);
  addcloud(x, y, scale * (IconSize ? 1 : 0.75), linesize);
}


void ChanceRain(int x, int y, bool IconSize) {
  int scale = Small, linesize = 5;
  if (IconSize == LargeIcon) scale = Large;
  y += 15;
  addsun(x - scale * 1.8, y - scale * 1.8, scale, IconSize);
  addcloud(x, y, scale * (IconSize ? 1 : 0.65), linesize);
  addrain(x, y, scale, IconSize);
}

void Rain(int x, int y, bool IconSize) {
  int scale = Small, linesize = 5;
  y += 15;
  if (IconSize == LargeIcon) scale = Large;
  addcloud(x, y, scale * (IconSize ? 1 : 0.75), linesize);
  addrain(x, y, scale, IconSize);
}

void Thunderstorms(int x, int y, bool IconSize) {
  int scale = Small, linesize = 5;
  if (IconSize == LargeIcon) scale = Large;
  y += 5;
  addcloud(x, y, scale * (IconSize ? 1 : 0.75), linesize);
  addtstorm(x, y, scale);
}

void Snow(int x, int y, bool IconSize) {
  int scale = Small, linesize = 5;
  if (IconSize == LargeIcon) scale = Large;
  addcloud(x, y, scale * (IconSize ? 1 : 0.75), linesize);
  addsnow(x, y, scale, IconSize);
}

void Mist(int x, int y, bool IconSize) {
  int scale = Small, linesize = 5;
  if (IconSize == LargeIcon) scale = Large;
  addsun(x, y, scale * (IconSize ? 1 : 0.75), linesize);
  addfog(x, y, scale, linesize, IconSize);
}

void Nodata(int x, int y, bool IconSize) {
  if (IconSize == LargeIcon) setFont(OpenSans24B); else setFont(OpenSans12B);
  drawString(x - 3, y - 10, "?", CENTER);
}

void DisplayConditionsSection(int x, int y, String IconName, bool IconSize) {
  //Serial.println("Icon name: " + IconName);
  if      (IconName.endsWith("n"))                 addmoon(x, y, IconSize);
  if      (IconName == "01d" || IconName == "01n") ClearSky(x, y, IconSize);
  else if (IconName == "02d" || IconName == "02n") FewClouds(x, y, IconSize);
  else if (IconName == "03d" || IconName == "03n") ScatteredClouds(x, y, IconSize);
  else if (IconName == "04d" || IconName == "04n") BrokenClouds(x, y, IconSize);
  else if (IconName == "09d" || IconName == "09n") ChanceRain(x, y, IconSize);
  else if (IconName == "10d" || IconName == "10n") Rain(x, y, IconSize);
  else if (IconName == "11d" || IconName == "11n") Thunderstorms(x, y, IconSize);
  else if (IconName == "13d" || IconName == "13n") Snow(x, y, IconSize);
  else if (IconName == "50d" || IconName == "50n") Mist(x, y, IconSize);
  else                                             Nodata(x, y, IconSize);
}

// clang-format on

} // namespace

// Forecast Section (uses Condition Section)
namespace
{
// clang-format off
void DisplayForecastWeather(int x, int y, int index, int fwidth) {
  auto const& wx_obj = shared::WxForecast[index];
  x = x + fwidth * index;
  DisplayConditionsSection(x + fwidth / 2 - 5, y + 85, wx_obj.Icon, SmallIcon);
  setFont(OpenSans10B);
  drawString(x + fwidth / 2, y + 30, String(ConvertUnixTime(wx_obj.Dt).substring(0, 5)), CENTER);
  drawString(x + fwidth / 2, y + 130, String(wx_obj.High, 0) + "°/" + String(wx_obj.Low, 0) + "°", CENTER);
}

void DisplayForecastSection(int x, int y) {
  int f = 0;
  do {
    DisplayForecastWeather(x, y, f, 82); // x,y cordinates, forecatsr number, spacing width
    f++;
  } while (f < 8);
}

// clang-format on
} // namespace

// Main Weather Icon and Text (uses Condition Section)
namespace
{
// clang-format off
void DisplayMainWeatherIcon(int x, int y) {
  DisplayConditionsSection(x, y, shared::WxConditions.Icon, LargeIcon);
}

// clang-format on

/**
 * @brief Find a line break position in a string (drawn using the current font)
 * so that the second part of the string is no wider than `max_width`.
 * If there's just a single split possible, but it's wider than `max_width`, it's still returned.
 *
 * @param str a null-terminated string
 * @param max_width requested width of the second part of the splitted string
 * @return index of a whitespace char on which to split the string into two lines; -1 if no split is possible
 */
int get_split_pos_2_lines(char const *str, int max_width)
{
    size_t len = strlen(str);

    if (len == 0) return -1;

    int full_width = text_width(str);

    if (full_width <= max_width) return -1;

    size_t i_r         = 0;
    char const *c      = str + len - 1;
    int possible_split = -1;

    while (i_r < len)
    {
        if (*c == ' ')
        {
            int width_to_end = text_width(c + 1);
            if (width_to_end <= max_width || possible_split == -1) possible_split = len - 1 - i_r;

            if (width_to_end >= max_width) break;
        }
        i_r++;
        c--;
    }

    if (possible_split == 0 || possible_split == len - 1) possible_split = -1;

    return possible_split;
}

bool DisplayWeatherText(int x, int y)
{
    setFont(OpenSans10B); // 12

    constexpr auto max_weather_width  = 315;
    constexpr auto line_height_adjust = 10;
    constexpr auto icon_center_line_x = 838;

    constexpr auto t_color = DarkGrey;

    // uncomment to draw the bounding rect for debugging
    // fillRect(x - max_weather_width, y - currentFont->advance_y, max_weather_width, currentFont->advance_y * 2,
    // DarkGrey);

    String w_text = TitleCase(shared::WxConditions.Forecast0);
    if (shared::WxConditions.Rainfall > 0)
        w_text += " (" + String(shared::WxConditions.Rainfall, 1) + String((cfg::UseMetricUnits ? "mm" : "in")) + ")";

    int split_pos = get_split_pos_2_lines(w_text.c_str(), max_weather_width);

    if (split_pos == -1)
    {
        int w = text_width(w_text.c_str());
        if (w / 2 + icon_center_line_x < x)
            drawString(icon_center_line_x, y, w_text, CENTER, t_color); // center, if it fits
        else
            drawString(x, y, w_text, RIGHT, t_color); // right-align, otherwise

        return false;
    }
    else
    {
        String l1 = w_text.substring(0, split_pos);
        String l2 = w_text.substring(split_pos + 1);
        int w1    = text_width(l1.c_str());
        int w2    = text_width(l2.c_str());
        int w_max = std::max(w1, w2);

        if (w_max / 2 + icon_center_line_x < x)
        {
            drawString(icon_center_line_x, y, l2, CENTER, t_color); // 2nd line
            drawString(icon_center_line_x, y - currentFont->advance_y + line_height_adjust, l1, CENTER,
                       t_color); // 1st line
        }
        else
        {
            drawString(x, y, l2, RIGHT, t_color);                                               // 2nd line
            drawString(x, y - currentFont->advance_y + line_height_adjust, l1, RIGHT, t_color); // 1st line
        }
        return true;
    }
}

void DisplayWeatherIconAndTextSection(int x, int y)
{
    bool two_lines = DisplayWeatherText(x, y);

    int icon_y = y - (two_lines ? 94 : 76);
    DisplayMainWeatherIcon(x - 115, icon_y); // Display weather icon scale = Large;
}

// clang-format on
} // namespace

// Main Weather Section
namespace
{
// clang-format off

void DrawUVI(int x, int y) {
  Rect_t area = {
    .x = x, .y = y, .width  = uvi_width, .height = uvi_height
  };
  drawGrayscaleImage(area,  uvi_data);
}

void Display_UVIndexLevel(int x, int y, float UVI) {
  String Level = "";
  if (UVI <= 2)              Level = " (L)";
  if (UVI >= 3 && UVI <= 5)  Level = " (M)";
  if (UVI >= 6 && UVI <= 7)  Level = " (H)";
  if (UVI >= 8 && UVI <= 10) Level = " (VH)";
  if (UVI >= 11)             Level = " (EX)";
  drawString(x + 20, y - 5, String(UVI, (UVI < 0 ? 1 : 0)) + Level, LEFT);
  DrawUVI(x - 10, y - 5);
}

void CloudCover(int x, int y, int CloudCover) {
  addcloud(x - 9, y,     Small * 0.3, 2); // Cloud top left
  addcloud(x + 3, y - 2, Small * 0.3, 2); // Cloud top right
  addcloud(x, y + 15,    Small * 0.6, 2); // Main cloud
  drawString(x + 30, y, String(CloudCover) + "%", LEFT);
}

void Visibility(int x, int y, String Visibility) {
  float start_angle = 0.52, end_angle = 2.61, Offset = 10;
  int r = 14;
  for (float i = start_angle; i < end_angle; i = i + 0.05) {
    drawPixel(x + r * cos(i), y - r / 2.0 + r * sin(i) + Offset, Black);
    drawPixel(x + r * cos(i), 1 + y - r / 2.0 + r * sin(i) + Offset, Black);
  }
  start_angle = 3.61; end_angle = 5.78;
  for (float i = start_angle; i < end_angle; i = i + 0.05) {
    drawPixel(x + r * cos(i), y + r / 2.0 + r * sin(i) + Offset, Black);
    drawPixel(x + r * cos(i), 1 + y + r / 2.0 + r * sin(i) + Offset, Black);
  }
  fillCircle(x, y + Offset, r / 4, Black);
  drawString(x + 20, y, Visibility, LEFT);
}

void DisplayVisiCCoverUVISection(int x, int y) {
  auto const& wx = shared::WxConditions;

  setFont(OpenSans12B);
  Visibility(x + 5, y, String(wx.Visibility) + "M");
  CloudCover(x + 155, y, wx.Cloudcover);
  if(!std::isnan(wx.UVI))
    Display_UVIndexLevel(x + 265, y, wx.UVI);
}

void DrawSegment(int x, int y, int o1, int o2, int o3, int o4, int o11, int o12, int o13, int o14) {
  drawLine(x + o1,  y + o2,  x + o3,  y + o4,  Black);
  drawLine(x + o11, y + o12, x + o13, y + o14, Black);
}

void DrawPressureAndTrend(int x, int y, float pressure, String slope) {
  drawString(x + 25, y - 10, String(pressure, (cfg::UseMetricUnits ? 0 : 1)) + (cfg::UseMetricUnits ? "hPa" : "in"), LEFT);
  if      (slope == "+") {
    DrawSegment(x, y, 0, 0, 8, -8, 8, -8, 16, 0);
    DrawSegment(x - 1, y, 0, 0, 8, -8, 8, -8, 16, 0);
  }
  else if (slope == "0") {
    DrawSegment(x, y, 8, -8, 16, 0, 8, 8, 16, 0);
    DrawSegment(x - 1, y, 8, -8, 16, 0, 8, 8, 16, 0);
  }
  else if (slope == "-") {
    DrawSegment(x, y, 0, 0, 8, 8, 8, 8, 16, 0);
    DrawSegment(x - 1, y, 0, 0, 8, 8, 8, 8, 16, 0);
  }
}

void DisplayTempHumiPressSection(int x, int y) {
  auto const& wx = shared::WxConditions;
  setFont(OpenSans18B);
  drawString(x - 30, y, String(wx.Temperature, 1) + "°" + (cfg::UseMetricUnits? "C" : "F")+"   " + String(wx.Humidity, 0) + "%", LEFT);
  setFont(OpenSans12B);
  DrawPressureAndTrend(x + 215, y + 15, wx.Pressure, wx.Trend);
}

// clang-format on

void DisplayTempHiLoFL(int x, int y)
{

    auto const& wx = shared::WxConditions;

    constexpr int32_t max_h      = max_of(ImgTempFL_height, ImgTempHi_height, ImgTempLo_height);
    constexpr auto panes_spacing = 40;
    constexpr auto text_spacing  = 5;
    auto text_y                  = y + 8;

    int dw;
    x -= 17;

    setFont(OpenSans12B);
    drawGrayscaleImage(ImgTempFL_info(x, y + (max_h - ImgTempFL_height) / 2));
    x += ImgTempFL_width + text_spacing;
    dw = drawString(x, text_y, String(wx.FeelsLike, 1) + "°", LEFT);
    x += dw;
    x += panes_spacing - 8;

    drawGrayscaleImage(ImgTempHi_info(x, y + (max_h - ImgTempHi_height) / 2));
    x += ImgTempHi_width + text_spacing;
    dw = drawString(x, text_y, String(wx.High, 0) + "°", LEFT);
    x += dw;
    x += panes_spacing;

    drawGrayscaleImage(ImgTempLo_info(x, y + (max_h - ImgTempLo_height) / 2));
    x += ImgTempLo_width + text_spacing;
    drawString(x, text_y, String(wx.Low, 0) + "°", LEFT);
}

// clang-format off

void DisplayMainWeatherSection(int x, int y) {
  setFont(OpenSans8B);
  DisplayTempHumiPressSection(x, y - 60);
  DisplayTempHiLoFL(x, y - 12);
  DisplayVisiCCoverUVISection(x - 10, y + 43);
}

// clang-format on
} // namespace

// Astronomy Section
namespace
{
// clang-format off
void DrawSunsetImage(int x, int y) {
  Rect_t area = {
    .x = x, .y = y, .width  = sunset_width, .height =  sunset_height
  };
  drawGrayscaleImage(area,  sunset_data);
}

void DrawSunriseImage(int x, int y) {
  Rect_t area = {
    .x = x, .y = y, .width  = sunrise_width, .height =  sunrise_height
  };
  drawGrayscaleImage(area,  sunrise_data);
}

int JulianDate(int d, int m, int y) {
  int mm, yy, k1, k2, k3, j;
  yy = y - (int)((12 - m) / 10);
  mm = m + 9;
  if (mm >= 12) mm = mm - 12;
  k1 = (int)(365.25 * (yy + 4712));
  k2 = (int)(30.6001 * mm + 0.5);
  k3 = (int)((int)((yy / 100) + 49) * 0.75) - 38;
  // 'j' for dates in Julian calendar:
  j = k1 + k2 + d + 59 + 1;
  if (j > 2299160) j = j - k3; // 'j' is the Julian date at 12h UT (Universal Time) For Gregorian calendar:
  return j;
}

double NormalizedMoonPhase(int d, int m, int y) {
  int j = JulianDate(d, m, y);
  //Calculate approximate moon phase
  double Phase = (j + 4.867) / 29.53059;
  return (Phase - (int) Phase);
}

// clang-format on

void DrawMoon(int x, int y, int diameter, int dd, int mm, int yy, bool is_south_hemisphere)
{
    // Here, the original code first drew the moon image directly on the screen and then created a separate "shade
    // image" in the framebuffer by drawing a filled black circle and then creating pure white fill in the areas that
    // should not be shaded.
    // Due to how EPDs works, when the framebuffer was finally flushed to the screen, the black parts of the shade
    // overlay darkened the moon image that was already on-screen.
    //
    // This new version operates entirely in the framebuffer: it first blits the moon image into the framebuffer and
    // then darkens the color of framebuffer pixels where the shade should be.
    //
    // Finally, the shaded moon phase image is flushed to the screen with the rest of the framebuffer content.
    //
    // This was mainly changed to allows operating in host emulator mode.

    double Phase = NormalizedMoonPhase(dd, mm, yy);
    if (is_south_hemisphere) Phase = 1 - Phase;

    auto const darken_by = 0x66;

    int y_upper_used = -1; // used to help avoid overdraw
    int y_lower_used = -1;

    // Draw dark part of moon
    int const number_of_lines = 90;
    for (double Ypos = 0; Ypos <= number_of_lines / 2.0; Ypos++)
    {
        double Xpos = sqrt(number_of_lines / 2.0 * number_of_lines / 2 - Ypos * Ypos);
        // Determine the edges of the lighted part of the moon
        double Rpos = 2 * Xpos;
        double Xpos1, Xpos2;
        if (Phase < 0.5)
        {
            Xpos1 = Rpos - 2 * Phase * Rpos - Xpos;
            Xpos2 = Xpos;
        }
        else
        {
            Xpos1 = -Xpos;
            Xpos2 = Xpos - 2 * Phase * Rpos + Rpos;
        }

        double x_start = (Xpos1 + number_of_lines) / number_of_lines * diameter + x;
        double x_end   = (Xpos2 + number_of_lines) / number_of_lines * diameter + x;

        // the shade is symmetrical w.r.t. the x axis, so we draw the upper and the lower part using the same
        // x_start and x_end calculation.
        uint16_t y_upper = (number_of_lines - Ypos) / number_of_lines * diameter + y;
        uint16_t y_lower = (Ypos + number_of_lines) / number_of_lines * diameter + y;

        // however, we must avoid darkening the same line twice as this results in double darkening.
        if (y_upper != y_upper_used)
        {
            sumColorHLine(x_start, x_end, y_upper, -((int16_t)darken_by));
            y_upper_used = y_upper;
        }

        if (y_upper == y_lower)
        {
            y_lower_used = y_lower;
            continue;
        }

        if (y_lower != y_lower_used)
        {
            sumColorHLine(x_start, x_end, y_lower, -((int16_t)darken_by));
            y_lower_used = y_lower;
        }
    }

    // finally, draw a black outline of the moon image
    drawCircle(x + diameter - 1, y + diameter, diameter / 2, Black);
}

// clang-format off

void DrawMoonImage(int x, int y) {
  Rect_t area = {
    .x = x, .y = y, .width  = moon_width, .height =  moon_height
  };
  drawGrayscaleImage(area,  moon_data);
}

String MoonPhase(int d, int m, int y, bool is_south_hemisphere) {
  int c, e;
  double jd;
  int b;
  if (m < 3) {
    y--;
    m += 12;
  }
  ++m;
  c   = 365.25 * y;
  e   = 30.6  * m;
  jd  = c + e + d - 694039.09;     /* jd is total days elapsed */
  jd /= 29.53059;                        /* divide by the moon cycle (29.53 days) */
  b   = jd;                              /* int(jd) -> b, take integer part of jd */
  jd -= b;                               /* subtract integer part to leave fractional part of original jd */
  b   = jd * 8 + 0.5;                /* scale fraction from 0-8 and round by adding 0.5 */
  b   = b & 7;                           /* 0 and 8 are the same phase so modulo 8 for 0 */
  if (is_south_hemisphere) b = 7 - b;
  if (b == 0) return TXT_MOON_NEW;              // New;              0%  illuminated
  if (b == 1) return TXT_MOON_WAXING_CRESCENT;  // Waxing crescent; 25%  illuminated
  if (b == 2) return TXT_MOON_FIRST_QUARTER;    // First quarter;   50%  illuminated
  if (b == 3) return TXT_MOON_WAXING_GIBBOUS;   // Waxing gibbous;  75%  illuminated
  if (b == 4) return TXT_MOON_FULL;             // Full;            100% illuminated
  if (b == 5) return TXT_MOON_WANING_GIBBOUS;   // Waning gibbous;  75%  illuminated
  if (b == 6) return TXT_MOON_THIRD_QUARTER;    // Third quarter;   50%  illuminated
  if (b == 7) return TXT_MOON_WANING_CRESCENT;  // Waning crescent; 25%  illuminated
  return "";
}

void DisplayAstronomySection(int x, int y) {
  // The original code uses gmtime() here and CycleStart is local time
  // shouldn't cause much difference
  struct tm const * now_utc = &shared::CycleStart;
  bool is_south_hemisphere = cfg::Latitude[0] == '-';

  setFont(OpenSans10B);
  drawString(x + 5, y + 102, MoonPhase(now_utc->tm_mday, now_utc->tm_mon + 1, now_utc->tm_year + 1900, is_south_hemisphere), LEFT);
  DrawMoonImage(x + 10, y + 23); // Different references!
  DrawMoon(x - 28, y - 15, 75, now_utc->tm_mday, now_utc->tm_mon + 1, now_utc->tm_year + 1900, is_south_hemisphere); // Spaced at 1/2 moon size, so 10 - 75/2 = -28
  drawString(x + 115, y + 40, ConvertUnixTime(shared::LocData.Sunrise).substring(0, 5), LEFT); // Sunrise
  drawString(x + 115, y + 80, ConvertUnixTime(shared::LocData.Sunset).substring(0, 5), LEFT);  // Sunset
  DrawSunriseImage(x + 180, y + 20);
  DrawSunsetImage(x + 180, y + 60);
}

// clang-format on
} // namespace

// Wind Section
namespace
{
// clang-format off
String WindDegToOrdinalDirection(float winddirection) {
  if (winddirection >= 348.75 || winddirection < 11.25)  return TXT_N;
  if (winddirection >=  11.25 && winddirection < 33.75)  return TXT_NNE;
  if (winddirection >=  33.75 && winddirection < 56.25)  return TXT_NE;
  if (winddirection >=  56.25 && winddirection < 78.75)  return TXT_ENE;
  if (winddirection >=  78.75 && winddirection < 101.25) return TXT_E;
  if (winddirection >= 101.25 && winddirection < 123.75) return TXT_ESE;
  if (winddirection >= 123.75 && winddirection < 146.25) return TXT_SE;
  if (winddirection >= 146.25 && winddirection < 168.75) return TXT_SSE;
  if (winddirection >= 168.75 && winddirection < 191.25) return TXT_S;
  if (winddirection >= 191.25 && winddirection < 213.75) return TXT_SSW;
  if (winddirection >= 213.75 && winddirection < 236.25) return TXT_SW;
  if (winddirection >= 236.25 && winddirection < 258.75) return TXT_WSW;
  if (winddirection >= 258.75 && winddirection < 281.25) return TXT_W;
  if (winddirection >= 281.25 && winddirection < 303.75) return TXT_WNW;
  if (winddirection >= 303.75 && winddirection < 326.25) return TXT_NW;
  if (winddirection >= 326.25 && winddirection < 348.75) return TXT_NNW;
  return "?";
}

void arrow(int x, int y, int asize, float aangle, int pwidth, int plength) {
  // aangle is the meteorological wind direction, that is,
  // the direction the wind is blowing _FROM_, with North = 0.
  // So, we must draw the arrow pointing the other way.
  aangle += 180;

  float dx = (asize - 10) * cos((aangle - 90) * PI / 180) + x; // calculate X position
  float dy = (asize - 10) * sin((aangle - 90) * PI / 180) + y; // calculate Y position
  float x1 =  0;            float y1 = plength;
  float x2 =  pwidth / 2.0; float y2 = pwidth / 2.0;
  float x3 = -pwidth / 2.0; float y3 = pwidth / 2.0;
  float angle = aangle * PI / 180 - 135;
  float xx1 = x1 * cos(angle) - y1 * sin(angle) + dx;
  float yy1 = y1 * cos(angle) + x1 * sin(angle) + dy;
  float xx2 = x2 * cos(angle) - y2 * sin(angle) + dx;
  float yy2 = y2 * cos(angle) + x2 * sin(angle) + dy;
  float xx3 = x3 * cos(angle) - y3 * sin(angle) + dx;
  float yy3 = y3 * cos(angle) + x3 * sin(angle) + dy;
  fillTriangle(xx1, yy1, xx3, yy3, xx2, yy2, Black);
}

// clang-format on

void wind_barbs(int x, int y, int radius, float wind_dir, int wind_speed_knots)
{
    // https://www.weather.gov/hfo/windbarbinfo

    int original_speed = wind_speed_knots;

    // if wind speed is >=50 the first symbol drawn is a triangle, so in that case
    // we don't need that much margin between the windbarb's line end and the outer compass circle
    if (wind_speed_knots >= 50)
        radius -= 2;
    else
        radius -= 8;

    float unit_x, unit_y;
    std::tie(unit_x, unit_y) = circle_unit_pos(wind_dir - 90);

    // always draw the "blows to" direction line
    drawDottedLine(x, y, x - unit_x * (radius - 5), y - unit_y * (radius - 5), Black, 0x88);

    constexpr auto barb_spacing   = 3;
    constexpr auto short_barb_len = 5;
    constexpr auto long_barb_len  = 5;

    constexpr auto penant_base_len   = 9;
    constexpr auto penant_height_len = 15;

    // no barb if calm
    if (wind_speed_knots < 3) return;

    // draw the "from direction" line mark
    drawLine(x + unit_x * radius, y + unit_y * radius, x, y, Black);

    int used_len = 0;

    int pennant_count = wind_speed_knots / 50;
    if (pennant_count > 0)
    {
        for (int i = 0; i < pennant_count; i++)
        {
            wind_speed_knots -= 50;

            int far_len  = radius - used_len;
            int mid_len  = far_len - penant_base_len / 2;
            int near_len = far_len - penant_base_len;

            int penn_far_x = far_len * unit_x + x, penn_far_y = far_len * unit_y + y;
            int penn_mid_x = mid_len * unit_x + x, penn_mid_y = mid_len * unit_y + y;
            int penn_near_x = near_len * unit_x + x, penn_near_y = near_len * unit_y + y;

            std::tie(penn_mid_x, penn_mid_y) =
                circle_pos(penn_mid_x, penn_mid_y, wind_dir - 90 + 90, penant_height_len);

            fillTriangle(penn_far_x, penn_far_y, penn_mid_x, penn_mid_y, penn_near_x, penn_near_y, Black);

            used_len += penant_base_len;
        }

        used_len += 3;
    }

    int barb_count = ((int)round(wind_speed_knots) + 2.5) / 5;

    // a single _short_ barb is drawn slightly offset, to help distinguish it from a single _long_ barb.
    if (barb_count == 1 && original_speed < 50) used_len += barb_spacing * 2;

    while (barb_count > 0)
    {
        int current_barb_len = barb_count >= 2 ? penant_height_len : penant_height_len / 2;

        for (int n = 0; n < 2; n++)
        {
            int dist         = radius - used_len - n;
            int barb_start_x = dist * unit_x + x, barb_start_y = dist * unit_y + y;

            int barb_end_x, barb_end_y;
            std::tie(barb_end_x, barb_end_y) =
                circle_pos(barb_start_x, barb_start_y, wind_dir - 90 + 75, current_barb_len);

            drawLine(barb_start_x, barb_start_y, barb_end_x, barb_end_y, Black);
        }

        barb_count -= 2;
        used_len += 5;
    }
}

// clang-format off

void DisplayDisplayWindSection(int x, int y, float angle, float windspeed, int Cradius) {

  constexpr auto draw_outer_ticks = false;
  constexpr auto draw_inner_ticks = true;
  constexpr float inner_circle_ration = 0.68f;

  float wind_speed_knots =
    cfg::UseMetricUnits ? windspeed * 1.94384 : windspeed * 0.8689;

  setFont(OpenSans8B);
  int dxo, dyo, dxi, dyi;
  drawCircle(x, y, Cradius, Black);       // Draw compass circle
  drawCircle(x, y, Cradius + 1, Black);   // Draw compass circle
  drawCircle(x, y, Cradius * inner_circle_ration, Black); // Draw compass inner circle
  for (float a = 0; a < 360; a = a + 22.5) {
    dxo = Cradius * cos((a - 90) * PI / 180);
    dyo = Cradius * sin((a - 90) * PI / 180);
    if (a == 45)  drawString(dxo + x + 15, dyo + y - 18, TXT_NE, CENTER);
    if (a == 135) drawString(dxo + x + 20, dyo + y - 2,  TXT_SE, CENTER);
    if (a == 225) drawString(dxo + x - 20, dyo + y - 2,  TXT_SW, CENTER);
    if (a == 315) drawString(dxo + x - 15, dyo + y - 18, TXT_NW, CENTER);
    if(draw_outer_ticks) {
      dxi = dxo * 0.9;
      dyi = dyo * 0.9;
      drawLine(dxo + x, dyo + y, dxi + x, dyi + y, Black);
    }
    if(draw_inner_ticks) {
      dxo = dxo * inner_circle_ration;
      dyo = dyo * inner_circle_ration;
      dxi = dxo * 0.9;
      dyi = dyo * 0.9;
      drawLine(dxo + x, dyo + y, dxi + x, dyi + y, Black);
    }
  }
  drawString(x, y - Cradius - 20,     TXT_N, CENTER);
  drawString(x, y + Cradius + 10,     TXT_S, CENTER);
  drawString(x - Cradius - 15, y - 5, TXT_W, CENTER);
  drawString(x + Cradius + 10, y - 5, TXT_E, CENTER);
  drawString(x + 3, y + 50, String(angle, 0) + "°", CENTER);
  setFont(OpenSans12B);
  drawString(x, y - 50, WindDegToOrdinalDirection(angle), CENTER);
  setFont(OpenSans24B);
  drawString(x + 3, y - 18, String(windspeed, 1), CENTER);
  setFont(OpenSans12B);
  drawString(x, y + 25, (cfg::UseMetricUnits ? "m/s" : "mph"), CENTER);

  arrow(x, y, Cradius - 22, angle, 18, 33); // Show wind direction on outer circle of width and length

  int wnd_speed_rnd = round(wind_speed_knots);
  if (wnd_speed_rnd >= 3)
    wind_barbs(x, y, Cradius, angle, wnd_speed_rnd);

}

// clang-format on
} // namespace

// Air Quality Section
namespace
{
// clang-format on

// that is, how many level thresholds are there, so there are 5 levels actually
constexpr auto aiq_scale_levels = 4;

struct IndicatorSpec
{
    size_t offset;                     // offset of member in Air_quality_record struct
    uint16_t levels[aiq_scale_levels]; // gauge level thresholds
    char const *main;                  // main text
    char const *sub;                   // subscript text
#ifdef __PLATFORMIO_BUILD_DEBUG__
    AQIndicator e_val; // used to check whether Indicators[] and AQIndicator have the same order
#endif
};

#ifdef __PLATFORMIO_BUILD_DEBUG__
#define CVAL(v) .e_val = v
#else
#define CVAL(v)
#endif

// clang-format off
constexpr IndicatorSpec const Indicators[] =
{
    {
      .offset = offsetof(Air_quality_record, SO2),
      .levels = {20, 80, 250 , 350},
      .main = "SO",
      .sub = "2",
      CVAL(AQIndicator::SO2)
    },
    {
      .offset = offsetof(Air_quality_record, NO2),
      .levels = {40, 70, 150, 200},
      .main = "NO",
      .sub = "2",
      CVAL(AQIndicator::NO2)
    },
    {
      .offset = offsetof(Air_quality_record, PM10),
      .levels = {20, 50, 100, 200},
      .main = "PM",
      .sub = "10",
      CVAL(AQIndicator::PM10)
    },
    {
      .offset = offsetof(Air_quality_record, PM2_5),
      .levels = {10, 25, 50, 75},
      .main = "PM",
      .sub = "2.5",
      CVAL(AQIndicator::PM2_5)
    },
    {
      .offset = offsetof(Air_quality_record, O3),
      .levels = {60, 100, 140, 180},
      .main = "O",
      .sub = "3",
      CVAL(AQIndicator::O3)
    },
    {
      .offset = offsetof(Air_quality_record, CO),
      .levels = {4400, 9400, 12400, 15400},
      .main = "CO",
      CVAL(AQIndicator::CO)
    },
    {
      .offset = offsetof(Air_quality_record, NH3),
      .levels = {999, 999, 999, 999},
      .main = "NH",
      .sub = "3",
      CVAL(AQIndicator::NH3)
    },
    {
      .offset = offsetof(Air_quality_record, NO),
      .levels = {999, 999, 999, 999},
      .main = "NO",
      CVAL(AQIndicator::NO)
    },
};
// clang-format on

#undef CVAL

#ifdef __PLATFORMIO_BUILD_DEBUG__
// check that enum values are ordered the same as Indicators[]
static_assert(Indicators[0].e_val == (AQIndicator)0);
static_assert(Indicators[1].e_val == (AQIndicator)1);
static_assert(Indicators[2].e_val == (AQIndicator)2);
static_assert(Indicators[3].e_val == (AQIndicator)3);
static_assert(Indicators[4].e_val == (AQIndicator)4);
static_assert(Indicators[5].e_val == (AQIndicator)5);
static_assert(Indicators[6].e_val == (AQIndicator)6);
static_assert(Indicators[7].e_val == (AQIndicator)7);
#endif

void DrawGauge(int x, int y, AQIndicator ind)
{
    IndicatorSpec const& spec = Indicators[(uint8_t)ind];
    auto val_ptr              = ((uint8_t const *)&shared::WxAirQ) + spec.offset;
    float val                 = *(float const *)(val_ptr);

    int level = 0;
    while (level < aiq_scale_levels && val >= spec.levels[level])
        level++;

    switch (level)
    {
        case 0: drawGrayscaleImage(ImgGaugeFrame0_info(x, y)); break;
        case 1: drawGrayscaleImage(ImgGaugeFrame1_info(x, y)); break;
        case 2: drawGrayscaleImage(ImgGaugeFrame2_info(x, y)); break;
        case 3: drawGrayscaleImage(ImgGaugeFrame3_info(x, y)); break;
        case 4: drawGrayscaleImage(ImgGaugeFrame4_info(x, y)); break;
        default: break;
    }

    constexpr auto gauge_center_offset_x = ImgGaugeFrame0_width / 2;
    constexpr auto gauge_txt_baseline_y  = 17 + 1;
    constexpr auto txt_subscript_shift_y = 8;

    setFont(OpenSans6B);
    int dw = drawString(x + gauge_center_offset_x, y + gauge_txt_baseline_y, spec.main, CENTER);
    if (spec.sub)
    {
        setFont(OpenSans5CB_Special2);
        drawString(x + gauge_center_offset_x + dw / 2, y + gauge_txt_baseline_y + txt_subscript_shift_y, spec.sub,
                   LEFT);
    }

    setFont(OpenSans8B);
    // all other indicators range < 1000, CO reaches 20 000, so scale it so it fits in 3 digits
    if (ind == AQIndicator::CO) val /= 100.0;
    int32_t disp_val = round(val);
    if (disp_val > 999) disp_val = 999;
    drawString(x + ImgGaugeFrame0_width + 3, y + 23 - 12, String(disp_val), LEFT);
}

void DisplayAQI(int x, int y)
{
    drawGrayscaleImage(ImgAIQ_info(x, y));
    setFont(OpenSans8B);
    // OWM's api uses 1=best...5=worst and we use x/5 scale, so we need to reverse the output
    int q = 6 - shared::WxAirQ.AQI;
    drawString(x + ImgAIQ_width + 3, y + 10, String(q) + "/5", LEFT);
}

void DisplayAirQualitySection(int x, int y)
{
    constexpr auto aiq_w   = 74;
    constexpr auto gauge_w = 84;

    DisplayAQI(x, y + 1);

    DrawGauge(x + +aiq_w, y, cfg::AQIComponents[0]);
    DrawGauge(x + aiq_w + gauge_w, y, cfg::AQIComponents[1]);
    DrawGauge(x + aiq_w + 2 * gauge_w, y, cfg::AQIComponents[2]);
}

// clang-format on
} // namespace

// General Info Section
namespace
{
// clang-format off
void CalcDateTimeString(String&  Date_str, String&  Time_str){

  struct tm const& timeinfo = shared::CycleStart;

  char   time_output[30], day_output[30], update_time[30];

  if (cfg::UseMetricUnits) {
    sprintf(day_output, "%s, %02u %s %04u", weekday_D[timeinfo.tm_wday], timeinfo.tm_mday, month_M[timeinfo.tm_mon], (timeinfo.tm_year) + 1900);
    strftime(update_time, sizeof(update_time), "%H:%M:%S", &timeinfo);  // Creates: '@ 14:05:49'
    sprintf(time_output, "%s", update_time);
  }
  else
  {
    strftime(day_output, sizeof(day_output), "%a %b-%d-%Y", &timeinfo); // Creates  'Sat May-31-2019'
    strftime(update_time, sizeof(update_time), "%r", &timeinfo);        // Creates: '@ 02:05:49pm'
    sprintf(time_output, "%s", update_time);
  }

  Date_str = day_output;
  Time_str = time_output;

}
void DisplayGeneralInfoSection() {

  setFont(OpenSans5CB_Special2);
  drawString(SCREEN_WIDTH - 3, SCREEN_HEIGHT - 10, "v" + String(shared::app_ver), RIGHT);

  setFont(OpenSans10B);
  drawString(5, 2, cfg::City, LEFT);

  int text_right = 769;

  if (shared::CycleStart.tm_isdst) {
     drawGrayscaleImage(ImgDST_info(text_right - ImgDST_width , 4));
     text_right -= ImgDST_width + 4;
  }

  setFont(OpenSans8B);

  String  Date_str;
  String  Time_str;
  CalcDateTimeString(Date_str, Time_str);

  int dw = drawString(text_right, 5, Date_str + "  @  " + Time_str + " " + shared::TZName , RIGHT);

  if(!shared::ActiveHours) {
    Rect_t r = {text_right - dw -2, 3, dw + 3, 20};
    invertRect(r);
  }
}

// clang-format on
} // namespace

// Status Section
namespace
{
// clang-format off
void DrawBattery(int x, int y) {
  uint8_t percentage = 100;

  auto const voltage = shared::voltage;

  if (voltage > 1 ) { // Only display if there is a valid reading
    percentage = 2836.9625 * pow(voltage, 4) - 43987.4889 * pow(voltage, 3) + 255233.8134 * pow(voltage, 2) - 656689.7123 * voltage + 632041.7303;
    if (voltage >= 4.20) percentage = 100;
    if (voltage <= 3.20) percentage = 0;  // orig 3.5
    drawRect(x + 25, y - 14, 40, 15, Black);
    fillRect(x + 65, y - 10, 4, 7, Black);
    fillRect(x + 27, y - 12, 36 * percentage / 100.0, 11, Black);
    drawString(x + 85, y - 14, String(percentage) + "%  " + String(voltage, 1) + "v", LEFT);
  }
}

void DrawRSSI(int x, int y, int rssi) {
  int WIFIsignal = 0;
  int xpos = 1;
  for (int _rssi = -100; _rssi <= rssi; _rssi = _rssi + 20) {
    if (_rssi <= -20)  WIFIsignal = 30; //            <-20dbm displays 5-bars
    if (_rssi <= -40)  WIFIsignal = 24; //  -40dbm to  -21dbm displays 4-bars
    if (_rssi <= -60)  WIFIsignal = 18; //  -60dbm to  -41dbm displays 3-bars
    if (_rssi <= -80)  WIFIsignal = 12; //  -80dbm to  -61dbm displays 2-bars
    if (_rssi <= -100) WIFIsignal = 6;  // -100dbm to  -81dbm displays 1-bar
    fillRect(x + xpos * 8, y - WIFIsignal, 6, WIFIsignal, Black);
    xpos++;
  }
}

void DisplayStatusSection(int x, int y, int rssi) {
  setFont(OpenSans8B);
  DrawRSSI(x + 305, y + 15, rssi);
  DrawBattery(x + 150, y);
}

// clang-format on
} // namespace

// Debug support
namespace
{
// clang-format on

void DisplayDebugTimingInfo()
{
    if (!Log_DrawTimings) return;

    auto wifi = get_event_duration(TimeEvent::ConnectWiFi);
    auto ntp  = get_event_duration(TimeEvent::SetUpNTP) + get_event_duration(TimeEvent::SyncTime);
    auto api  = get_event_duration(TimeEvent::FetchWeather) + get_event_duration(TimeEvent::ParseWeather) +
               get_event_duration(TimeEvent::FetchForecast) + get_event_duration(TimeEvent::ParseForecast) +
               get_event_duration(TimeEvent::FetchAQI) + get_event_duration(TimeEvent::ParseAQI) +
               get_event_duration(TimeEvent::CloseHttp);

    auto ui = get_event_duration(TimeEvent::DrawUI);

#if HOST_BUILD
    // keep it fixed to that the preview png doesn't change everytime
    ui = std::chrono::milliseconds{5};
#endif

    String dbg;
    dbg += "wifi: " + String(wifi.count()) + " ms; ";
    dbg += "ntp: " + String(ntp.count()) + " ms; ";
    dbg += "api: " + String(api.count()) + " ms; ";
    dbg += "ui: " + String(ui.count()) + " ms";

    setFont(OpenSans5CB_Special2);
    drawString(SCREEN_WIDTH - 50, 25, dbg, RIGHT);
}

// clang-format on
} // namespace

// clang-format off
void DisplayWeather() {
  OPT_LOG(Log_Lifecycle, Serial.println("Drawing UI..."));
  assert(framebuffer);

  mark_event(TimeEvent::DrawUI);
  DisplayStatusSection(600, 20, shared::wifi_signal);    // Wi-Fi signal strength and Battery voltage
  DisplayGeneralInfoSection();                           // Top line of the display
  DisplayDisplayWindSection(137, 150, shared::WxConditions.Winddir, shared::WxConditions.Windspeed, 100);
  DisplayAstronomySection(5, 252);                       // Astronomy section Sun rise/set, Moon phase and Moon icon
  DisplayMainWeatherSection(320, 110);                   // Centre section of display for Location, temperature, Weather report, current Wx Symbol
  DisplayWeatherIconAndTextSection(SCREEN_WIDTH - 10, 196);
  DisplayForecastSection(285, 220);                      // 3hr forecast boxes
  DisplayGraphSection(320, 220);                         // Graphs of pressure, temperature, humidity and rain or snowfall
  DisplayAirQualitySection(300, 195);
  mark_event_done(TimeEvent::DrawUI);

  DisplayDebugTimingInfo();

  OPT_LOG(Log_Lifecycle, Serial.println("Updating screen..."));
  clear_epd_flush_fb_and_power_off();
}

// clang-format on

namespace
{

Rect_t getErrorArea()
{
    int const m_x = 80, m_y = 60;
    return {.x = m_x, .y = m_y, .width = SCREEN_WIDTH - m_x * 2, .height = SCREEN_HEIGHT - m_y * 2};
}
} // namespace

void DisplayError(String const& message)
{
    assert(framebuffer);

    int const pad = 20;

    Rect_t ea = getErrorArea();
    fillRect(ea.x, ea.y, ea.width, ea.height, White);

    drawRect(ea.x, ea.y, ea.width, ea.height, Black);
    drawRect(ea.x + 1, ea.y + 1, ea.width - 2, ea.height - 2, Black);

    setFont(OpenSans8B);
    drawString_multiline(ea.x + pad, ea.y + pad, message);

    clear_epd_flush_fb_and_power_off(&ea);
}

bool InitGraphics() { return init_epd_alloc_fb(); }

uint8_t *Framebuffer() { return framebuffer; }