#ifndef __BEZIER_H__
#define __BEZIER_H__

#include <cmath>
#include <iostream>
#include <vector>

#if 0
#include "BzPoint.h"


Point bezier(std::vector<Point>& pts, double t);
double factorial(double num);
double choose(double a, double b);
#endif

struct BezPoint {
	double x, y;
	BezPoint() { x = 0, y = 0; }
	BezPoint(double _x, double _y)
	{
		x = _x;
		y = _y;
	}
};
BezPoint quadraticBezier(BezPoint p[], double t);
#endif
