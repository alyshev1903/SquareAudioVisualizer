#pragma once

#include <juce_opengl/juce_opengl.h>
#include "JuceHeader.h"

#include <string>
#include <unordered_map>
#include "vendor/glm/glm.hpp"

struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
private:
	std::string m_FilePath;
	unsigned int m_RendererID;
	std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	Shader(const std::string& filepath);
	Shader(const void* embeddedData, int dataSize);
	~Shader();

	void Bind() const;
	void Unbind() const;

	void SetUniform1i(const std::string& name, int value);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);

	
private:
	
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

	ShaderProgramSource ParseShader(const std::string& filepath);
	ShaderProgramSource ParseShaderSource(const String& embeddedSource);

	int GetUniformLocation(const std::string& name);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Shader)
};