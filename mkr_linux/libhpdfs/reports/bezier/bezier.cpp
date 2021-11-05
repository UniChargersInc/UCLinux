#include "bezier.h"
using namespace std;
#if 0
int main_bezier(void) {
	std::vector<std::vector<Point> > pts;
	char endPointCount = 0;
	double inx;
	double iny;
	int ptCount = 0;
	int endPoint;
	double deltaT;

	cin >> ptCount >> deltaT;
	for (int i = 0; i < ptCount; i++) {
		cin >> inx >> iny >> endPoint;
		Point p(inx, iny);
		if (endPointCount == 0 && endPoint == 1) {
			pts.push_back(std::vector<Point>());
			pts[pts.size() - 1].push_back(p);
			endPointCount++;
			continue;
		}
		pts[pts.size() - 1].push_back(p);
		if (endPointCount != 0 && endPoint == 1 && i != ptCount - 1) {
			pts.push_back(std::vector<Point>());
			pts[pts.size() - 1].push_back(p);
			endPointCount++;
		}
	}


	for (std::size_t i = 0; i < pts.size(); i++) {
		for (double t = 0; t <= 1; t += deltaT) {
			cout << bezier(pts[i], t) << endl;
		}
	}
	return 0;
}

Point bezier(std::vector<Point>& pts, double t) {
	Point p;
	std::size_t len = pts.size();
	for (std::size_t i = 0; i < len; i++) {
		p += pts[i] * choose(len - 1, i) * pow(1 - t, len - 1 - i) * pow(t, i);
	}
	return p;
}

double factorial(double num) {
	if (num <= 1) {
		return 1;
	}
	double fac = 1;
	for (double i = 1; i <= num; i++) {
		fac *= i;
	}
	return fac;
}

double choose(double a, double b) {
	return factorial(a) / (factorial(b) * factorial(a - b));
}
#endif

#include <stdio.h>
#include <stdlib.h> 
#include <math.h> 

void bezierCubicCurve(int x[], int y[])
{
	double xu = 0.0, yu = 0.0, u = 0.0;
	int i = 0;
	for (u = 0.0; u <= 1.0; u += 0.05)
	{
		xu = pow(1 - u, 3)*x[0] + 3 * u*pow(1 - u, 2)*x[1] + 3 * pow(u, 2)*(1 - u)*x[2]
			+ pow(u, 3)*x[3];
		yu = pow(1 - u, 3)*y[0] + 3 * u*pow(1 - u, 2)*y[1] + 3 * pow(u, 2)*(1 - u)*y[2]
			+ pow(u, 3)*y[3];
		printf("X: %i   Y: %i \n", (int)xu, (int)yu);
	}
}
//https://stackoverflow.com/questions/5634460/quadratic-b%C3%A9zier-curve-calculate-points
BezPoint quadraticBezier(BezPoint p[] ,double t)  //quadratic Bezier formula
{
	//p[0] is the start point, p[1] is the control point, and p[2] is the end point. t is the parameter, which goes from 0 to 1.
//	float t = 0.5; // given example value
	double x = (1 - t) * (1 - t) * p[0].x + 2 * (1 - t) * t * p[1].x + t * t * p[2].x;
	double y = (1 - t) * (1 - t) * p[0].y + 2 * (1 - t) * t * p[1].y + t * t * p[2].y;

	BezPoint pOut(x, y);
	return pOut;
}

int main_bezier_test(void) {
	int x[] = { 0,75,50,300 };  
	int y[] = { 0,2,140,100 };
	bezierCubicCurve(x, y);
	return 0;
}
