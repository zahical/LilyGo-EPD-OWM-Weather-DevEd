/*
 * Copyright (c) 2024 zahical. All rights reserved.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/**
 * @file Sample OWM API responses for testing.
 */

extern char const *const Sample_AQI{
    R"--(

{
  "coord": {
    "lon": 23.32,
    "lat": 42.69
  },
  "list": [
    {
      "main": {
        "aqi": 3
      },
      "components": {
        "co": 453.95,
        "no": 0,
        "no2": 20.56,
        "o3": 43.27,
        "so2": 5.72,
        "pm2_5": 29.11,
        "pm10": 48.32,
        "nh3": 3.42
      },
      "dt": 1732747858
    }
  ]
}


)--"

};

extern char const *const Sample_Weather{
    R"--(

{
    "coord": {
        "lon": 23.32,
        "lat": 42.69
    },
    "weather": [
        {
            "id": 802,
            "main": "Clouds",
            "description": "thunderstorm with heavy drizzle",
            "icon": "09n"
        }
    ],
    "base": "stations",
    "main": {
        "temp": -0.8,
        "feels_like": -7.46,
        "temp_min": -1.19,
        "temp_max": -0.49,
        "pressure": 1016,
        "humidity": 81,
        "sea_level": 1016,
        "grnd_level": 930
    },
    "visibility": 10000,
    "wind": {
        "speed": 38.23,
        "deg": 280
    },
    "rain": {
        "1h": 42.69
    },
    "clouds": {
        "all": 40
    },
    "dt": 1732326095,
    "sys": {
        "type": 2,
        "id": 2033225,
        "country": "BG",
        "sunrise": 1732339716,
        "sunset": 1732373890
    },
    "timezone": 7200,
    "id": 727011,
    "name": "Sofia",
    "cod": 200
}


)--"

};

