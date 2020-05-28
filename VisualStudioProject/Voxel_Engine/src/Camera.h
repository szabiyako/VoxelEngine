#pragma once

#include "glm/glm.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Camera
{
private:
	//Base for View
	glm::vec3 m_Pos;
	glm::vec3 m_Front;
	glm::vec3 m_Up;

	//View matrix
	glm::mat4 m_View;

	//For projection
	float m_FOV;          // in degrees
	float m_AspectRatio;
	float m_NearSize;
	float m_FarSize;

	//Projection matrix
	glm::mat4 m_Proj;

	//MouseRotation
	float m_LastX;
	float m_LastY;
	float m_Yaw;
	float m_Pitch;
	bool m_FirstMouse;

	bool m_IsChangedView;
	bool m_IsChangedProj;
public:
	//Full constructor
	Camera(const glm::vec3&   pos          = glm::vec3(0.0f, 0.0f, 3.5f),
		   const glm::vec3&   front        = glm::vec3(0.0f, 0.0f, -1.0f),
		   const glm::vec3&   up           = glm::vec3(0.0f, 1.0f, 0.0f),
		   const float&       yaw          = -90.f, 
		   const float&       pitch        = 0.f,
		   const float&       aspectratio  = 800.f/600.f,
		   const float&       fov          = 45.f,
		   const float&       near         = 0.1f,    //0.01f provides z-fighting at far distances, so better use 0.5f or 1.f or rewrite viewprojection matrix (or 32 bits z buffer - doesnt help)
		   const float&       far          = 100000.f
	);

	void RotateFromCursor(const double& xpos, const double& ypos, const float& sensitivity);
	void ResetStartCursorPos(const double& xpos, const double& ypos);



	//Inline Methods
	inline glm::mat4 GetUImatrix()
	{
		return glm::ortho(-1.f, 1.f, -1.f / m_AspectRatio, 1.f / m_AspectRatio, 0.0f, 1.f);
	}

	//View
	inline void MoveFront(const float& distance)
	{
		float k = glm::length(m_Front) / glm::length(glm::vec2(m_Front.x, m_Front.z));
		m_Pos += distance * k * glm::vec3(m_Front.x, 0.f, m_Front.z); // or just normalize
		m_IsChangedView = true;
	}
	inline void MoveFrontToView(const float& distance)
	{
		m_Pos += distance * m_Front;
		m_IsChangedView = true;
	}
	inline void MoveUp(const float& distance)
	{ 
		m_Pos += distance * m_Up;
		m_IsChangedView = true;
	}
	inline void MoveRight(const float& distance)
	{
		m_Pos += distance * glm::normalize(glm::cross(m_Front, m_Up));
		m_IsChangedView = true;
	}
	inline void SetPos(const glm::vec3& pos)
	{
		m_Pos = pos;
		m_IsChangedView = true;
	}
	inline void SetYaw(const float& yaw)
	{
		m_Yaw = yaw;
		m_IsChangedView = true;
	}
	inline void SetPitch(const float& pitch)
	{
		m_Pitch = pitch;
		m_IsChangedView = true;
	}
	inline glm::vec3 GetPos() const { return m_Pos; }
	inline glm::vec3 GetFront() const { return m_Front; }
	inline float GetPitch() const { return m_Pitch; }
	inline float GetYaw() const { return m_Yaw; }
	inline glm::mat4 GetView()
	{
		if (m_IsChangedView)
		{
			m_IsChangedView = false;
			return m_View = glm::lookAt(m_Pos, m_Pos + m_Front, m_Up);
		}
		return m_View;
	}

	//Proj
	inline void SetNearSize(const float& near)
	{
		m_NearSize = near;
		m_IsChangedProj = true;
	}
	inline void SetFarSize(const float& far)
	{
		m_FarSize = far;
		m_IsChangedProj = true;
	}
	inline void SetFOV(const float& fov)
	{
		m_FOV = fov;
		m_IsChangedProj = true;
	}
	inline void SetAspectRatio(const float& aspectratio)
	{
		m_AspectRatio = aspectratio;
		m_IsChangedProj = true;
	}
	inline glm::mat4 GetProj()
	{
		if (m_IsChangedProj)
		{
			m_IsChangedProj = false;
			return m_Proj = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearSize, m_FarSize);
		}
		return m_Proj;
	}



	inline glm::mat4 GetViewProj()
	{
		if (m_IsChangedView)
		{
			m_IsChangedView = false;
			m_View = glm::lookAt(m_Pos, m_Pos + m_Front, m_Up);
		}
		if (m_IsChangedProj)
		{
			m_IsChangedProj = false;
			m_Proj = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearSize, m_FarSize);
		}
		return m_Proj * m_View;
	}
};

