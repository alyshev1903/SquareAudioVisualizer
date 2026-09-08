#pragma once

#include "JuceHeader.h"
#include <juce_opengl/juce_opengl.h>
#include "Renderer.h"
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"
#include "Shader.h"

class Camera {
public:
	Camera(glm::vec3 eye,
		glm::vec3 viewDirection = glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		int defh = 540,
		int defw = 960,
		float def_sensitivity = 0.1f);
	~Camera();

	glm::mat4 GetViewMatrix() const;

	void Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, int currentWidth, int currentHeight);

	void ProcessMouseOrbit(glm::vec3 target, float deltaX, float deltaY);
	void Orbit(glm::vec3 target, float deltaYaw, float deltaPitch);

	float& GetSensitivity() { return m_Sensitivity; }

private:
	glm::vec3 m_ViewDirection;
	glm::vec3 m_Up;
	glm::vec3 m_Eye;

	int height;
	int width;
	float m_Sensitivity;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Camera)
};