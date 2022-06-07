#pragma once

//#include <glm/glm.hpp>
#include "klein/klein.hpp"

namespace Aves {

	class Cube {

	public:
		Cube();
		Cube(kln::motor pose, kln::line rate, kln::line inertia);
		~Cube() {}

		void step();

		kln::motor getPose() { return pose; }
		kln::motor getPoseInv() { return pose.inverse(); }
		kln::line getRate() { return rate; }

	private:

		kln::motor pose;	// position and orientation or motion

		kln::line rate;
		//kln::line accel; //  acceleration

		//kln::line momentum;
		//kln::line forque;	// force torque

		kln::line inertia;


		kln::line inertiaMap();
		kln::line inertiaMapInv(kln::line momentum);

		kln::line commutate(kln::line line1, kln::line line2);

		//kln::motor rateMotor;

		kln::motor deltaPose();

		kln::line deltaRate();

		void eulerIntegrationStep(int numSteps, float deltaTime);


		kln::line join = kln::point(1, 1, 1) & kln::point(1, 2, 3);
		kln::line meet = kln::plane(1, 1, 1, 1) ^ kln::plane(1, 2, 3, 4);

	};
}