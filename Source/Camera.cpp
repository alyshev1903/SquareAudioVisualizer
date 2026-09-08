#include "Camera.h"

using namespace ::juce::gl;

Camera::Camera(glm::vec3 eye, glm::vec3 viewDirection, glm::vec3 up, int defh, int defw, float def_sensitivity)
	: m_Eye(eye), m_ViewDirection(viewDirection), m_Up(up), height(defh), width(defw), m_Sensitivity(def_sensitivity)
{}

Camera::~Camera() {}

glm::mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(m_Eye, m_Eye + m_ViewDirection, m_Up);
}

void Camera::Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, int currentWidth, int currentHeight)
{
	glm::mat4 view = GetViewMatrix();

	if (currentHeight == 0) currentHeight = 1;

	float aspect = (float)currentWidth / (float)currentHeight;
	glm::mat4 projection = glm::perspective(glm::radians(FOVdeg), aspect, nearPlane, farPlane);

	shader.SetUniformMat4f("u_Camera", projection * view);
}

void Camera::ProcessMouseOrbit(glm::vec3 target, float deltaX, float deltaY)
{
	Orbit(target, deltaX * m_Sensitivity, -deltaY * m_Sensitivity);
}

void Camera::Orbit(glm::vec3 target, float deltaYaw, float deltaPitch)
{
	glm::vec3 offset = m_Eye - target;
	float radius = glm::length(offset);

	float yaw = atan2(offset.z, offset.x);
	float pitch = asin(offset.y / radius);

	yaw += glm::radians(deltaYaw);
	pitch += glm::radians(deltaPitch);

	float limit = glm::radians(89.0f);
	pitch = glm::clamp(pitch, -limit, limit);

	glm::vec3 newOffset;
	newOffset.x = radius * cos(pitch) * cos(yaw);
	newOffset.y = radius * sin(pitch);
	newOffset.z = radius * cos(pitch) * sin(yaw);

	m_Eye = target + newOffset;
	m_ViewDirection = glm::normalize(target - m_Eye);
}