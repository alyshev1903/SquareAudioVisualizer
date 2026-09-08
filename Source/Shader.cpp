#include "Shader.h"
#include "Renderer.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "String.h"

using namespace ::juce::gl;

Shader::Shader(const std::string& filepath)
	: m_FilePath(filepath), m_RendererID(0)
{
	ShaderProgramSource source = ParseShader(filepath);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::Shader(const void* embeddedData, int dataSize)
	: m_FilePath("<embedded>"), m_RendererID(0)
{
	juce::String fullSource = juce::String::createStringFromData(embeddedData, dataSize);
	ShaderProgramSource source = ParseShaderSource(fullSource);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererID));
}


ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);
	std::stringstream buffer;
	buffer << stream.rdbuf();
	return ParseShaderSource(juce::String(buffer.str()));
}

ShaderProgramSource Shader::ParseShaderSource(const juce::String& source)
{
	enum class shaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };

	juce::StringArray lines;
	lines.addLines(source);

	std::string ss[2];
	shaderType type = shaderType::NONE;

	for (auto& line : lines)
	{
		if (line.contains("#shader"))
		{
			if (line.contains("vertex"))        type = shaderType::VERTEX;
			else if (line.contains("fragment")) type = shaderType::FRAGMENT;
		}
		else if (type != shaderType::NONE)
		{
			ss[(int)type] += line.toStdString() + "\n";
		}
	}

	return { ss[0], ss[1] };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (!result) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

void Shader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}

void Shader::SetUniform1i(const std::string& name, int value)
{
	GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform1f(const std::string& name, float value)
{
	GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

int Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
		return m_UniformLocationCache[name];
	}
	
	int location = glGetUniformLocation(m_RendererID , name.c_str());
	if (location == -1) {
		std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
	}
	
	m_UniformLocationCache[name] = location;
	
	return location;
}