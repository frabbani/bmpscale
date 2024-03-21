#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>
#include <ctype.h>

#define SWAP( a, b ) \
{ \
  __typeof__(a) t = a; \
  a = b; \
  b = t; \
} \


typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

#pragma pack(push,1)
typedef struct {
  union {
    struct {
      uint8 b, g, r;
    };
    uint8 rgb[3];
  };
} color_t;
#pragma pack(pop)

color_t color(uint8 r, uint8 g, uint8 b);
color_t color_lerp(color_t c0, color_t c1, float alpha);

void write_ppm(const color_t *pixels, int32 w, int32 h, const char name[]);

