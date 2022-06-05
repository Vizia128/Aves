#include "Cube.hpp"

namespace Aves {

	kln::line Cube::inertiaMap()
	{
		kln::line rateDual = !rate;
		float temp[6];

		temp[0] = rateDual.e01() * (inertia.e01() || 1);
		temp[1] = rateDual.e02() * (inertia.e02() || 1);
		temp[2] = rateDual.e03() * (inertia.e03() || 1);
		temp[3] = rateDual.e23() * (inertia.e23() || 1);
		temp[4] = rateDual.e31() * (inertia.e31() || 1);
		temp[5] = rateDual.e12() * (inertia.e12() || 1);

		return kln::line(temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]);
		
	}

	kln::line Cube::inertiaMapInv()
	{
		float temp[6];

		temp[0] = rate.e01() / (inertia.e01() || 1);
		temp[1] = rate.e02() / (inertia.e02() || 1);
		temp[2] = rate.e03() / (inertia.e03() || 1);
		temp[3] = rate.e23() / (inertia.e23() || 1);
		temp[4] = rate.e31() / (inertia.e31() || 1);
		temp[5] = rate.e12() / (inertia.e12() || 1);

		return ! kln::line(temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]);
	}

	kln::motor Cube::deltaPose()
	{
		kln::motor rateMotor;
		rateMotor.p1_ = rate.p1_;
		rateMotor.p2_ = rate.p2_;

		return pose * rateMotor / (-2);
	}

}