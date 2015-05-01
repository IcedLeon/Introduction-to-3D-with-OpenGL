#include "Camera.h"

GLvoid Camera::BuildCamera(vec3		a_vPos,
						   vec3		a_vUp,
						   GLfloat	a_fYaw,
						   GLfloat	a_fPitch)
{
	this->m_vPos				= a_vPos;
	this->m_vUp					= a_vUp;
	this->m_fYaw				= a_fYaw;
	this->m_fPitch				= a_fPitch;
	this->m_vFront				= vec3(0.0f, 0.0f, -1.0f);
	this->m_vWorldUp			= m_vUp;
	this->m_fCamSpeed			= 25.0f;
	this->m_fMouseSensitivity	= 0.3f;
	this->m_fFOV				= 45.0f;
	//Re-just the camera vectors.
	this->UpdateCameraVectors();
}

GLvoid Camera::BuildCamera(GLfloat a_fPosX,
						   GLfloat a_fPosY,
						   GLfloat a_fPosZ,
						   GLfloat a_fUpX,
						   GLfloat a_fUpY,
						   GLfloat a_fUpZ,
						   GLfloat a_fYaw,
						   GLfloat a_fPitch)
{
	this->m_vPos				= vec3(a_fPosX, a_fPosY, a_fPosZ);
	this->m_vUp					= vec3(a_fUpX, a_fUpY, a_fUpZ);
	this->m_vFront				= vec3(0.0f, 0.0f, -1.0f);
	this->m_vWorldUp			= m_vUp;
	this->m_fCamSpeed			= 15.0f;
	this->m_fMouseSensitivity	= 0.5f;
	this->m_fFOV				= 45.0f;
	//Re-just the camera vectors.
	this->UpdateCameraVectors();
}

mat4 Camera::GetWorldTransform() const
{
	return glm::inverse(GetViewTransform());
}

mat4 Camera::GetViewTransform() const
{
	return lookAt(m_vPos, m_vPos + m_vFront, m_vWorldUp);
}

mat4 Camera::GetProjectionTransform(glm::vec2 a_vScreenSize, float a_fNearPlane, float a_fFarPlane) const
{
	float _AsRatio = a_vScreenSize.x / a_vScreenSize.y;
	return glm::perspective(m_fFOV, _AsRatio, a_fNearPlane, a_fFarPlane);
}

mat4 Camera::GetProjViewTransform(glm::vec2 a_vScreenSize) const
{
	mat4 _result = GetProjectionTransform(a_vScreenSize);
	_result *= GetViewTransform();
	return _result;
}

void Camera::KeyboardInput(CameraMovement a_eDir, GLfloat a_fDeltaTime)
{
	GLfloat _speed = this->m_fCamSpeed * a_fDeltaTime;

	switch (a_eDir)
	{
	case FORWARD:
		this->m_vPos += this->m_vFront * _speed;
		break;
	case BACKWARD:
		this->m_vPos -= this->m_vFront * _speed;
		break;
	case LEFT:
		this->m_vPos -= this->m_vRight * _speed;
		break;
	case RIGHT:
		this->m_vPos += this->m_vRight * _speed;
		break;
	default:
		break;
	}
	this->UpdateCameraVectors();
}

void Camera::MouseInput(GLfloat a_fOffsetX, GLfloat a_fOffsetY, GLboolean a_bConstrainPitch)
{
	a_fOffsetX *= this->m_fMouseSensitivity;
	a_fOffsetY *= this->m_fMouseSensitivity;

	this->m_fYaw += a_fOffsetX;
	this->m_fPitch += a_fOffsetY;

	if (a_bConstrainPitch)
	{
		if (this->m_fPitch > 89.0f)
		{
			this->m_fPitch = 89.0f;
		}
		if (this->m_fPitch < -89.0f)
		{
			this->m_fPitch = -89.0f;
		}
	}
	this->UpdateCameraVectors();
}

