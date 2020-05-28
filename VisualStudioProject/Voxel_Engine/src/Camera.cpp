#include "Camera.h"


Camera::Camera(const glm::vec3 & pos, const glm::vec3 & front, const glm::vec3 & up, const float & yaw, const float & pitch,
			   const float& aspectratio, const float& fov, const float& near, const float& far)
	: m_FirstMouse(true), m_LastX(0.f), m_LastY(0.f), m_IsChangedView(false), m_IsChangedProj(false),
	  m_Pos(pos), m_Front(front), m_Up(up), m_Yaw(yaw), m_Pitch(pitch),
	  m_AspectRatio(aspectratio), m_FOV(fov), m_NearSize(near), m_FarSize(far)
{
	m_View = glm::lookAt(m_Pos, m_Pos + m_Front, m_Up);
	m_Proj = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearSize, m_FarSize);
}


void Camera::RotateFromCursor(const double& xpos, const double& ypos, const float& sensitivity)
{
	if (m_FirstMouse)
	{
		m_LastX = (float)xpos;
		m_LastY = (float)ypos;
		m_FirstMouse = false;
	}

	float xoffset = (float)xpos - m_LastX;
	float yoffset = m_LastY - (float)ypos;
	m_LastX = (float)xpos;
	m_LastY = (float)ypos;

	xoffset *= sensitivity;
	yoffset *= sensitivity;

	m_Yaw += xoffset;
	m_Pitch += yoffset;

	if (m_Pitch > 89.f)
		m_Pitch = 89.f;
	if (m_Pitch < -89.f)
		m_Pitch = -89.f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	direction.y = sin(glm::radians(m_Pitch));
	direction.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	m_Front = glm::normalize(direction);

	m_IsChangedView = true;
}

void Camera::ResetStartCursorPos(const double & xpos, const double & ypos)
{
	m_LastX = (float)xpos;
	m_LastY = (float)ypos;
}
