#pragma once
#include "Vector3.h"
#include "Input.h"
class Camera
{
protected:
	Vector3 pos;
	Vector3 front;
	Vector3 up;

	float pitch, yaw, roll, speed, distance;
	char *name;
	int last_m_x, last_m_y;
public:
	Camera();

	Vector3 GetPos();
	Vector3 GetFront();
	Vector3 GetUp();

	//setters
	void SetPos(float x, float y, float z);
	void SetFront(float x, float y, float z);
	void SetUp(float x, float y, float z);

	//getters
	float GetYaw();
	float GetPitch();
	char* GetName();

	//functions
	virtual void HandleInput(Input* input, float dt);
	virtual void Update(float dt);
	virtual void SetLastMousePos(int x, int y);

};

