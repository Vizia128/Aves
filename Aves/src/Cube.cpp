#include "Cube.hpp"
#include <iostream>

namespace Aves {

	Cube::Cube()
		: pose(kln::motor(-1, 0, 0, 0, 0, 1, 3, -2)), rate(kln::line(0,0,0,0.1,0.001,0)), inertia(kln::line(2, 1, 3, 1, 1, 1))
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

		temp[0] = rateDual.e01() * (inertia.e01());
		temp[1] = rateDual.e02() * (inertia.e02());
		temp[2] = rateDual.e03() * (inertia.e03());
		temp[3] = rateDual.e23() * (inertia.e23());
		temp[4] = rateDual.e31() * (inertia.e31());
		temp[5] = rateDual.e12() * (inertia.e12());

		return kln::line(temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]);		
	}

	kln::line Cube::inertiaMapInv(kln::line momentum)
	{
		float temp[6];

		temp[0] = momentum.e01() / (inertia.e01());
		temp[1] = momentum.e02() / (inertia.e02());
		temp[2] = momentum.e03() / (inertia.e03());
		temp[3] = momentum.e23() / (inertia.e23());
		temp[4] = momentum.e31() / (inertia.e31());
		temp[5] = momentum.e12() / (inertia.e12());

		//std::cout << "\tI.e01 : " << inertia.e01() << "\tI.e02 : " << inertia.e02() << "\tI.e03 : " << inertia.e03()
		//	<< "\tI.e23 : " << inertia.e23() << "\tI.e31 : " << inertia.e31() << "\tI.e12 : " << inertia.e12() << std::endl;


		//std::cout << "\tM.e01 : " << momentum.e01() << "\tM.e02 : " << momentum.e02() << "\tM.e03 : " << momentum.e03()
		//	<< "\tM.e23 : " << momentum.e23() << "\tM.e31 : " << momentum.e31() << "\tM.e12 : " << momentum.e12()<<std::endl;

		//kln::line tempLine = !kln::line(temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]);

		//std::cout << "\tV.e01 : " << tempLine.e01() << "\tV.e02 : " << tempLine.e02() << "\tV.e03 : " << tempLine.e03()
		//	<< "\tV.e23 : " << tempLine.e23() << "\tV.e31 : " << tempLine.e31() << "\tV.e12 : " << tempLine.e12() << std::endl;

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