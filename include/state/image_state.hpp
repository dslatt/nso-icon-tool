#pragma once

#include "util/image.hpp"

class ImageState
{
public:
  ImageState();
  Image frame, character, background, working;
  void resize();
  void updateFrame(std::string path);
  void updateCharacter(std::string path);
  void updateBackground(std::string path);
  void updateWorking(std::string path);
  void merge();
};
