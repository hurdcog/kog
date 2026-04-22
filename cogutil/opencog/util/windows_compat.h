/**
 * windows_compat.h
 *
 * Windows compatibility layer for OpenCog CogUtil
 * Provides POSIX-like functionality on Windows/MSVC
 *
 * Copyright (C) 2025 OpenCog Foundation
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef _OPENCOG_WINDOWS_COMPAT_H
#define _OPENCOG_WINDOWS_COMPAT_H

#ifdef _WIN32

// ============================================================================
// IMPORTANT: Include Order Matters!
// winsock2.h MUST be included before windows.h to avoid conflicts
// ============================================================================

// Include winsock2.h first - it defines struct timeval
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

// Link with Winsock library
#pragma comment(lib, "ws2_32.lib")

// ============================================================================
// Windows Macro Conflicts - MUST be handled before including other headers
// ============================================================================

// Windows headers define ERROR as a macro (value 0) which conflicts with
// our Logger::Level::ERROR enum. We save and undefine it here.
#ifdef ERROR
#define _WINDOWS_ERROR_MACRO ERROR
#undef ERROR
#endif

// Windows headers may also define these
#ifdef WARN
#undef WARN
#endif

#ifdef INFO
#undef INFO
#endif

#ifdef DEBUG
#undef DEBUG
#endif

// ============================================================================
// Math Constants
// ============================================================================
// Define _USE_MATH_DEFINES before including math headers to get M_PI, etc.
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <cmath>

// Ensure M_PI is defined even if _USE_MATH_DEFINES didn't work
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.71828182845904523536
#endif

#ifndef M_LOG2E
#define M_LOG2E 1.44269504088896340736
#endif

#ifndef M_LN2
#define M_LN2 0.693147180559945309417
#endif

#ifndef M_LN10
#define M_LN10 2.30258509299404568402
#endif

#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880
#endif

// ============================================================================
// POSIX Function Replacements
// ============================================================================

// getcwd() → _getcwd()
#include <direct.h>
#ifndef getcwd
#define getcwd _getcwd
#endif

// chdir() → _chdir()
#ifndef chdir
#define chdir _chdir
#endif

// mkdir() → _mkdir()
#include <io.h>
#ifndef mkdir
#define mkdir(path, mode) _mkdir(path)
#endif

// access() → _access()
#ifndef access
#define access _access
#endif

// unlink() → _unlink()
#ifndef unlink
#define unlink _unlink
#endif

// rmdir() → _rmdir()
#ifndef rmdir
#define rmdir _rmdir
#endif

// fileno() → _fileno()
#ifndef fileno
#define fileno _fileno
#endif

// isatty() → _isatty()
#ifndef isatty
#define isatty _isatty
#endif

// ============================================================================
// POSIX Constants
// ============================================================================

// File access modes for access()
#ifndef F_OK
#define F_OK 0  // File exists
#endif

#ifndef R_OK
#define R_OK 4  // Read permission
#endif

#ifndef W_OK
#define W_OK 2  // Write permission
#endif

#ifndef X_OK
#define X_OK 1  // Execute permission (not supported on Windows)
#endif

// ============================================================================
// Time Functions
// ============================================================================

#include <time.h>
#include <sys/timeb.h>

// gettimeofday() replacement for Windows
// Note: struct timeval is now defined via winsock2.h
#ifndef HAVE_GETTIMEOFDAY
struct timezone {
    int tz_minuteswest;
    int tz_dsttime;
};

static __forceinline int gettimeofday(struct timeval* tv, struct timezone* tz) {
    if (tv) {
        FILETIME ft;
        GetSystemTimeAsFileTime(&ft);
        
        // Convert FILETIME to Unix epoch time
        unsigned __int64 tmpres = 0;
        tmpres |= ft.dwHighDateTime;
        tmpres <<= 32;
        tmpres |= ft.dwLowDateTime;
        
        // FILETIME is in 100-nanosecond intervals since Jan 1, 1601
        // Unix epoch is Jan 1, 1970
        tmpres -= 116444736000000000ULL;
        tmpres /= 10;  // Convert to microseconds
        
        tv->tv_sec = (long)(tmpres / 1000000UL);
        tv->tv_usec = (long)(tmpres % 1000000UL);
    }
    
    if (tz) {
        _tzset();
        long timezone_seconds;
        _get_timezone(&timezone_seconds);
        tz->tz_minuteswest = timezone_seconds / 60;
        
        int daylight;
        _get_daylight(&daylight);
        tz->tz_dsttime = daylight;
    }
    
    return 0;
}
#endif // HAVE_GETTIMEOFDAY

// ============================================================================
// POSIX Function Macros - Use macros to guarantee visibility in all TUs
// ============================================================================

// usleep() - sleep for microseconds
// Windows Sleep() takes milliseconds, so convert
// Using a macro ensures the function is always resolved at compile time
static __forceinline void _oc_usleep_impl(unsigned int usec) {
    Sleep((usec + 999) / 1000);  // Round up to avoid zero sleep
}
#ifndef usleep
#define usleep(usec) _oc_usleep_impl(usec)
#endif

// gmtime_r() - thread-safe version of gmtime
// Windows has gmtime_s with reversed parameter order
static __forceinline struct tm* _oc_gmtime_r_impl(const time_t* timer, struct tm* buf) {
    if (gmtime_s(buf, timer) == 0) {
        return buf;
    }
    return nullptr;
}
#ifndef gmtime_r
#define gmtime_r(timer, buf) _oc_gmtime_r_impl(timer, buf)
#endif

// fdatasync() - sync file data to disk (not metadata)
// Windows doesn't distinguish between data and metadata sync
// Use _commit() which is similar to fsync()
#include <io.h>
static __forceinline int _oc_fdatasync_impl(int fd) {
    return _commit(fd);
}
#ifndef fdatasync
#define fdatasync(fd) _oc_fdatasync_impl(fd)
#endif

// ============================================================================
// String Functions
// ============================================================================

// Disable deprecated warnings for standard C functions
#pragma warning(disable: 4996)

// Remove any snprintf macro definitions that conflict with C++11
#ifdef snprintf
#undef snprintf
#endif

// Use standard library snprintf
#include <cstdio>

// ============================================================================
// Threading Functions
// ============================================================================

#include <process.h>

// getpid() → _getpid()
#ifndef getpid
#define getpid _getpid
#endif

// ============================================================================
// DLL Export/Import
// ============================================================================

#ifdef COGUTIL_EXPORTS
#define COGUTIL_API __declspec(dllexport)
#else
#define COGUTIL_API __declspec(dllimport)
#endif

// ============================================================================
// Compiler-Specific Fixes
// ============================================================================

// MSVC doesn't support __attribute__
#ifndef __attribute__
#define __attribute__(x)
#endif

// MSVC doesn't support __builtin_expect
#ifndef __builtin_expect
#define __builtin_expect(expr, val) (expr)
#endif

// ============================================================================
// Network Functions Helper
// ============================================================================

// Initialize Winsock (call once at program start)
inline int init_winsock() {
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData);
}

// Cleanup Winsock (call once at program end)
inline void cleanup_winsock() {
    WSACleanup();
}

#endif // _WIN32

#endif // _OPENCOG_WINDOWS_COMPAT_H
