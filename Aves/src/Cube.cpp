#include "Cube.hpp"

namespace Aves {

	Cube::Cube()
		: pose(kln::motor()), rate(kln::line(0,0,0,3,0,0.001)), inertia(kln::line(1,1,1,1,1,1))
	{}

	Cube::Cube(kln::motor pose, kln::line rate, kln::line inertia)
		: pose(pose), rate(rate), inertia(inertia)
	{}

	void Cube::step()
	{
		eulerIntegrationStep(100, 0.1);
	}

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

	kln::line Cube::inertiaMapInv(kln::line momentum)
	{
		float temp[6];

		temp[0] = momentum.e01() / (inertia.e01() || 1);
		temp[1] = momentum.e02() / (inertia.e02() || 1);
		temp[2] = momentum.e03() / (inertia.e03() || 1);
		temp[3] = momentum.e23() / (inertia.e23() || 1);
		temp[4] = momentum.e31() / (inertia.e31() || 1);
		temp[5] = momentum.e12() / (inertia.e12() || 1);

		return ! kln::line(temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]);
	}

	kln::line Cube::commutate(kln::line line1, kln::line line2)
	{
		kln::motor lineM = line1 * line2 - line2 * line1;

		kln::line line;
		line.p1_ = lineM.p1_;
		line.p2_ = lineM.p2_;

		return line;
	}

	kln::motor Cube::deltaPose()
	{
		// deltaPose = -1/2 * Pose * Rate
		kln::motor rateMotor;
		rateMotor.p1_ = rate.p1_;
		rateMotor.p2_ = rate.p2_;

		return pose * rateMotor / (-2);
	}

	kln::line Cube::deltaRate()
	{
		// deltaRate = I^(-1)[ Forques + Rate x I[Rate] ] = I^(_1)[ Forques + 1/2 * (Rate * I[Rate] - I[Rate] * Rate)]

		kln::line Irate = inertiaMap();
		kln::line forques = kln::line(0,0,0,0,0,0);

		return inertiaMapInv(forques + commutate(rate, Irate) / 2);
	}

	void Cube::eulerIntegrationStep(int numSteps, float deltaTime)
	{
		for (int i = 0; i < numSteps; i++)
		{
			rate += deltaRate() * deltaTime / numSteps;
			pose += deltaPose() * deltaTime / numSteps;
		}
	}

}