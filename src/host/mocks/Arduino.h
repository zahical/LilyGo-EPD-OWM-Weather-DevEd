/*
 * Copyright (c) 2024 zahical. All rights reserved.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/**
 * @file A minimal (and incomplete) mock of the standard Arduino environment.
 * Only the functionality used by the UI drawing code is implemented.
 */
#pragma once
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#define PI                        3.1415926535897932384626433832795
#define sq(x)                     ((x) * (x))
#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

typedef bool boolean;
typedef uint8_t byte;

using std::abs;

inline unsigned long millis()
{
    using namespace std::chrono;
    return time_point_cast<milliseconds>(high_resolution_clock::now()).time_since_epoch().count();
}

#define F(a) a

/**
 * @brief (incomplete) Arduino String over std::string (with some extensions)
 */
class String
{

  public:
    String(char const *cstr = "") { _s = cstr; }

    explicit String(unsigned char v, unsigned char base = 10) : _s(stringify((unsigned)v, base)) {}

    explicit String(int v, unsigned char base = 10) : _s(stringify(v, base)) {}

    explicit String(unsigned int v, unsigned char base = 10) : _s(stringify(v, base)) {}

    explicit String(long v, unsigned char base = 10) : _s(stringify(v, base)) {}

    explicit String(unsigned long v, unsigned char base = 10) : _s(stringify(v, base)) {}

    explicit String(long long v, unsigned char base = 10) : _s(stringify(v, base)) {}

    explicit String(unsigned long long v, unsigned char base = 10) : _s(stringify(v, base)) {}

    explicit String(float v, unsigned char decimalPlaces = 2) : _s(stringify_floating(v, decimalPlaces)) {}

    explicit String(double v, unsigned char decimalPlaces = 2) : _s(stringify_floating(v, decimalPlaces)) {}

    bool operator==(String const& rhs) const { return _s == rhs._s; }

    bool operator!=(String const& rhs) const { return _s != rhs._s; }

    String& operator=(char const *cstr)
    {
        _s = cstr;
        return *this;
    }

    unsigned int length(void) const { return _s.length(); }

    String& operator+=(String const& rhs)
    {
        _s += rhs._s;
        return (*this);
    }

    friend String operator+(String const& a, String const& b) { return String(a._s + b._s); }

    bool endsWith(String const& suffix) const
    {
        auto const suffix_start = _s.size() - suffix._s.size();
        auto const result       = _s.find(suffix._s, suffix_start);

        return (result == suffix_start) && (result != std::string::npos);
    }

    bool startsWith(String const& s2) const
    {
        if (_s.length() < s2._s.length()) return false;

        return _s.rfind(s2, 0) == 0;
    }

    String substring(unsigned int beginIndex) const { return substring(beginIndex, length()); }

    String substring(unsigned int left, unsigned int right) const
    {

        if (left > right)
        {
            unsigned int temp = right;
            right             = left;
            left              = temp;
        }

        if (left >= _s.length()) return {};
        if (right > _s.length()) right = _s.length();

        return _s.substr(left, right - left);
    }

    int indexOf(String const& s2) const
    {
        auto r   = _s.find(s2._s);
        return r = std::string::npos ? -1 : r;
    }

    void toLowerCase()
    {
        // well,... https://devblogs.microsoft.com/oldnewthing/20241007-00/?p=110345
        for (size_t i = 0; i < _s.length(); i++)
            _s[i] = std::tolower(_s[i]);
    }

    void toUpperCase()
    {
        for (size_t i = 0; i < _s.length(); i++)
            _s[i] = std::toupper(_s[i]);
    }

    void replace(String const& find, String const& replace)
    {
        size_t start_pos = 0;

        while ((start_pos = _s.find(find._s, start_pos)) != std::string::npos)
        {
            _s.replace(start_pos, find._s.length(), replace._s);
            start_pos += replace._s.length();
        }
    }

    char const *c_str() const { return _s.c_str(); }

  public:
    // extensions for interop with std::string
    String(std::string const& v) : _s(v){};
    String(std::string&& v) : _s(std::move(v)){};

    std::string const& s_str() const& { return _s; }

    std::string&& s_str() && { return std::move(_s); }

    operator std::string const &() const { return _s; }

  private:
    template <typename T>
    std::string stringify(T v, unsigned char base)
    {
        std::stringstream stream;
        stream << std::setbase(base) << v;
        return stream.str();
    }

    template <typename T>
    std::string stringify_floating(T v, unsigned char decimalPlaces)
    {
        std::stringstream stream;
        stream << std::fixed << std::setprecision(decimalPlaces) << v;
        return stream.str();
    }

    std::string _s;
};

class SerialMock
{
  public:
    void println(String const& s) { std::cout << s.s_str() << '\n'; };

    void print(String const& s) { std::cout << s.s_str(); };
};

extern SerialMock Serial;