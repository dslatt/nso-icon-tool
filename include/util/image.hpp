#pragma once

#include <string>

struct Image
{
  unsigned char *img = nullptr;
  int size = 0;   // raw size in bytes
  int pixels = 0; // pixel count
  int x = 0, y = 0, n = 0;

  ~Image();

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
  bool writeJpg(std::string path);
  bool writePng(std::string path);
  void applyAlpha(float alpha);

  static void multAlpha(Image& image, float alpha);
  static void merge(Image &frame, Image &character, Image &background, Image &output);
};