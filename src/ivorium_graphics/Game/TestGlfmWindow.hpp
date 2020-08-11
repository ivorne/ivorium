#pragma once

#include "WindowListener.hpp"
#include "Window.hpp"
#include "../OpenGL/RenderTarget.hpp"
#include <ivorium_core/ivorium_core.hpp>
#include <optional>

class GLFMDisplay;

namespace iv
{

class TestGlfmWindow
{
public:
                                    TestGlfmWindow( GLFMDisplay * display );
                                    ~TestGlfmWindow();
};

}
