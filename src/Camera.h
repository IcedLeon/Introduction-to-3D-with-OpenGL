#ifndef _CAMERA_H_
#define _CAMERA_H_
//Library
#include "gl_core_4_4.h"
#define GLM_SWIZZLE
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include <vector>

using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::lookAt;
using glm::radians;
using glm::normalize;
using glm::cross;

enum CameraMovement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera
{
private:
	//Attribute of the camera.
	vec3	m_vPos,
			m_vFront,
			m_vUp,
			m_vRight,
			m_vWorldUp;
	//Euclidian angles
	GLfloat m_fYaw,
			m_fPitch,
			//Camera option
			m_fCamSpeed,
			m_fMouseSensitivity,
			m_fFOV;
	//Recalculate the front vector from the camera Euclidian angles.
	void UpdateCameraVectors();

public:
	Camera() :	m_vPos(vec3(0.0f, 0.0f, 0.0f)),
				m_vFront(vec3(0.0f, 0.0f, 0.0f)),
				m_vUp(vec3(0.0f, 0.0f, 0.0f)),
				m_vRight(vec3(0.0f, 0.0f, 0.0f)),
				m_vWorldUp(vec3(0.0f, 0.0f, 0.0f)),
				m_fYaw(0.0f),
				m_fPitch(0.0f),
				m_fCamSpeed(0.0f),
				m_fMouseSensitivity(0.0f),
				m_fFOV(0.0f)
	{ }
	~Camera() { }
	//Camera builder with vectors
	void BuildCamera(vec3	 a_vPos,
					 vec3	 a_vUp	  = vec3(0.0f, 1.0f, 0.0f),
					 GLfloat a_fYaw	  = -90.0f,
					 GLfloat a_fPitch = 0.0f);
	//Camera builder with scalar values
	void BuildCamera(GLfloat a_fPosX,
					 GLfloat a_fPosY,
					 GLfloat a_fPosZ,
					 GLfloat a_fUpX,
					 GLfloat a_fUpY,
					 GLfloat a_fUpZ,
					 GLfloat a_fYaw,
					 GLfloat a_fPitch);
	//Return the view matrix
	mat4 GetWorldTransform() const;
	mat4 GetViewTransform() const;
	mat4 GetProjectionTransform(glm::vec2 a_vScreenSize, float a_fNearPlane = 0.01f, float a_fFarPlane = 100000.0f) const;
	mat4 GetProjViewTransform(glm::vec2 a_vScreenSize) const;

	//Keyborad input processor.
	void KeyboardInput(CameraMovement a_eDir, GLfloat a_fDeltaTime);
	//Mouse input processor.
	void MouseInput(GLfloat a_fOffsetX,
					GLfloat a_fOffsetY,
					GLboolean a_bConstrainPitch = true);
	void MouseScrollZoom(GLfloat a_fOffsetY);

	vec3 PickAgainstPlain(float a_fX, float a_fY, glm::ivec2 a_vWinSize, vec4 a_vPlane);

	GLfloat GetZoom() const;
	GLfloat GetYaw() const;
	GLfloat GetPitch() const;
};
#endif //!_CAMERA_H_