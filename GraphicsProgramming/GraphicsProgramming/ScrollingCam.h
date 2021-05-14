#pragma once
#include "Camera.h"
class ScrollingCam :
	public Camera
{
public:
	ScrollingCam();
	void HandleInput(Input* input, float dt);
	void Update(float dt);
};

