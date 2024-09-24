#pragma once

#include <string>
#include <memory>
#include <filesystem>

struct Image
{
  std::unique_ptr<unsigned char[]> img;
  int size = 0;   // raw size in bytes
  int pixels = 0; // pixel count
  int x = 0, y = 0, n = 0;

  ~Image() = default;

  Image(const Image &other);
  Image(Image &&other) noexcept;
  Image &operator=(const Image &other);
  Image &operator=(Image &&other);

  Image();
  Image(int x, int y);

  Image(unsigned char *img, int x, int y, int n);
  bool allocate();
  Image(unsigned char *buffer, size_t size);
  Image(std::string file);
  void resize(int x, int y);
  bool writeJpg(std::filesystem::path path);
  bool writePng(std::filesystem::path path);
  void applyAlpha(float alpha);

  std::string hash();

  static void applyAlpha(Image& image, float alpha);
  static void merge(Image &frame, Image &character, Image &background, Image &output);
};