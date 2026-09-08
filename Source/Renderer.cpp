#include "Renderer.h"
#include <iostream>

using namespace ::juce::gl;

void GLClearError() {
    while (juce::gl::glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = juce::gl::glGetError()) {
        std::cout << "[OpenGL Error] (" << error << ")" << function
            << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

