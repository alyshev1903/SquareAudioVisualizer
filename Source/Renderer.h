#pragma once
#include <juce_opengl/juce_opengl.h>
#include "VertexArray.h"
#include "Shader.h"

#if JUCE_WINDOWS
    extern "C" __declspec(dllimport) int __stdcall IsDebuggerPresent();
    #define ASSERT(x) if (!(x)) { if (::IsDebuggerPresent()) __debugbreak(); }
#else
    #define ASSERT(x) (void)(x)
#endif

#if JUCE_DEBUG
    #define GLCall(x) GLClearError();\
        x;\
        ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#else
    #define GLCall(x) x
#endif

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);
void DrawPoints(Shader & shader, const VertexArray & vao, int count, bool withGlow, float alpha = 1.0f, float sizeScale = 1.0f);
