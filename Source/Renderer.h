#pragma once
#include <juce_opengl/juce_opengl.h>
#include "VertexArray.h"
#include "Shader.h"

extern "C" __declspec(dllimport) int __stdcall IsDebuggerPresent();

#define ASSERT(x) if (!(x)) { if (::IsDebuggerPresent()) __debugbreak(); }
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);