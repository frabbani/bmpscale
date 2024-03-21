#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "defs.h"

#define BI_RGB        0
#define BI_RLE8       1
#define BI_RLE4       2
#define BI_BITFIELDS  3
#define BI_JPEG       4
#define BI_PNG        5

#pragma pack(push, 1)

typedef struct {
  uint8 num0, num1;
}bmp_file_magic_t;      //2 bytes

typedef struct {
  uint32 filesize;
  uint16 creators[2];
  uint32 dataoffset;
}bmp_file_header_t;     //12 bytes

typedef struct {
  uint32 headersize;
  int32 width, height;
  uint16 numplanes, bitsperpixel;
  uint32 compression;
  uint32 datasize;
  int32 hpixelsper, vpixelsper;  //horizontal and vertical pixels-per-meter
  uint32 numpalcolors, numimpcolors;

}bmp_dib_header_t;   //40 bytes, all windows versions since 3.0

#pragma pack(pop)

void bmp_read(color_t **pixels, int *w, int *h, const void *data);
void bmp_write(const color_t *pixels, int w, int h, const char name[]);
void bmp_write_palette(uint8 *values, int w, int h, const color_t *palette, const char name[]);

#ifdef __cplusplus
}
#endif

