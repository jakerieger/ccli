#ifndef COMMON_H
#define COMMON_H

#define _GNU_SOURCE 1  // Enable POSIX 2008.1 extensions

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;
#if defined(__GNUC__) || defined(__clang__)
typedef __int128_t i128;
typedef __uint128_t u128;
#endif
typedef float f32;
typedef double f64;
typedef u8 byte;

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0])
#define STR_ARRAY(...)                                                                                                 \
    (char*[]) {                                                                                                        \
        __VA_ARGS__                                                                                                    \
    }

inline static bool path_exists(const char* path) {
    struct stat st;
    return stat(path, &st) == 0;
}

inline static bool path_is_dir(const char* path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        return false;
    }
    return S_ISDIR(st.st_mode);
}

inline static bool path_is_file(const char* path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        return false;
    }
    return S_ISREG(st.st_mode);
}

#endif
