#pragma once

#include <glm/glm.hpp>
#include "klein/klein.hpp"

namespace Aves {

	class Cube {

	public:
		Cube();
		~Cube();

	private:

		kln::motor pose;	// position and orientation or motion

		kln::line rate;
		kln::line accel; //  acceleration

		kln::line momentum;
		kln::line forque;	// force torque

		kln::line inertia;


		kln::line inertiaMap();
		kln::line inertiaMapInv();

		kln::motor rateMotor;

		kln::motor deltaPose();


		kln::line join = kln::point(1, 1, 1) & kln::point(1, 2, 3);
		kln::line meet = kln::plane(1, 1, 1, 1) ^ kln::plane(1, 2, 3, 4);

	};
}