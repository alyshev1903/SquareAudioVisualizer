#pragma once
#include "JuceHeader.h"
#include <juce_opengl/juce_opengl.h>


using namespace ::juce::gl;

class VertexBuffer
{
private:
	unsigned int m_RendererID;
public:
	VertexBuffer(const void* data, unsigned int size, unsigned int usage = GL_STATIC_DRAW);
	~VertexBuffer();

	void SetData(const void* data, unsigned int size, unsigned int offset = 0);

	void Bind() const;
	void Unbind() const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VertexBuffer)
};