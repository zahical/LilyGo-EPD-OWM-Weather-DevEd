/*
 * Copyright (c) 2024 zahical. All rights reserved.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/**
 * @file Common utility types.
 */
#pragma once
#include <Arduino.h>
#include <utility>

/**
 * @brief A really simple std::unexpected<E> like type (the C++23 meaning) to be used with OpResult<T> below.
 *
 * Represents a failed operation, with an error message.
 */
class op_failed
{
  public:
    op_failed(String const& s) : _err(s) {}

    op_failed(String&& s) : _err(std::move(s)) {}

  private:
    String _err;
    template <typename>
    friend class OpResult;
};

/**
 * @brief A really simple C++23 std::expected<T,E> like type with E hardcoded to String
 *
 * This type is used to propagate either a success result or an error out of a function without using exceptions.
 *
 * @tparam T The type of the return result
 */
template <typename T>
class OpResult
{

  public:
    OpResult(op_failed const& r) : _success(false), _err(r._err) {}

    OpResult(op_failed&& r) : _success(false), _err(std::move(r._err)) {}

    OpResult(T const& r) : _val(r), _success(true) {}

    OpResult(T&& r) : _val(std::move(r)), _success(true) {}

    // SafeBool for C++11
    explicit operator bool() const { return _success; }

    T& operator*() & { return _val; }

    T&& operator*() && { return std::move(_val); }

    String const& error() const { return _err; }

  private:
    bool _success;
    String _err;
    T _val;
};

/**
 * @brief Specialization of OpResult for void.
 *
 * As in that case there is no result, there are no operator*() members to access it.
 */
template <>
class OpResult<void>
{

  public:
    OpResult(op_failed const& r) : _success(false), _err(r._err) {}

    OpResult(op_failed&& r) : _success(false), _err(std::move(r._err)) {}

    OpResult() : _success(true) {}

    // SafeBool for C++11
    explicit operator bool() const { return _success; }

    String const& error() const { return _err; }

  private:
    bool _success;
    String _err;
};

#define OPT_LOG(b, ...)         \
    {                           \
        if (b) { __VA_ARGS__; } \
    }
