#include "Camera.h"
#include "Input.h"
#include <math.h>
#include <iostream>

Camera::Camera() {

	pos.x = 4.7;
	pos.y = 94;
	pos.z = 205;
	front.x = 1.6;
	front.y = -200.0;
	front.z = -380.0;
	up.x = 0.0;
	up.y = 1.0;
	up.z = 0.0;
	speed = 50.0;

	pitch = 9.8;
	yaw = 7.8;
	roll = 0;

	//last mouse position
	last_m_x = 0;
	last_m_y = 0;

	distance = front.length();
}

void Camera::HandleInput(Input *input, float dt)
{
}

Vector3 Camera::GetPos() {
	return pos;
}
Vector3 Camera::GetFront() {
	return front;
}
Vector3 Camera::GetUp() {
	return up;
}
void Camera::SetPos(float x, float y, float z) {
	pos.x = x;
	pos.y = y;
	pos.z = z;
}
void Camera::SetFront(float x, float y, float z) {
	front.x = x;
	front.y = y;
	front.z = z;
}
void Camera::SetUp(float x, float y, float z) {
	up.x = x;
	up.y = y;
	up.z = z;
}
void Camera::Update(float dt) {
	
}
void Camera::SetLastMousePos(int x, int y) {
	last_m_x = x;
	last_m_y = y;
}
float Camera::GetYaw() {
	return yaw;
}
float Camera::GetPitch() {
	return pitch;
}
char* Camera::GetName() {
	return name;
}