#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <set>

#include "bitmap/bitmap.h"

int w, h;
color_t *pixels;

struct FileData {
  std::string name;
  std::vector<uint8> data;
  FileData() = default;
  FileData(std::string_view fileName) {
    FILE *fp = fopen(fileName.data(), "rb");
    if (!fp) {
      printf("FileData: '%s' is not a valid file\n", fileName.data());
      return;
    }
    fseek(fp, 0, SEEK_END);
    auto size = ftell(fp);
    if (!size) {
      printf("FileData: '%s' is empty\n", fileName.data());
      fclose(fp);
      return;
    }
    name = fileName;
    data.reserve(size);
    fseek(fp, 0, SEEK_SET);
    while (!feof(fp)) {
      data.push_back(fgetc(fp));
    }
    fclose(fp);
  }

  void* getData() {
    return data.data();
  }
};

#pragma pack(push,1)
struct Color : public color_t {
  Color() {
    r = g = b = 0;
  }

  bool operator ==(const Color &rhs) const {
    int dr = abs(rhs.r - r);
    int dg = abs(rhs.g - g);
    int db = abs(rhs.b - b);
    return dr <= 1 && dg <= 1 && db <= 1;
  }

  bool operator <(const Color &rhs) const {
    if (r == rhs.r) {
      if (g == rhs.g) {
        return b < rhs.b;
      }
      return g < rhs.g;
    }
    return r < rhs.r;
  }
  Color(color_t c) {
    *(static_cast<color_t*>(this)) = c;
  }
  Color(const Color &c) {
    r = c.r;
    g = c.g;
    b = c.b;
  }
};
#pragma pack(pop)

struct Point {
  int x = 0, y = 0;
  Point() = default;
  Point(const Point &rhs)
      :
      x(rhs.x),
      y(rhs.y) {
  }
  Point(int x_, int y_)
      :
      x(x_),
      y(y_) {
  }
  Point scale(int s) {
    x *= s;
    y *= s;
    return *this;
  }

  Point operator +(const Point &rhs) const {
    return Point(x + rhs.x, y + rhs.y);
  }
  Point operator -(const Point &rhs) const {
    return Point(x - rhs.x, y + rhs.y);
  }

  Point& operator =(const Point &rhs) {
    x = rhs.x;
    y = rhs.y;
    return *this;
  }
};

struct Pixels {
  Color *pixels = nullptr;
  int w = 0, h = 0;
  ~Pixels() {
    w = h = 0;
    delete[] pixels;
    pixels = nullptr;
  }
  Pixels(int w_, int h_)
      :
      w(w_),
      h(h_) {
    pixels = new Color[w * h];
  }

  Color& operator()(int x, int y) {
    x = x < 0 ? 0 : x >= w ? w - 1 : x;
    y = y < 0 ? 0 : y >= h ? h - 1 : y;
    return pixels[y * w + x];
  }
  Color& operator()(const Point &p) {
    return (*this)(p.x, p.y);
  }

};

int main(int argc, char *args[]) {
  setbuf( stdout, nullptr);
  printf("Hello...\n");

  FileData fd("i.bmp");
  bmp_read(&pixels, &w, &h, fd.getData());
  printf("w x h: %d x %d\n", w, h);
  printf("# of pixels: %d\n", w * h);

  Pixels input(w, h);
  for (int i = 0; i < w * h; i++)
    input.pixels[i] = pixels[i];

  Pixels output(w * 2, h * 2);

  // EPX scaling
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      Color c = input(x, y);
      Point o = Point(x, y).scale(2);
      Point ps[2][2];
      ps[0][0] = o;
      ps[0][1] = o + Point(1, 0);
      ps[1][0] = o + Point(0, 1);
      ps[1][1] = o + Point(1, 1);

      output(ps[0][0]) = c;
      output(ps[0][1]) = c;
      output(ps[1][0]) = c;
      output(ps[1][1]) = c;

      Color t = input(x, y + 1);
      Color b = input(x, y - 1);
      Color l = input(x - 1, y);
      Color r = input(x + 1, y);
      if (t == l)
        output(ps[1][0]) = t;
      if (t == r)
        output(ps[1][1]) = t;
      if (b == l)
        output(ps[0][0]) = b;
      if (b == r)
        output(ps[0][1]) = b;
    }
  }

  bmp_write(output.pixels, output.w, output.h, "o");
  printf("Goodbye!\n");
  return 0;
}
