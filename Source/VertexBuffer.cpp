#include "VertexBuffer.h"
#include <windows.h>
#include <GL/gl.h>
#include "Renderer.h"

using namespace ::juce::gl;

VertexBuffer::VertexBuffer(const void* data, unsigned int size, unsigned int usage)
{
	glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &m_RendererID);
}

void VertexBuffer::SetData(const void* data, unsigned int size, unsigned int offset)
{
	Bind();
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void VertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void VertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}