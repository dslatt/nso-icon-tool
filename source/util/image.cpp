#include "util/image.hpp"

#include <utility>
#include <cstring>
#include <algorithm>
#include <fmt/format.h>
#include <span>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include "extern/nanovg/stb_image.h"
#include "extern/stb_image_write.h"
#include "extern/stb_image_resize2.h"

#include <xxhash.h>

Image::Image(const Image &other)
{
  if (other.img) {
    img = std::make_unique<unsigned char[]>(other.size);
    std::memcpy(img.get(), other.img.get(), other.size);
    size = other.size;
    pixels = other.pixels;
    x = other.x;
    y = other.y;
    n = other.n;
  }
}
Image::Image(Image &&other) noexcept
{
  img = std::exchange(other.img, nullptr);
  size = std::exchange(other.size, 0);
  x = std::exchange(other.x, 0);
  y = std::exchange(other.y, 0);
  n = std::exchange(other.n, 0);
}
Image &Image::operator=(const Image &other)
{
  return *this = Image(other);
}
Image &Image::operator=(Image &&other)
{
  img = std::exchange(other.img, nullptr);
  size = std::exchange(other.size, 0);
  x = std::exchange(other.x, 0);
  y = std::exchange(other.y, 0);
  n = std::exchange(other.n, 0);
  return *this;
}

Image::Image() : Image(0, 0, 0, 4) {}

Image::Image(int x, int y) : Image(nullptr, x, y, 4) { allocate(); }

bool Image::allocate()
{
  img = std::make_unique<unsigned char[]>(size);

  return img != 0;
}

Image::Image(unsigned char *buffer, size_t size)
{
  this->img = std::unique_ptr<unsigned char[]>(stbi_load_from_memory(buffer, size, &x, &y, &n, 4));
  this->n = 4;
  this->pixels = x * y;
  this->size = pixels * 4 * sizeof(char);
}

Image::Image(std::string file)
{
  stbi_set_unpremultiply_on_load(1);
  stbi_convert_iphone_png_to_rgb(1);
  this->img = std::unique_ptr<unsigned char[]>(stbi_load(file.c_str(), &x, &y, &n, 4));
  this->pixels = x * y;
  this->size = pixels * 4 * sizeof(char);
}

Image::Image(unsigned char *img, int x, int y, int n)
{
  this->img = std::unique_ptr<unsigned char[]>(img);
  this->x = x;
  this->y = y;
  this->n = n;
  this->pixels = x * y;
  this->size = pixels * 4 * sizeof(char);
}

void Image::resize(int x, int y)
{
  if (this->x != x || this->y != y)
  {
    auto *resized = stbir_resize_uint8_linear(img.get(), this->x, this->y, 0, nullptr, x, y, 0, stbir_pixel_layout::STBIR_RGBA);
    if (resized)
    {
      *this = Image(resized, x, y, 4);
    }
  }
}

bool Image::writeJpg(std::string path)
{
  return stbi_write_jpg(path.c_str(), x, y, 4, img.get(), 90) != 0;
}

bool Image::writePng(std::string path)
{
  return stbi_write_png(path.c_str(), x, y, 4, img.get(), 0) != 0;
}

void Image::applyAlpha(float alpha) {
  Image::applyAlpha(*this, std::clamp(alpha, 0.0f, 1.0f)) ;
}

std::string Image::hash()
{
  if (img)
  {
    auto xxh = XXH3_64bits(img.get(), pixels * 4 * sizeof(char));
    return fmt::format("{}", xxh);
  }

  return "";
}

#pragma pack(push, 1)
struct Pixel {
  uint8_t r, g, b, a;

  static Pixel blend(Pixel& a, Pixel& b) {

    auto blender = [](uint8_t a, uint8_t b, uint8_t alpha) {
      return static_cast<uint8_t>(((a * alpha) + (b * (255 - alpha))) / 255);
    };

    return Pixel {
      .r = blender(a.r, b.r, a.a),
      .g = blender(a.g, b.g, a.a),
      .b = blender(a.b, b.b, a.a),
      .a = 0xff
    };
  }
};
#pragma pack(pop)

// assumes images same size, little endian, RGBA channels
void Image::merge(Image &frame, Image &character, Image &background, Image &output)
{
  auto total = frame.x * frame.y;

  std::span frameRef{reinterpret_cast<Pixel*>(frame.img.get()), frame.size / sizeof(Pixel)};
  std::span characterRef{reinterpret_cast<Pixel*>(character.img.get()), character.size / sizeof(Pixel)};
  std::span backgroundRef{reinterpret_cast<Pixel*>(background.img.get()), background.size / sizeof(Pixel)};
  std::span outputRef{reinterpret_cast<Pixel*>(output.img.get()), output.size / sizeof(Pixel)};

  for (auto i = 0; i < total; i++)
  {
    // frame blocking
    if (frameRef[i].a == 0xff)
    {
      outputRef[i] = frameRef[i];
      // character blocking
    }
    else if (frameRef[i].a == 0 && characterRef[i].a == 0xff)
    {
      outputRef[i] = characterRef[i];
      // background only
    }
    else if (frameRef[i].a == 0 && characterRef[i].a == 0)
    {
      outputRef[i] = backgroundRef[i];
    }
    // blend
    else
    {
      outputRef[i] = Pixel::blend(characterRef[i], backgroundRef[i]);
      outputRef[i] = Pixel::blend(frameRef[i], outputRef[i]);
    }
  }
}

void Image::applyAlpha(Image& image, float alpha) {
  auto total = image.x * image.y;
  auto ref = std::span(reinterpret_cast<Pixel*>(image.img.get()), image.size / sizeof(Pixel));

  for (auto i = 0; i < total; i++) {
    ref[i] = Pixel{
      ref[i].r,
      ref[i].g,
      ref[i].b,
      static_cast<uint8_t>(static_cast<float>(ref[i].a) * alpha)
    };
  }
}