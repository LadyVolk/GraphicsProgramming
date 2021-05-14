#include "FixedCam.h"

FixedCam::FixedCam() {
	pos.x = -4;
	pos.y = 100;
	pos.z = 250;
	front.x = 1.6;
	front.y = -200.0;
	front.z = -380.0;
	up.x = 0.0;
	up.y = 1.0;
	up.z = 0.0;

	name = "fixed";
}