extern char const *const Sample_Forecast{
    R"--(

{
    "cod": "200",
    "message": 0,
    "cnt": 40,
    "list": [
        {
            "dt": 1732330800,
            "main": {
                "temp": -0.8,
                "feels_like": -5.65,
                "temp_min": -1.92,
                "temp_max": -0.8,
                "pressure": 1017,
                "sea_level": 1017,
                "grnd_level": 931,
                "humidity": 81,
                "temp_kf": 1.12
            },
            "weather": [
                {
                    "id": 600,
                    "main": "Snow",
                    "description": "light snow",
                    "icon": "13n"
                }
            ],
            "clouds": {
                "all": 40
            },
            "wind": {
                "speed": 4.56,
                "deg": 280,
                "gust": 11.89
            },
            "visibility": 10000,
            "pop": 0.57,
            "rain": {
                "3h": 20.4
            },
            "sys": {
                "pod": "n"
            },
            "dt_txt": "2024-11-23 03:00:00"
        },
        {
            "dt": 1732341600,
            "main": {
                "temp": -1.26,
                "feels_like": -5.77,
                "temp_min": -2.19,
                "temp_max": -1.26,
                "pressure": 1018,
                "sea_level": 1018,
                "grnd_level": 934,
                "humidity": 80,
                "temp_kf": 0.93
            },
            "weather": [
                {
                    "id": 802,
                    "main": "Clouds",
                    "description": "scattered clouds",
                    "icon": "03d"
                }
            ],
            "clouds": {
                "all": 40
            },
            "wind": {
                "speed": 3.92,
                "deg": 273,
                "gust": 9.81
            },
            "visibility": 10000,
            "pop": 0.12,
            "sys": {
                "pod": "d"
            },
            "dt_txt": "2024-11-23 06:00:00"
        },
        {
            "dt": 1732352400,
            "main": {
                "temp": -0.15,
                "feels_like": -5.57,
                "temp_min": -0.15,
                "temp_max": 0.17,
                "pressure": 1022,
                "sea_level": 1022,
                "grnd_level": 938,
                "humidity": 73,
                "temp_kf": -0.32
            },
            "weather": [
                {
                    "id": 802,
                    "main": "Clouds",
                    "description": "scattered clouds",
                    "icon": "03d"
                }
            ],
            "clouds": {
                "all": 33
            },
            "wind": {
                "speed": 5.85,
                "deg": 286,
                "gust": 9.54
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "d"
            },
            "dt_txt": "2024-11-23 09:00:00"
        },
        {
            "dt": 1732363200,
            "main": {
                "temp": 1.16,
                "feels_like": -4.04,
                "temp_min": 1.16,
                "temp_max": 1.16,
                "pressure": 1025,
                "sea_level": 1025,
                "grnd_level": 939,
                "humidity": 66,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 802,
                    "main": "Clouds",
                    "description": "scattered clouds",
                    "icon": "03d"
                }
            ],
            "clouds": {
                "all": 40
            },
            "wind": {
                "speed": 6.14,
                "deg": 288,
                "gust": 10.26
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "d"
            },
            "dt_txt": "2024-11-23 12:00:00"
        },
        {
            "dt": 1732374000,
            "main": {
                "temp": -0.35,
                "feels_like": -5.12,
                "temp_min": -0.35,
                "temp_max": -0.35,
                "pressure": 1029,
                "sea_level": 1029,
                "grnd_level": 942,
                "humidity": 64,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 801,
                    "main": "Clouds",
                    "description": "few clouds",
                    "icon": "02n"
                }
            ],
            "clouds": {
                "all": 12
            },
            "wind": {
                "speed": 4.62,
                "deg": 281,
                "gust": 10.28
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "n"
            },
            "dt_txt": "2024-11-23 15:00:00"
        },
        {
            "dt": 1732384800,
            "main": {
                "temp": -1.15,
                "feels_like": -5.57,
                "temp_min": -1.15,
                "temp_max": -1.15,
                "pressure": 1032,
                "sea_level": 1032,
                "grnd_level": 945,
                "humidity": 68,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 800,
                    "main": "Clear",
                    "description": "clear sky",
                    "icon": "01n"
                }
            ],
            "clouds": {
                "all": 5
            },
            "wind": {
                "speed": 3.83,
                "deg": 267,
                "gust": 8.69
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "n"
            },
            "dt_txt": "2024-11-23 18:00:00"
        },
        {
            "dt": 1732395600,
            "main": {
                "temp": -1.28,
                "feels_like": -5.48,
                "temp_min": -1.28,
                "temp_max": -1.28,
                "pressure": 1035,
                "sea_level": 1035,
                "grnd_level": 947,
                "humidity": 73,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 801,
                    "main": "Clouds",
                    "description": "few clouds",
                    "icon": "02n"
                }
            ],
            "clouds": {
                "all": 24
            },
            "wind": {
                "speed": 3.51,
                "deg": 273,
                "gust": 7.48
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "n"
            },
            "dt_txt": "2024-11-23 21:00:00"
        },
        {
            "dt": 1732406400,
            "main": {
                "temp": -1.89,
                "feels_like": -5.3,
                "temp_min": -1.89,
                "temp_max": -1.89,
                "pressure": 1036,
                "sea_level": 1036,
                "grnd_level": 949,
                "humidity": 76,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 802,
                    "main": "Clouds",
                    "description": "scattered clouds",
                    "icon": "03n"
                }
            ],
            "clouds": {
                "all": 25
            },
            "wind": {
                "speed": 2.54,
                "deg": 264,
                "gust": 5.69
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "n"
            },
            "dt_txt": "2024-11-24 00:00:00"
        },
        {
            "dt": 1732417200,
            "main": {
                "temp": -2.65,
                "feels_like": -2.65,
                "temp_min": -2.65,
                "temp_max": -2.65,
                "pressure": 1038,
                "sea_level": 1038,
                "grnd_level": 950,
                "humidity": 82,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 800,
                    "main": "Clear",
                    "description": "clear sky",
                    "icon": "01n"
                }
            ],
            "clouds": {
                "all": 4
            },
            "wind": {
                "speed": 1.14,
                "deg": 276,
                "gust": 1.74
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "n"
            },
            "dt_txt": "2024-11-24 03:00:00"
        },
        {
            "dt": 1732428000,
            "main": {
                "temp": -2.57,
                "feels_like": -2.57,
                "temp_min": -2.57,
                "temp_max": -2.57,
                "pressure": 1038,
                "sea_level": 1038,
                "grnd_level": 950,
                "humidity": 80,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 800,
                    "main": "Clear",
                    "description": "clear sky",
                    "icon": "01d"
                }
            ],
            "clouds": {
                "all": 4
            },
            "wind": {
                "speed": 0.52,
                "deg": 271,
                "gust": 0.94
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "d"
            },
            "dt_txt": "2024-11-24 06:00:00"
        },
        {
            "dt": 1732438800,
            "main": {
                "temp": 2.17,
                "feels_like": 2.17,
                "temp_min": 2.17,
                "temp_max": 2.17,
                "pressure": 1038,
                "sea_level": 1038,
                "grnd_level": 951,
                "humidity": 56,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 800,
                    "main": "Clear",
                    "description": "clear sky",
                    "icon": "01d"
                }
            ],
            "clouds": {
                "all": 5
            },
            "wind": {
                "speed": 0.94,
                "deg": 289,
                "gust": 1.29
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "d"
            },
            "dt_txt": "2024-11-24 09:00:00"
        },
        {
            "dt": 1732449600,
            "main": {
                "temp": 4.49,
                "feels_like": 4.49,
                "temp_min": 4.49,
                "temp_max": 4.49,
                "pressure": 1035,
                "sea_level": 1035,
                "grnd_level": 949,
                "humidity": 47,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 800,
                    "main": "Clear",
                    "description": "clear sky",
                    "icon": "01d"
                }
            ],
            "clouds": {
                "all": 6
            },
            "wind": {
                "speed": 1.24,
                "deg": 289,
                "gust": 2.02
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "d"
            },
            "dt_txt": "2024-11-24 12:00:00"
        },
        {
            "dt": 1732460400,
            "main": {
                "temp": 1.06,
                "feels_like": 1.06,
                "temp_min": 1.06,
                "temp_max": 1.06,
                "pressure": 1034,
                "sea_level": 1034,
                "grnd_level": 948,
                "humidity": 65,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 800,
                    "main": "Clear",
                    "description": "clear sky",
                    "icon": "01n"
                }
            ],
            "clouds": {
                "all": 5
            },
            "wind": {
                "speed": 0.66,
                "deg": 260,
                "gust": 0.7
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "n"
            },
            "dt_txt": "2024-11-24 15:00:00"
        },
        {
            "dt": 1732471200,
            "main": {
                "temp": -0.12,
                "feels_like": -0.12,
                "temp_min": -0.12,
                "temp_max": -0.12,
                "pressure": 1035,
                "sea_level": 1035,
                "grnd_level": 948,
                "humidity": 62,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 800,
                    "main": "Clear",
                    "description": "clear sky",
                    "icon": "01n"
                }
            ],
            "clouds": {
                "all": 7
            },
            "wind": {
                "speed": 0.78,
                "deg": 229,
                "gust": 0.83
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "n"
            },
            "dt_txt": "2024-11-24 18:00:00"
        },
        {
            "dt": 1732482000,
            "main": {
                "temp": -0.01,
                "feels_like": -0.01,
                "temp_min": -0.01,
                "temp_max": -0.01,
                "pressure": 1034,
                "sea_level": 1034,
                "grnd_level": 947,
                "humidity": 56,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 803,
                    "main": "Clouds",
                    "description": "broken clouds",
                    "icon": "04n"
                }
            ],
            "clouds": {
                "all": 54
            },
            "wind": {
                "speed": 1.1,
                "deg": 250,
                "gust": 1.12
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "n"
            },
            "dt_txt": "2024-11-24 21:00:00"
        },
        {
            "dt": 1732492800,
            "main": {
                "temp": -0.52,
                "feels_like": -0.52,
                "temp_min": -0.52,
                "temp_max": -0.52,
                "pressure": 1034,
                "sea_level": 1034,
                "grnd_level": 947,
                "humidity": 56,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 803,
                    "main": "Clouds",
                    "description": "broken clouds",
                    "icon": "04n"
                }
            ],
            "clouds": {
                "all": 59
            },
            "wind": {
                "speed": 1.06,
                "deg": 257,
                "gust": 1.11
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "n"
            },
            "dt_txt": "2024-11-25 00:00:00"
        },
        {
            "dt": 1732503600,
            "main": {
                "temp": -0.5,
                "feels_like": -0.5,
                "temp_min": -0.5,
                "temp_max": -0.5,
                "pressure": 1033,
                "sea_level": 1033,
                "grnd_level": 946,
                "humidity": 57,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 800,
                    "main": "Clear",
                    "description": "clear sky",
                    "icon": "01n"
                }
            ],
            "clouds": {
                "all": 4
            },
            "wind": {
                "speed": 0.88,
                "deg": 264,
                "gust": 1.09
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "n"
            },
            "dt_txt": "2024-11-25 03:00:00"
        },
        {
            "dt": 1732514400,
            "main": {
                "temp": 0.06,
                "feels_like": 0.06,
                "temp_min": 0.06,
                "temp_max": 0.06,
                "pressure": 1033,
                "sea_level": 1033,
                "grnd_level": 946,
                "humidity": 55,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 802,
                    "main": "Clouds",
                    "description": "scattered clouds",
                    "icon": "03d"
                }
            ],
            "clouds": {
                "all": 26
            },
            "wind": {
                "speed": 0.53,
                "deg": 272,
                "gust": 0.7
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "d"
            },
            "dt_txt": "2024-11-25 06:00:00"
        },
        {
            "dt": 1732525200,
            "main": {
                "temp": 5.18,
                "feels_like": 5.18,
                "temp_min": 5.18,
                "temp_max": 5.18,
                "pressure": 1031,
                "sea_level": 1031,
                "grnd_level": 946,
                "humidity": 42,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 802,
                    "main": "Clouds",
                    "description": "scattered clouds",
                    "icon": "03d"
                }
            ],
            "clouds": {
                "all": 40
            },
            "wind": {
                "speed": 0.93,
                "deg": 90,
                "gust": 1.18
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "d"
            },
            "dt_txt": "2024-11-25 09:00:00"
        },
        {
            "dt": 1732536000,
            "main": {
                "temp": 7.54,
                "feels_like": 7.54,
                "temp_min": 7.54,
                "temp_max": 7.54,
                "pressure": 1029,
                "sea_level": 1029,
                "grnd_level": 944,
                "humidity": 40,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 802,
                    "main": "Clouds",
                    "description": "scattered clouds",
                    "icon": "03d"
                }
            ],
            "clouds": {
                "all": 25
            },
            "wind": {
                "speed": 0.74,
                "deg": 80,
                "gust": 1.3
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "d"
            },
            "dt_txt": "2024-11-25 12:00:00"
        },
        {
            "dt": 1732546800,
            "main": {
                "temp": 3.69,
                "feels_like": 2.68,
                "temp_min": 3.69,
                "temp_max": 3.69,
                "pressure": 1029,
                "sea_level": 1029,
                "grnd_level": 944,
                "humidity": 59,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 801,
                    "main": "Clouds",
                    "description": "few clouds",
                    "icon": "02n"
                }
            ],
            "clouds": {
                "all": 15
            },
            "wind": {
                "speed": 1.34,
                "deg": 95,
                "gust": 1.42
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "n"
            },
            "dt_txt": "2024-11-25 15:00:00"
        },
        {
            "dt": 1732557600,
            "main": {
                "temp": 1.68,
                "feels_like": 0.35,
                "temp_min": 1.68,
                "temp_max": 1.68,
                "pressure": 1030,
                "sea_level": 1030,
                "grnd_level": 944,
                "humidity": 61,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 803,
                    "main": "Clouds",
                    "description": "broken clouds",
                    "icon": "04n"
                }
            ],
            "clouds": {
                "all": 57
            },
            "wind": {
                "speed": 1.38,
                "deg": 95,
                "gust": 2.01
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "n"
            },
            "dt_txt": "2024-11-25 18:00:00"
        },
        {
            "dt": 1732568400,
            "main": {
                "temp": 1.23,
                "feels_like": 1.23,
                "temp_min": 1.23,
                "temp_max": 1.23,
                "pressure": 1029,
                "sea_level": 1029,
                "grnd_level": 942,
                "humidity": 58,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 804,
                    "main": "Clouds",
                    "description": "overcast clouds",
                    "icon": "04n"
                }
            ],
            "clouds": {
                "all": 100
            },
            "wind": {
                "speed": 0.79,
                "deg": 108,
                "gust": 0.99
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "n"
            },
            "dt_txt": "2024-11-25 21:00:00"
        },
        {
            "dt": 1732579200,
            "main": {
                "temp": 0.44,
                "feels_like": 0.44,
                "temp_min": 0.44,
                "temp_max": 0.44,
                "pressure": 1028,
                "sea_level": 1028,
                "grnd_level": 941,
                "humidity": 62,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 804,
                    "main": "Clouds",
                    "description": "overcast clouds",
                    "icon": "04n"
                }
            ],
            "clouds": {
                "all": 94
            },
            "wind": {
                "speed": 0.43,
                "deg": 127,
                "gust": 0.7
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "n"
            },
            "dt_txt": "2024-11-26 00:00:00"
        },
        {
            "dt": 1732590000,
            "main": {
                "temp": 0.18,
                "feels_like": 0.18,
                "temp_min": 0.18,
                "temp_max": 0.18,
                "pressure": 1026,
                "sea_level": 1026,
                "grnd_level": 940,
                "humidity": 63,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 804,
                    "main": "Clouds",
                    "description": "overcast clouds",
                    "icon": "04n"
                }
            ],
            "clouds": {
                "all": 97
            },
            "wind": {
                "speed": 0.22,
                "deg": 159,
                "gust": 0.54
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "n"
            },
            "dt_txt": "2024-11-26 03:00:00"
        },
        {
            "dt": 1732600800,
            "main": {
                "temp": 0.1,
                "feels_like": 0.1,
                "temp_min": 0.1,
                "temp_max": 0.1,
                "pressure": 1026,
                "sea_level": 1026,
                "grnd_level": 940,
                "humidity": 63,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 803,
                    "main": "Clouds",
                    "description": "broken clouds",
                    "icon": "04d"
                }
            ],
            "clouds": {
                "all": 70
            },
            "wind": {
                "speed": 0.21,
                "deg": 199,
                "gust": 0.52
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "d"
            },
            "dt_txt": "2024-11-26 06:00:00"
        },
        {
            "dt": 1732611600,
            "main": {
                "temp": 4.96,
                "feels_like": 4.96,
                "temp_min": 4.96,
                "temp_max": 4.96,
                "pressure": 1025,
                "sea_level": 1025,
                "grnd_level": 940,
                "humidity": 49,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 800,
                    "main": "Clear",
                    "description": "clear sky",
                    "icon": "01d"
                }
            ],
            "clouds": {
                "all": 0
            },
            "wind": {
                "speed": 1,
                "deg": 288,
                "gust": 1.57
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "d"
            },
            "dt_txt": "2024-11-26 09:00:00"
        },
        {
            "dt": 1732622400,
            "main": {
                "temp": 7.89,
                "feels_like": 7.05,
                "temp_min": 7.89,
                "temp_max": 7.89,
                "pressure": 1022,
                "sea_level": 1022,
                "grnd_level": 938,
                "humidity": 46,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 800,
                    "main": "Clear",
                    "description": "clear sky",
                    "icon": "01d"
                }
            ],
            "clouds": {
                "all": 0
            },
            "wind": {
                "speed": 1.68,
                "deg": 292,
                "gust": 2.73
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "d"
            },
            "dt_txt": "2024-11-26 12:00:00"
        },
        {
            "dt": 1732633200,
            "main": {
                "temp": 4.51,
                "feels_like": 3.58,
                "temp_min": 4.51,
                "temp_max": 4.51,
                "pressure": 1023,
                "sea_level": 1023,
                "grnd_level": 939,
                "humidity": 64,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 800,
                    "main": "Clear",
                    "description": "clear sky",
                    "icon": "01n"
                }
            ],
            "clouds": {
                "all": 0
            },
            "wind": {
                "speed": 1.36,
                "deg": 283,
                "gust": 1.43
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "n"
            },
            "dt_txt": "2024-11-26 15:00:00"
        },
        {
            "dt": 1732644000,
            "main": {
                "temp": 5.27,
                "feels_like": 5.27,
                "temp_min": 5.27,
                "temp_max": 5.27,
                "pressure": 1024,
                "sea_level": 1024,
                "grnd_level": 940,
                "humidity": 63,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 802,
                    "main": "Clouds",
                    "description": "scattered clouds",
                    "icon": "03n"
                }
            ],
            "clouds": {
                "all": 45
            },
            "wind": {
                "speed": 1.33,
                "deg": 281,
                "gust": 1.68
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "n"
            },
            "dt_txt": "2024-11-26 18:00:00"
        },
        {
            "dt": 1732654800,
            "main": {
                "temp": 5.45,
                "feels_like": 3.96,
                "temp_min": 5.45,
                "temp_max": 5.45,
                "pressure": 1024,
                "sea_level": 1024,
                "grnd_level": 940,
                "humidity": 65,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 804,
                    "main": "Clouds",
                    "description": "overcast clouds",
                    "icon": "04n"
                }
            ],
            "clouds": {
                "all": 99
            },
            "wind": {
                "speed": 1.93,
                "deg": 281,
                "gust": 3.49
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "n"
            },
            "dt_txt": "2024-11-26 21:00:00"
        },
        {
            "dt": 1732665600,
            "main": {
                "temp": 4.16,
                "feels_like": 1.95,
                "temp_min": 4.16,
                "temp_max": 4.16,
                "pressure": 1025,
                "sea_level": 1025,
                "grnd_level": 940,
                "humidity": 70,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 803,
                    "main": "Clouds",
                    "description": "broken clouds",
                    "icon": "04n"
                }
            ],
            "clouds": {
                "all": 80
            },
            "wind": {
                "speed": 2.45,
                "deg": 299,
                "gust": 4.81
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "n"
            },
            "dt_txt": "2024-11-27 00:00:00"
        },
        {
            "dt": 1732676400,
            "main": {
                "temp": 3.82,
                "feels_like": 2.4,
                "temp_min": 3.82,
                "temp_max": 3.82,
                "pressure": 1026,
                "sea_level": 1026,
                "grnd_level": 941,
                "humidity": 82,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 803,
                    "main": "Clouds",
                    "description": "broken clouds",
                    "icon": "04n"
                }
            ],
            "clouds": {
                "all": 71
            },
            "wind": {
                "speed": 1.65,
                "deg": 291,
                "gust": 2.36
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "n"
            },
            "dt_txt": "2024-11-27 03:00:00"
        },
        {
            "dt": 1732687200,
            "main": {
                "temp": 3.57,
                "feels_like": 3.57,
                "temp_min": 3.57,
                "temp_max": 3.57,
                "pressure": 1027,
                "sea_level": 1027,
                "grnd_level": 942,
                "humidity": 88,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 803,
                    "main": "Clouds",
                    "description": "broken clouds",
                    "icon": "04d"
                }
            ],
            "clouds": {
                "all": 78
            },
            "wind": {
                "speed": 1.31,
                "deg": 307,
                "gust": 1.88
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "d"
            },
            "dt_txt": "2024-11-27 06:00:00"
        },
        {
            "dt": 1732698000,
            "main": {
                "temp": 6.74,
                "feels_like": 5.94,
                "temp_min": 6.74,
                "temp_max": 6.74,
                "pressure": 1027,
                "sea_level": 1027,
                "grnd_level": 943,
                "humidity": 75,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 804,
                    "main": "Clouds",
                    "description": "overcast clouds",
                    "icon": "04d"
                }
            ],
            "clouds": {
                "all": 96
            },
            "wind": {
                "speed": 1.49,
                "deg": 308,
                "gust": 2.4
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "d"
            },
            "dt_txt": "2024-11-27 09:00:00"
        },
        {
            "dt": 1732708800,
            "main": {
                "temp": 8.87,
                "feels_like": 8.87,
                "temp_min": 8.87,
                "temp_max": 8.87,
                "pressure": 1025,
                "sea_level": 1025,
                "grnd_level": 941,
                "humidity": 66,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 803,
                    "main": "Clouds",
                    "description": "broken clouds",
                    "icon": "04d"
                }
            ],
            "clouds": {
                "all": 63
            },
            "wind": {
                "speed": 1.26,
                "deg": 307,
                "gust": 1.52
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "d"
            },
            "dt_txt": "2024-11-27 12:00:00"
        },
        {
            "dt": 1732719600,
            "main": {
                "temp": 6.29,
                "feels_like": 6.29,
                "temp_min": 6.29,
                "temp_max": 6.29,
                "pressure": 1026,
                "sea_level": 1026,
                "grnd_level": 942,
                "humidity": 78,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 804,
                    "main": "Clouds",
                    "description": "overcast clouds",
                    "icon": "04n"
                }
            ],
            "clouds": {
                "all": 87
            },
            "wind": {
                "speed": 0.97,
                "deg": 320,
                "gust": 1
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "n"
            },
            "dt_txt": "2024-11-27 15:00:00"
        },
        {
            "dt": 1732730400,
            "main": {
                "temp": 4.66,
                "feels_like": 4.66,
                "temp_min": 4.66,
                "temp_max": 4.66,
                "pressure": 1028,
                "sea_level": 1028,
                "grnd_level": 943,
                "humidity": 86,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 804,
                    "main": "Clouds",
                    "description": "overcast clouds",
                    "icon": "04n"
                }
            ],
            "clouds": {
                "all": 86
            },
            "wind": {
                "speed": 0.13,
                "deg": 119,
                "gust": 0.55
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "n"
            },
            "dt_txt": "2024-11-27 18:00:00"
        },
        {
            "dt": 1732741200,
            "main": {
                "temp": 3.72,
                "feels_like": 3.72,
                "temp_min": 3.72,
                "temp_max": 3.72,
                "pressure": 1028,
                "sea_level": 1028,
                "grnd_level": 943,
                "humidity": 89,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 803,
                    "main": "Clouds",
                    "description": "broken clouds",
                    "icon": "04n"
                }
            ],
            "clouds": {
                "all": 64
            },
            "wind": {
                "speed": 0.46,
                "deg": 113,
                "gust": 0.57
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "n"
            },
            "dt_txt": "2024-11-27 21:00:00"
        },
        {
            "dt": 1732752000,
            "main": {
                "temp": 3.43,
                "feels_like": 3.43,
                "temp_min": 3.43,
                "temp_max": 3.43,
                "pressure": 1028,
                "sea_level": 1028,
                "grnd_level": 943,
                "humidity": 89,
                "temp_kf": 0
            },
            "weather": [
                {
                    "id": 803,
                    "main": "Clouds",
                    "description": "broken clouds",
                    "icon": "04n"
                }
            ],
            "clouds": {
                "all": 82
            },
            "wind": {
                "speed": 0.65,
                "deg": 111,
                "gust": 0.75
            },
            "visibility": 10000,
            "pop": 0,
            "sys": {
                "pod": "n"
            },
            "dt_txt": "2024-11-28 00:00:00"
        }
    ],
    "city": {
        "id": 729968,
        "name": "Krasno Selo",
        "coord": {
            "lat": 42.69,
            "lon": 23.32
        },
        "country": "BG",
        "population": 0,
        "timezone": 7200,
        "sunrise": 1732339716,
        "sunset": 1732373890
    }
}


)--"

};