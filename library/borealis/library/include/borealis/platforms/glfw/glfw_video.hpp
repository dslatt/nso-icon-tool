/*
    Copyright 2021 natinusala

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#pragma once

#include <borealis/core/video.hpp>

#ifdef __PSV__
#define GLFW_INCLUDE_ES2
#else
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

namespace brls
{

// GLFW Video Context
class GLFWVideoContext : public VideoContext
{
  public:
    GLFWVideoContext(const std::string& windowTitle, uint32_t windowWidth, uint32_t windowHeight, float windowX = NAN, float windowY = NAN);
    ~GLFWVideoContext() override;

    NVGcontext* getNVGContext() override;

    void clear(NVGcolor color) override;
    void beginFrame() override;
    void endFrame() override;
    void resetState() override;
    double getScaleFactor() override;
    void fullScreen(bool fs) override;
    int getCurrentMonitorIndex() override;

    GLFWwindow* getGLFWWindow();

  private:
    GLFWwindow* window     = nullptr;
    NVGcontext* nvgContext = nullptr;

#ifdef __SWITCH__
    int oldWidth, oldHeight;
    GLFWmonitor* monitor = nullptr;
#endif
};

} // namespace brls
