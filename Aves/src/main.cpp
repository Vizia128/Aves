#include <iostream>
#include <spdlog/spdlog.h>

#include "Log.hpp"
#include "World.hpp"
#include "Cube.hpp"

int main()
{
	//Aves::Log::Init();

	//Aves::World world;
	kln::line join = kln::point(0, 0, 1) & kln::point(0, 0, 2);
	kln::line meet = kln::plane(1, 0, 0, 0) ^ kln::plane(0, 1, 0, 0);
	kln::direction dir = kln::direction(1, 2, 3);



	kln::line line1 = kln::line(2, 3, 5, 7, 11, 13);
	long double a = line1.e01();
	long double b = line1.e02();
	long double c = line1.e03();
	long double d = line1.e23();
	long double e = line1.e31();
	long double f = line1.e12();
	long double g = a + b + c + d + e + f;
	kln::line line2 = kln::line(15, 17, 19, 23, 29, 31);
	kln::line line3 = line1 + line2;

	kln::motor motor = kln::motor();

	kln::motor linemotor;
	linemotor.p1_ = line1.p1_;
	linemotor.p2_ = line1.p2_;

	motor = motor * linemotor;



	kln::line test = line1 + line2 + line3;// +dir;

	//while (!world.shouldEnd())
	//{
	//	world.run();
	//}
	//dir = dir * 2;
}