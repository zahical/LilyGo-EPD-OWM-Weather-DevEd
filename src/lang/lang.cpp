/*
 * Copyright (c) David Bird 2021. All rights to this software are reserved.
 * Copyright (c) 2024 zahical. All rights reserved.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/**
 * @file Localizations.
 */
#include "lang.h"

#ifdef LANG_FR_FR
// Temperature - Humidity - Forecast
char const *const TXT_TEMPERATURE_C    = "Température (°C)";
char const *const TXT_TEMPERATURE_F    = "Température (°F)";
char const *const TXT_HUMIDITY_PERCENT = "Humidité (%)";

// Pressure
char const *const TXT_PRESSURE_HPA = "Pression (hPa)";
char const *const TXT_PRESSURE_IN  = "Pression (in)";

// Moon
char const *const TXT_MOON_NEW             = "Nouvelle Lune";
char const *const TXT_MOON_WAXING_CRESCENT = "Premier Croissant";
char const *const TXT_MOON_FIRST_QUARTER   = "Premier Quartier";
char const *const TXT_MOON_WAXING_GIBBOUS  = "Gibbeuse Croissan.";
char const *const TXT_MOON_FULL            = "Pleine Lune";
char const *const TXT_MOON_WANING_GIBBOUS  = "Gibbeuse Décroiss.";
char const *const TXT_MOON_THIRD_QUARTER   = "Dernier Quartier";
char const *const TXT_MOON_WANING_CRESCENT = "Dernier Croissant";

// RainFall / SnowFall
char const *const TXT_RAINFALL_MM = "Pluie (mm)";
char const *const TXT_RAINFALL_IN = "Pluie (in)";
char const *const TXT_SNOWFALL_MM = "Neige (mm)";
char const *const TXT_SNOWFALL_IN = "Neige (in)";

// Wind
char const *const TXT_N   = "N";
char const *const TXT_NNE = "NNE";
char const *const TXT_NE  = "NE";
char const *const TXT_ENE = "ENE";
char const *const TXT_E   = "E";
char const *const TXT_ESE = "ESE";
char const *const TXT_SE  = "SE";
char const *const TXT_SSE = "SSE";
char const *const TXT_S   = "S";
char const *const TXT_SSW = "SSO";
char const *const TXT_SW  = "SO";
char const *const TXT_WSW = "OSO";
char const *const TXT_W   = "O";
char const *const TXT_WNW = "ONO";
char const *const TXT_NW  = "NO";
char const *const TXT_NNW = "NNO";

// Day of the week
char const *const weekday_D[] = {"Dim", "Lun", "Mar", "Mer", "Jeu", "Ven", "Sam"};

// Month
char const *const month_M[] = {"Jan", "Fév", "Mar", "Avr", "Mai", "Jun", "Jul", "Aou", "Sep", "Oct", "Nov", "Déc"};

#else

// Temperature - Humidity - Forecast
char const *const TXT_TEMPERATURE_C    = "Temperature (°C)";
char const *const TXT_TEMPERATURE_F    = "Temperature (°F)";
char const *const TXT_HUMIDITY_PERCENT = "Humidity (%)";

// Pressure
char const *const TXT_PRESSURE_IN  = "Pressure (in)";
char const *const TXT_PRESSURE_HPA = "Pressure (hPa)";

// Moon
char const *const TXT_MOON_NEW             = "New";
char const *const TXT_MOON_WAXING_CRESCENT = "Waxing Crescent";
char const *const TXT_MOON_FIRST_QUARTER   = "First Quarter";
char const *const TXT_MOON_WAXING_GIBBOUS  = "Waxing Gibbous";
char const *const TXT_MOON_FULL            = "Full";
char const *const TXT_MOON_WANING_GIBBOUS  = "Waning Gibbous";
char const *const TXT_MOON_THIRD_QUARTER   = "Third Quarter";
char const *const TXT_MOON_WANING_CRESCENT = "Waning Crescent";

// RainFall / SnowFall
char const *const TXT_RAINFALL_MM = "Rainfall (mm)";
char const *const TXT_RAINFALL_IN = "Rainfall (in)";
char const *const TXT_SNOWFALL_MM = "Snowfall (mm)";
char const *const TXT_SNOWFALL_IN = "Snowfall (in)";

// Wind
char const *const TXT_N   = "N";
char const *const TXT_NNE = "NNE";
char const *const TXT_NE  = "NE";
char const *const TXT_ENE = "ENE";
char const *const TXT_E   = "E";
char const *const TXT_ESE = "ESE";
char const *const TXT_SE  = "SE";
char const *const TXT_SSE = "SSE";
char const *const TXT_S   = "S";
char const *const TXT_SSW = "SSW";
char const *const TXT_SW  = "SW";
char const *const TXT_WSW = "WSW";
char const *const TXT_W   = "W";
char const *const TXT_WNW = "WNW";
char const *const TXT_NW  = "NW";
char const *const TXT_NNW = "NNW";

// Day of the week
char const *const weekday_D[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

// Month
char const *const month_M[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

#endif
