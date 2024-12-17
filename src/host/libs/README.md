# Folder content

Subfolders here contain libraries that are only used by the on-host emulator build.

These are provided as in-tree copy, because PIO doesn't really expose a convenient way to use non-PIO libraries via lib_deps.

## [LilyGo-EPD47](epd_driver/)

**Abridged** and _slightly_ modified sources of the https://github.com/Xinyuan-LilyGO/LilyGo-EPD47/tree/v1.0.1 library that's also used when drawing on the actual hardware: tag `v1.0.1` commit SHA: `d35c008`. License: GPLv3

Specifically, only the drawing functions are available and any hardware-related functionality has been `#ifdef`ed out (search for `PORT_HACKS`).

Note that for the actual esp32 hardware builds, the LilyGo-EPD library is still used via the regular `lib_deps` mechanism in [platformio.ini](../../../platformio.ini)

## [RPNG](rpng/)

> rpng is a simple and easy-to-use library to load/save png images and manage png chunks.

A copy of https://github.com/raysan5/rpng/blob/1.5/src/rpng.h tag `1.5` commit SHA: `1de30ae`.

License: Zlib

## [cpp-httplib](httplib/)

> A C++11 single-file header-only cross platform HTTP/HTTPS library.

A copy of https://github.com/yhirose/cpp-httplib/blob/v0.18.1/httplib.h tag `v0.18.1` commit SHA: `5c1a34e`.

License: MIT
