#include "state/image_state.hpp"

Image empty(256, 256);

ImageState::ImageState()
    : frame(256, 256)
    , character(256, 256)
    , background(256, 256)
    , working(256, 256)
{
}

void ImageState::merge() { Image::merge(frame, character, background, working); }

void ImageState::resize()
{
  frame.resize(256, 256);
  character.resize(256, 256);
  background.resize(256, 256);
  working.resize(256, 256);
}

void ImageState::updateFrame(std::string path)
{
  frame = path.empty() ? empty : Image(path);
  resize();
  merge();
}

void ImageState::updateCharacter(std::string path)
{
  character = path.empty() ? empty : Image(path);
  resize();
  merge();
}

void ImageState::updateBackground(std::string path)
{
  background = path.empty() ? empty : Image(path);
  resize();
  merge();
}

void ImageState::updateWorking(std::string path)
{
  working = path.empty() ? empty : Image(path);
  resize();
}