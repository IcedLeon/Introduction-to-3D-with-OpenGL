#include "Camera.h"

GLvoid Camera::BuildCamera(vec3 a_vPos,
	vec3 a_vUp,
	GLfloat a_fYaw,
	GLfloat a_fPitch)
{
	this->m_vPos = a_vPos;
	this->m_vUp = a_vUp;
	this->m_fYaw = a_fYaw;
	this->m_fPitch = a_fPitch;
	this->m_vFront = vec3(0.0f, 0.0f, -1.0f);
	this->m_fCamSpeed = 5.0f;
	this->m_fMouseSensitivity = 0.5f;
	this->m_fZoom = 45.0f;
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
	this->m_vPos = vec3(a_fPosX, a_fPosY, a_fPosZ);
	this->m_vUp = vec3(a_fUpX, a_fUpY, a_fUpZ);
	this->m_vFront = vec3(0.0f, 0.0f, -1.0f);
	this->m_fCamSpeed = 5.0f;
	this->m_fMouseSensitivity = 0.5f;
	this->m_fZoom = 45.0f;
	this->UpdateCameraVectors();
}

mat4 Camera::GetViewMatrix() const
{
	return lookAt(this->m_vPos, this->m_vPos + this->m_vFront, this->m_vWorldUp);
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
}

void Camera::MouseInput(GLfloat a_fOffsetX, GLfloat a_fOffsetY, GLboolean a_bConstrainPitch)
{
	a_fOffsetX *= this->m_fMouseSensitivity;
	a_fOffsetY *= this->m_fMouseSensitivity;

	this->m_fYaw += a_fOffsetX;
	this->m_fPitch += a_fOffsetY;
	//Unless I want to free the camera from the contrain and have it behave like a spectator camera,
	//this section should constrain the pitch between bounds so the screen do not get flipped freely.
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
		this->UpdateCameraVectors();
	}
}

void Camera::MouseScrollZoom(GLfloat a_fOffsetY)
{
	if (this->m_fZoom >= 1.0f && this->m_fZoom <= 45.0f)
	{
		this->m_fZoom -= a_fOffsetY;
	}
	if (this->m_fZoom <= 1.0f)
	{
		this->m_fZoom = 1.0f;
	}
	if (this->m_fZoom >= 45.0f)
	{
		this->m_fZoom = 45.0f;
	}
}

void Camera::UpdateCameraVectors()
{
	vec3 _front;
	//Re-calculate the front base on the current yaw and pitch of the camera.
	_front.x = cos(radians(this->m_fYaw)) * cos(radians(this->m_fPitch));
	_front.y = sin(radians(this->m_fPitch));
	_front.z = sin(radians(this->m_fYaw)) * cos(radians(this->m_fPitch));
	//Now we simply make them equal to our old front vector by normaling those new value.
	this->m_vFront = normalize(_front);
	//and we also re-adjust right and up vector by normalize the cross product of the front vector and the worldup vector for the RIGHT
	//and by normaling the cross product of the new RIGHT and the new front vectors.
	this->m_vRight = normalize(cross(this->m_vFront, this->m_vWorldUp));
	this->m_vUp = normalize(cross(this->m_vRight, this->m_vFront));
}