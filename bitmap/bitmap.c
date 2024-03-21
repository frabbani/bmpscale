#include "bitmap.h"

#define BI_RGB        0
#define BI_RLE8       1
#define BI_RLE4       2
#define BI_BITFIELDS  3
#define BI_JPEG       4
#define BI_PNG        5

void bmp_read(color_t **pixels, int *w, int *h, const void *data) {
  if (!pixels || !w || !h)
    return;
  *pixels = NULL;
  *w = *h = 0;

  const uint8_t *stream = data;
  uint32_t pos = 0;

  const bmp_file_magic_t *magic = (const void*) &stream[pos];
  if (magic->num0 != 'B' || magic->num1 != 'M')
    return;
  pos += sizeof(bmp_file_magic_t);

  const bmp_file_header_t *fileheader = (const void*) &stream[pos];
  pos += sizeof(bmp_file_header_t);

  const bmp_dib_header_t *dibheader = (const void*) &stream[pos];
  if (dibheader->compression != BI_RGB || dibheader->bitsperpixel != 24)
    return;
  *w = dibheader->width;
  *h = dibheader->height;
  *pixels = malloc(*w * *h * sizeof(color_t));

  uint16_t bypp = dibheader->bitsperpixel / 8;
  uint16_t rem = 0;
  if ((*w * bypp) & 0x03)
    rem = 4 - ((*w * bypp) & 0x03);

  pos = fileheader->dataoffset;
  for (int y = 0; y < *h; y++) {
    for (int x = 0; x < *w; x++) {
      (*pixels)[y * *w + x].b = stream[pos++];
      (*pixels)[y * *w + x].g = stream[pos++];
      (*pixels)[y * *w + x].r = stream[pos++];
    }
    pos += rem;
  }
}

void bmp_write(const color_t *pixels, int w, int h, const char name[]) {
  char file[128];

  strcpy(file, name);
  int ext = 0;
  for (const char *p = name; *p != '\0'; p++) {
    if ('.' == p[0] && 'b' == p[1] && 'm' == p[2] && 'p' == p[3] && '\0' == p[4]) {
      ext++;
      break;
    }
  }
  if (!ext)
    strcat(file, ".bmp");

  FILE *fp = fopen(file, "wb");

  bmp_file_magic_t magic;
  magic.num0 = 'B';
  magic.num1 = 'M';
  fwrite(&magic, 2, 1, fp);  //hard coding 2 bytes, (our structure isn't packed).

  bmp_file_header_t fileheader;
  fileheader.filesize = w * h * sizeof(int) + 54;

  fileheader.creators[0] = 0;
  fileheader.creators[1] = 0;
  fileheader.dataoffset = 54;
  fwrite(&fileheader, sizeof(bmp_file_header_t), 1, fp);

  bmp_dib_header_t dibheader;
  dibheader.headersize = 40;
  dibheader.width = w;
  dibheader.height = h;
  dibheader.numplanes = 1;
  dibheader.bitsperpixel = 24;
  dibheader.compression = BI_RGB;
  dibheader.datasize = 0;
  dibheader.hpixelsper = dibheader.vpixelsper = 1000;
  dibheader.numpalcolors = dibheader.numimpcolors = 0;
  fwrite(&dibheader, sizeof(bmp_dib_header_t), 1, fp);

  int rem = 0;
  if ((w * 3) & 0x03)
    rem = 4 - ((w * 3) & 0x03);

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      fputc(pixels[y * w + x].b, fp);
      fputc(pixels[y * w + x].g, fp);
      fputc(pixels[y * w + x].r, fp);
    }
    for (int i = 0; i < rem; i++)
      fputc(0xff, fp);
  }

  fclose(fp);
}

void bmp_write_palette(uint8_t *values, int w, int h, const color_t *palette, const char name[]) {
  char file[128];

  strcpy(file, name);
  int ext = 0;
  for (const char *p = name; *p != '\0'; p++) {
    if ('.' == p[0] && 'b' == p[1] && 'm' == p[2] && 'p' == p[3] && '\0' == p[4]) {
      ext++;
      break;
    }
  }
  if (!ext)
    strcat(file, ".bmp");

  FILE *fp = fopen(file, "wb");

  bmp_file_magic_t magic;
  magic.num0 = 'B';
  magic.num1 = 'M';
  fwrite(&magic, 2, 1, fp);  //hard coding 2 bytes, (our structure isn't packed).

  bmp_file_header_t fileheader;
  fileheader.filesize = w * h * sizeof(uint8) + 54 + 256 * 4;

  fileheader.creators[0] = 0;
  fileheader.creators[1] = 0;
  fileheader.dataoffset = 54 + 256 * 4;
  fwrite(&fileheader, sizeof(bmp_file_header_t), 1, fp);

  bmp_dib_header_t dibheader;
  dibheader.headersize = 40;
  dibheader.width = w;
  dibheader.height = h;
  dibheader.numplanes = 1;
  dibheader.bitsperpixel = 8;
  dibheader.compression = BI_RGB;
  dibheader.datasize = 0;
  dibheader.hpixelsper = dibheader.vpixelsper = 1000;
  dibheader.numpalcolors = dibheader.numimpcolors = 256;
  fwrite(&dibheader, sizeof(bmp_dib_header_t), 1, fp);
  for (int i = 0; i < 256; i++) {
    fputc(palette[i].r, fp);
    fputc(palette[i].g, fp);
    fputc(palette[i].b, fp);
    fputc(0, fp);

  }

  int32 rem = 4 - (w & 0x03);
  for (int32 y = 0; y < h; y++) {
    fwrite(&values[y * w], 1, w, fp);
    if (rem < 4) {
      for (int i = 0; i < rem; i++)
        fputc(0xff, fp);
    }
  }

  fclose(fp);
}