void Camera::MouseScrollZoom(GLfloat a_fOffsetY)
{
	if (this->m_fFOV >= 1.0f && this->m_fFOV <= 90.0f)
	{
		this->m_fFOV -= a_fOffsetY;
	}
	if (this->m_fFOV <= 1.0f && this->m_fFOV >= 90.0f)
	{
		this->m_fFOV += a_fOffsetY;
	}
	if (this->m_fFOV <= 1.0f)
	{
		this->m_fFOV = 1.0f;
	}
	if (this->m_fFOV >= 90.0f)
	{
		this->m_fFOV = 90.0f;
	}
	this->UpdateCameraVectors();
}

void Camera::UpdateCameraVectors()
{
	vec3 _front;
	//Re-calculate the front base on the current yaw and pitch of the camera.
	_front.x = cos(radians(this->m_fYaw)) * cos(radians(this->m_fPitch));
	_front.y = sin(radians(this->m_fPitch));
	_front.z = sin(radians(this->m_fYaw)) * cos(radians(this->m_fPitch));
	//Now we simply make them equal to our old front vector by normaling those new value.
	this->m_vFront	= normalize(_front);
	//and we also re-adjust right and up vector by normalize the cross product of the front vector and the worldup vector for the RIGHT
	//and by normaling the cross product of the new RIGHT and the new front vectors.
	this->m_vRight	= normalize(cross(this->m_vFront, this->m_vWorldUp));
	this->m_vUp		= normalize(cross(this->m_vRight, this->m_vFront));
}

vec3 Camera::GetRayFromMouse(float a_fX, float a_fY, glm::ivec2 a_viViewport)
{
	//Screen space coord (from viewport).
	float _X = (2.0f * a_fX) / a_viViewport.x - 1.0f;
	float _Y = 1.0f - (2.0f * a_fY) / a_viViewport.y;
	float _Z = 1.0f;
	//From NDS
	vec3 _rayNDS = vec3(_X, _Y, _Z);
	//From clip
	vec4 _rayClp = vec4(_rayNDS.x, _rayNDS.y, -1.0f, 1.0f);
	//From view
	mat4 _invProj = GetProjectionTransform(a_viViewport);
	_invProj = glm::inverse(_invProj);
	vec4 _rayViw = _invProj * _rayClp;
	_rayViw /= _rayViw.w;
	_rayViw = vec4(_rayViw.x, _rayViw.y, -1.0f, 0.0f);
	//To world space
	mat4 _invView = GetViewTransform();
	_invView = glm::inverse(_invView);
	vec3 _rayWld = vec3(_invView * _rayViw);
	//Normalization
	_rayWld = normalize(_rayWld);

	return _rayWld;
}

//vec3 Camera::ScreenPositionToDir(float a_fX, float a_fY, glm::ivec2 a_viViewport) const
//{
//	mat4 _proj = GetProjectionTransform(a_viViewport);
//	vec3 _screenPos(a_fX / a_viViewport.x * 2 - 1, (a_fY * 2 - 1) * -1, -1);
//
//	_screenPos.x /= _proj[0][0];
//	_screenPos.y /= _proj[1][1];
//
//	return normalize()
//}

vec3 Camera::PickAgainstPlain(float a_fX, float a_fY, glm::ivec2 a_vWinSize, vec4 a_vPlane)
{
	float _nxPos = a_fX / a_vWinSize.x;
	float _nyPos = a_fY / a_vWinSize.y;
	
	float _sxPos = _nxPos - 0.5f;
	float _syPos = _nyPos - 0.5f;

	float _fxPos = _sxPos * 2;
	float _fyPos = _syPos * -2;

	mat4 _invView = glm::inverse(GetProjViewTransform(a_vWinSize));

	vec4 _mousePos(_fxPos, _fyPos, 1.0f, 1.0f);


	vec4 _worldPos = _invView * _mousePos;

	_worldPos /= _worldPos.w;

	vec3 _camPos = GetWorldTransform()[3].xyz();

	vec3 _dir = _worldPos.xyz() - _camPos;

	float _t = -(glm::dot(_camPos, a_vPlane.xyz()) + a_vPlane.w) / (glm::dot(_dir, a_vPlane.xyz()));

	vec3 _result = _camPos + _dir * _t;

	return _result;
}

GLfloat Camera::GetZoom() const
{
	return m_fFOV;
}

GLfloat Camera::GetYaw() const
{
	return m_fYaw;
}

GLfloat Camera::GetPitch() const
{
	return m_fPitch;
}