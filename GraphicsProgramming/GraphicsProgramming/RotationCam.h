#pragma once
#include "Camera.h"
class RotationCam :
	public Camera
{
public:
	RotationCam();
	void HandleInput(Input* input, float dt);
	void Update(float dt);

};

