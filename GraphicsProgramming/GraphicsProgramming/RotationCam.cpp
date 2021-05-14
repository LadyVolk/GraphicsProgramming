#include "RotationCam.h"

RotationCam::RotationCam() {
	pos.x = 40;
	pos.y = 10;
	pos.z = 10;
	front.x = 1.6;
	front.y = -200.0;
	front.z = -380.0;
	up.x = 0.0;
	up.y = 1.0;
	up.z = 0.0;
	speed = 40.0;

	pitch = 9.8;
	yaw = 7.8;
	roll = 0;

	//last mouse position
	last_m_x = 0;
	last_m_y = 0;

	distance = front.length();

	name = "rotation";
}
void RotationCam::HandleInput(Input* input, float dt)
{
	int m_x, m_y;
	float x_dif, y_dif;

	m_x = input->getMouseX();
	m_y = input->getMouseY();

	x_dif = m_x - last_m_x;
	y_dif = m_y - last_m_y;

	last_m_x = m_x;
	last_m_y = m_y;

	yaw += x_dif * 0.01;
	pitch += y_dif * 0.01;

}

void RotationCam::Update(float dt) {
	Vector3 direction;
	direction.x = cos(yaw) * cos(pitch);
	direction.y = sin(pitch);
	direction.z = sin(yaw) * cos(pitch);

	front = direction.normalised();
	front.scale(distance);
}