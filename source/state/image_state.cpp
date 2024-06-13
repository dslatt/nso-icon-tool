#include "state/image_state.hpp"

#define RED(px) (uint8_t)(px)
#define GREEN(px) (uint8_t)(px >> 8)
#define BLUE(px) (uint8_t)(px >> 16)
#define ALPHA(px) (uint8_t)(px >> 24)
#define RGBA(r, g, b, a) ((r & 0xff) | ((g & 0xff) << 8) | ((b & 0xff) << 16) | ((a & 0xff) << 24))
#define BLEND(a, b, alpha) (((a * alpha) + (b * (255 - alpha))) / 255)

Image empty(256, 256);

// assumes images same size, little endian, RGBA channels
void overlayImg(Image &frame, Image &character, Image &background, Image &output)
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

ImageState::ImageState() : frame(256, 256),
                           character(256, 256),
                           background(256, 256),
                           working(256, 256)
{
}

void ImageState::merge()
{
  overlayImg(frame, character, background, working);
}

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