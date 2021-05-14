#pragma once
#include "Camera.h"
class MovableCam :
	public Camera
{
public:
	MovableCam();
	void HandleInput(Input* input, float dt);
	void Update(float dt);
	
};

