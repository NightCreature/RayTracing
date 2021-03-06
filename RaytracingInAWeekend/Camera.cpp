//#include "DXUT.h"

#include "Camera.h"
#include "Math/matrix33.h"

#include <Windows.h>
#include <directxmath.h>

const float Camera::m_maxPitchAngle = 89.9f;

Camera::Camera() :
m_roll(0.0f),
m_yaw(0.0f),
m_pitch(0.0f),
m_movementSpeed(50.0f),
m_rotationSpeed(25.0f)
{
}

Camera::Camera(float movementSpeed, float rotationSpeed) :
m_roll(0.0f),
m_yaw(0.0f),
m_pitch(0.0f),
m_movementSpeed(movementSpeed),
m_rotationSpeed(rotationSpeed)
{
}

Camera::~Camera()
{
}

void Camera::positionCamera(const Vector3& eye, const Vector3& lookat, const Vector3& up)
{
    m_eye = eye;
    m_cameraForward = lookat - eye;
    m_cameraForward.normalize();
    cross(m_cameraRight, up, m_cameraForward);
    m_cameraForward.normalize();
    m_cameraRight.normalize();
    cross(m_cameraUp, m_cameraForward, m_cameraRight);
    createCamera();
}

void Camera::roll(float angle)
{
    Matrix33 rollRotation;
    rotate(rollRotation, m_cameraForward, angle);
    m_roll += angle; //Keep track of the roll angle
    m_cameraRight = m_cameraRight * rollRotation;
    m_cameraUp    = m_cameraUp * rollRotation;
}

void Camera::pitch(float angle)
{
    Matrix33 pitchRotation;
    rotate(pitchRotation, m_cameraRight, angle);
    m_pitch += angle; //Keep track of the pitch angle
    m_cameraForward = m_cameraForward * pitchRotation;
    m_cameraUp      = m_cameraUp * pitchRotation;
}

void Camera::yaw(float angle)
{
    Matrix33 yawRotation;
    rotate(yawRotation, m_cameraUp, angle);
    m_yaw += angle; //Keep track of the yaw angle
    m_cameraForward = m_cameraForward * yawRotation;
    m_cameraRight   = m_cameraRight * yawRotation;
}

void Camera::moveOverPlane(float deltau, float deltav, float deltan)
{
    m_cameraPositionDelta = Vector3(
        deltau * m_cameraRight.x() + deltav * m_cameraUp.x() + deltan * m_cameraForward.x(),
        deltau * m_cameraRight.y() + deltav * m_cameraUp.y() + deltan * m_cameraForward.y(),
        deltau * m_cameraRight.z() + deltav * m_cameraUp.z() + deltan * m_cameraForward.z());

    m_eye = m_eye + m_cameraPositionDelta;
}

Matrix44 Camera::createCamera()
{
    m_cameraForward.normalize();
    m_cameraUp.normalize();
    cross(m_cameraRight, m_cameraUp, m_cameraForward);
    m_cameraRight.normalize();
    cross(m_cameraUp, m_cameraForward, m_cameraRight);
    m_cameraUp.normalize();
  
    Vector3 evec(m_eye);
    m_camera = Matrix44(m_cameraRight.x(),    m_cameraRight.y(),    m_cameraRight.z(),    -evec.dot(m_cameraRight),
                        m_cameraUp.x(),       m_cameraUp.y(),       m_cameraUp.z(),       -evec.dot(m_cameraUp),
                        m_cameraForward.x(),  m_cameraForward.y(),  m_cameraForward.z(),  -evec.dot(m_cameraForward),
                        0.0f,	              0.0f,	                0.0f,		          1.0f);

    m_camera.transpose(); //We want row major matrices
    m_invcamera = m_camera.inverted();
    return m_camera;
}

float Camera::restrictAngleTo360(float angle) const
{
    while(angle > 2 * math::gmPI)
        angle -= 2 * math::gmPI;

    while(angle < 0)
        angle += 2 * math::gmPI;

    return angle;
}
