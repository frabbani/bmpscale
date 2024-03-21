#include "defs.h"

color_t color( uint8 r, uint8 g, uint8 b ){
  color_t c;
  c.r = r;
  c.g = g;
  c.b = b;
  return c;
}

color_t color_lerp( color_t c0, color_t c1, float alpha ){
  color_t c;
  alpha = alpha < 0.0f ? 0.0f : alpha > 1.0f ? 1.0f : alpha;

  float r = (float)c0.r + alpha * (float)(c1.r - c0.r );
  float g = (float)c0.g + alpha * (float)(c1.g - c0.g );
  float b = (float)c0.b + alpha * (float)(c1.b - c0.b );
  c.r = (uint8)r;
  c.g = (uint8)g;
  c.b = (uint8)b;
  return c;
}

void write_ppm( const color_t *pixels, int32 w, int32 h, const char name[] ){
  char file[128];
  sprintf( file, "%s.ppm", name );
  FILE *fp = fopen( file, "wb" );
  fprintf( fp, "P6\n" );
  fprintf( fp, "%d %d 255\n", w, h );
  fwrite ( pixels, 3, w * h, fp );
  fclose( fp );
}

