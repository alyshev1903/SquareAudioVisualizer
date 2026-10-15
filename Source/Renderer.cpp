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

void DrawPoints(Shader& shader, const VertexArray& vao, int count, bool withGlow, float alpha, float sizeScale)
{
    shader.SetUniform1f("u_Alpha", alpha);
    shader.SetUniform1f("u_SizeScale", sizeScale);
    vao.Bind();

    shader.SetUniform1i("u_Pass", 0);
    GLCall(glDrawArrays(GL_POINTS, 0, (GLsizei)count));

    if (!withGlow)
        return;

    shader.SetUniform1i("u_Pass", 1);
    GLCall(glDepthMask(GL_FALSE));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE));
    GLCall(glDrawArrays(GL_POINTS, 0, (GLsizei)count));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCall(glDepthMask(GL_TRUE));
}