#include "util/image.hpp"

#include <utility>
#include <cstring>
#include <algorithm>
#include <fmt/format.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include "extern/nanovg/stb_image.h"
#include "extern/stb_image_write.h"
#include "extern/stb_image_resize2.h"

#include <xxhash.h>

Image::~Image()
{
  if (img)
  {
    free(img);
  }
  img = nullptr;
}

Image::Image(const Image &other)
{
  unsigned char *data = (unsigned char*)malloc(other.size);
  if (data)
  {
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
  if (img)
    free(img);
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
  if (img)
    free(img);
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
  if (img != nullptr)
  {
    free(img);
  }

  img = (unsigned char *)calloc(size / sizeof(char), sizeof(char));

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

bool Image::writeJpg(std::string path)
{
  return stbi_write_jpg(path.c_str(), x, y, 4, img, 90) != 0;
}

bool Image::writePng(std::string path)
{
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

#define RED(px) (uint8_t)(px)
#define GREEN(px) (uint8_t)(px >> 8)
#define BLUE(px) (uint8_t)(px >> 16)
#define ALPHA(px) (uint8_t)(px >> 24)
#define RGBA(r, g, b, a) ((r & 0xff) | ((g & 0xff) << 8) | ((b & 0xff) << 16) | ((a & 0xff) << 24))
#define BLEND(a, b, alpha) (((a * alpha) + (b * (255 - alpha))) / 255)

// assumes images same size, little endian, RGBA channels
void Image::merge(Image &frame, Image &character, Image &background, Image &output)
{
  auto total = frame.x * frame.y;
  auto frameRef = (int *)frame.img;
  auto characterRef = (int *)character.img;
  auto backgroundRef = (int *)background.img;
  auto outputRef = (int *)output.img;

  for (auto i = 0; i < total; i++)
  {
    auto frameAlpha = ALPHA(*frameRef);
    auto characterAlpha = ALPHA(*characterRef);
    auto backgroundAlpha = ALPHA(*backgroundRef);

    // frame blocking
    if (frameAlpha == 0xff)
    {
      *outputRef = *frameRef;
      // character blocking
    }
    else if (frameAlpha == 0 && characterAlpha == 0xff)
    {
      *outputRef = *characterRef;
      // background only
    }
    else if (frameAlpha == 0 && characterAlpha == 0)
    {
      *outputRef = *backgroundRef;
    }
    // blend
    else
    {
      *outputRef = RGBA(BLEND(RED(*characterRef), RED(*backgroundRef), characterAlpha),
                        BLEND(GREEN(*characterRef), GREEN(*backgroundRef), characterAlpha),
                        BLEND(BLUE(*characterRef), BLUE(*backgroundRef), characterAlpha),
                        0xff);

      *outputRef = RGBA(BLEND(RED(*frameRef), RED(*outputRef), frameAlpha),
                        BLEND(GREEN(*frameRef), GREEN(*outputRef), frameAlpha),
                        BLEND(BLUE(*frameRef), BLUE(*outputRef), frameAlpha),
                        0xff);
    }

    frameRef++;
    characterRef++;
    outputRef++;
    backgroundRef++;
  }
}

void Image::applyAlpha(Image& image, float alpha) {
  auto total = image.x * image.y;
  auto ref = (int *)image.img;

  for (auto i = 0; i < total; i++) {
    *ref++ = RGBA(RED(*ref), GREEN(*ref), BLUE(*ref), uint8_t(((float)ALPHA(*ref)) * alpha));
  }
}