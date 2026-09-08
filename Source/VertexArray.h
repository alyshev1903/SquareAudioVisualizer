#pragma once
#include "JuceHeader.h"
#include "VertexBuffer.h"
#include <juce_opengl/juce_opengl.h>

class VertexBufferLayout;

class VertexArray
{
private:
	unsigned int m_RendererID;
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

	void Bind() const;
	void Unbind() const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VertexArray)
};