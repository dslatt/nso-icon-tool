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

Image::~Image() {
  if (img) {
    free(img); // free() and stbi_image_free() are the same. be sure to only use malloc() or stbi* managed memory for img
    img = nullptr;
  }
}

Image::Image(const Image &other)
{
  auto* data = static_cast<unsigned char*>(malloc(other.size));
  if (data && other.img) {
    std::memcpy(data, other.img, other.size);
    img = data;
    size = other.size;
    pixels = other.pixels;
    x = other.x;
    y = other.y;
    n = other.n;
  }
}
Image::Image(Image &&other) noexcept
{
  if (img) { free(img); }
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
  if (img) { free(img); }
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
  if (img) { free(img); }
  img = static_cast<unsigned char*>(calloc(size / sizeof(char), sizeof(char)));

  return img != 0;
}

Image::Image(unsigned char *buffer, size_t size)
{
  this->img = stbi_load_from_memory(buffer, size, &x, &y, &n, 4);
  this->n = 4;
  this->pixels = x * y;
  this->size = pixels * 4 * sizeof(char);
}

Image::Image(std::string file)
{
  stbi_set_unpremultiply_on_load(1);
  stbi_convert_iphone_png_to_rgb(1);
  this->img = stbi_load(file.c_str(), &x, &y, &n, 4);
  this->pixels = x * y;
  this->size = pixels * 4 * sizeof(char);
}

Image::Image(unsigned char *img, int x, int y, int n)
{
  this->img = img;
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
    auto *resized = stbir_resize_uint8_linear(img, this->x, this->y, 0, nullptr, x, y, 0, stbir_pixel_layout::STBIR_RGBA);
    if (resized)
    {
      *this = Image(resized, x, y, 4);
    }
  }
}

bool Image::writeJpg(std::filesystem::path path)
{
  if (path.extension() != ".jpg") path.replace_extension(".jpg");
  return stbi_write_jpg(path.c_str(), x, y, 4, img, 90) != 0;
}

bool Image::writePng(std::filesystem::path path)
{
  if (path.extension() != ".png") path.replace_extension(".png");
  return stbi_write_png(path.c_str(), x, y, 4, img, 0) != 0;
}

void Image::applyAlpha(float alpha) {
  Image::applyAlpha(*this, std::clamp(alpha, 0.0f, 1.0f)) ;
}

std::string Image::hash()
{
  if (img)
  {
    auto xxh = XXH3_64bits(img, pixels * 4 * sizeof(char));
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

  std::span frameRef{reinterpret_cast<Pixel*>(frame.img), frame.size / sizeof(Pixel)};
  std::span characterRef{reinterpret_cast<Pixel*>(character.img), character.size / sizeof(Pixel)};
  std::span backgroundRef{reinterpret_cast<Pixel*>(background.img), background.size / sizeof(Pixel)};
  std::span outputRef{reinterpret_cast<Pixel*>(output.img), output.size / sizeof(Pixel)};

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
  auto ref = std::span(reinterpret_cast<Pixel*>(image.img), image.size / sizeof(Pixel));

  for (auto i = 0; i < total; i++) {
    ref[i] = Pixel{
      ref[i].r,
      ref[i].g,
      ref[i].b,
      static_cast<uint8_t>(static_cast<float>(ref[i].a) * alpha)
    };
  }
}