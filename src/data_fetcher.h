/*
 * Copyright (c) 2024 zahical. All rights reserved.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/**
 * @file Interface to the HTTP client.
 * All methods are safe to call in host emulator build, as well.
 */

#pragma once
#include "common.h"
#include <Arduino.h>

/**
 * @brief Fetches the specified `url` from the `server`.
 *
 * Whether HTTP or HTTPS is used, depends on the value of cfg::UseHTTPS.
 *
 * @param server host name only, no prefix, slashes, etc.
 * @param uri url only, should start with a forward slash
 * @return on success, the response body as a string; on error, the error response, as a string.
 */
OpResult<String> get_url(String const& server, String const& uri);

/**
 * @brief Closes any still opened HTTP connections.
 */
void reset_http();