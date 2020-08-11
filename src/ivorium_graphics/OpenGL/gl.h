#pragma once

#include <ivorium_config.hpp>

#if IV_GLPLATFORM_GLFW
    #include <GL/glew.h>
    #include <GL/gl.h>
#elif IV_GLPLATFORM_GLFM
    #include <GLES3/gl3.h>
#else
    #error "Unknown OpenGL platform."
#endif

