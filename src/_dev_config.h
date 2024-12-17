/*
 * Copyright (c) 2024 zahical. All rights reserved.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/**
 * @file dev mode credentials.
 *
 * This file is conditionally included in config.cpp.
 * It must be renamed to dev_config.h to be used.
 *
 * Inadvertently submitting secrets to a public repo is an old problem. To alleviate it, the following approach is used.
 *
 * If you are doing active development on the project
 * (as opposed to simply trying to build it once, flash it and forget about it)
 * it's better to:
 * - set your api key, wifi password, etc. here (instead of config.cpp)
 * - RENAME the file to dev_config.h
 *
 * Once this is done, the values here will automatically be used (instead of the ones in config.cpp)
 * _however_ the file dev_config.h is excluded in .gitignore so it will be practically impossible
 * to add it to a commit by accident.
 */

// no pragma once here

constexpr char const WiFiSSID[]     = "";
constexpr char const WiFiPassword[] = "";
constexpr char const ApiKey[]       = "";
