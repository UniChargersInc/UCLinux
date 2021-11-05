#include "MapUtils.h"
#include <math.h>

constexpr float kRadEarthMeters = 6378160.187f;
constexpr float kPi = 3.14159265f;

const float EPSILON = 0.001;

float MapUtils::distance(carto::MapPos &p1, carto::MapPos & p2)
{
	// If points are the same, return 0
	if (p1 == p2)
		return 0.0f;

	// Delta longitude. Don't need to worry about crossing 180
	// since cos(x) = cos(-x)
	double deltalng = (p2.getX() - p1.getX()) * carto::Const::DEG_TO_RAD;
	double a = p1.getY() * carto::Const::DEG_TO_RAD;
	double c = p2.getY() * carto::Const::DEG_TO_RAD;

	// Find the angle subtended in radians (law of cosines)
	double cosb = (sin(a) * sin(c)) + (cos(a) * cos(c) * cos(deltalng));

	// Angle subtended * radius of earth (portion of the circumference).
	// Protect against cosb being outside -1 to 1 range.
	if (cosb >= 1.0)
		return 0.00001f;
	else if (cosb < -1.0)
		return kPi * kRadEarthMeters;
	else
		return (float)(acos(cosb) * kRadEarthMeters);
}
float MapUtils::distance(const MyLocation_T& mloc, carto::MapPos &p2)
{
	carto::MapPos p1(mloc.lon, mloc.lat);
	return distance(p1, p2);
}
//https://stackoverflow.com/questions/3932502/calculate-angle-between-two-latitude-longitude-points

// Angular measures -> copied from valhalla\midgard\constants.h
//constexpr float kPi = 3.14159265f;
//constexpr float kPiOver2 = (kPi * 0.5f);
//constexpr float kPiOver4 = (kPi * 0.25f);
//constexpr float kDegPerRad = (180.0f / M_PI/*kPi*/);  // Radians to degrees conversion
//constexpr float kRadPerDeg = (M_PI / 180.0);//(kPi / 180.0f);  // Degrees to radians conversion
//constexpr float kEpsilon = 0.000001f;
/*
*@return   Returns the heading in degrees with range[0, 360] where 0 is
*           due north, 90 is east, 180 is south, and 270 is west.
*/

float MapUtils::Heading(const carto::MapPos& _this,const carto::MapPos& ll2)  {
	// If points are the same, return 0
	if (_this == ll2)
		return 0.0f;

	// Convert to radians and compute heading
	float lat1 = _this.getY() * /*kRadPerDeg*/carto::Const::DEG_TO_RAD;
	float lat2 = ll2.getY() * carto::Const::DEG_TO_RAD;
	float dlng = (ll2.getX() - _this.getX()) * carto::Const::DEG_TO_RAD;
	float y = sinf(dlng) * cosf(lat2);
	float x = cosf(lat1) * sinf(lat2) - sinf(lat1) * cosf(lat2) * cosf(dlng);
	float bearing = atan2f(y, x) * carto::Const::RAD_TO_DEG;
	bearing= (bearing < 0.0f) ? bearing + 360.0f : bearing;

	return bearing; 
	//return (360.0f - bearing);
}

float MapUtils::Heading(const MyLocation_T& mloc1, const MyLocation_T& mloc2)
{
	carto::MapPos mpos1(mloc1.lon, mloc1.lat);
	carto::MapPos mpos2(mloc2.lon, mloc2.lat);
	return Heading(mpos1, mpos2);
}
float MapUtils::Heading(const MyLocation_T& mloc1, const carto::MapPos& mpos2)
{
	carto::MapPos mpos1(mloc1.lon, mloc1.lat);
	return Heading(mpos1, mpos2);
}

//float MapUtils::getDistanceToLineSegment(carto::MapPos &pA,	carto::MapPos &pB,	carto::MapPos &p)
//{
//	//https://brilliant.org/wiki/dot-product-distance-between-point-and-a-line/
//	//The distance  from a point(x0,y0)  to the line AB ax+by+c=0 is d=abs(a(x0)+b(y0)+c)/sqrt(a^2+b^2))
//#if 0
//	const double Kr = 1.0;// carto::Const::DEG_TO_RAD;
//
//	double x0 = p.getX()*Kr, y0 = p.getY()*Kr;
//	double xa = pA.getX()*Kr, ya = pA.getY()*Kr;
//	double xb = pB.getX()*Kr, yb = pB.getY()*Kr;
//
//	//if (abs(xb - xa) < EPSILON*EPSILON)
//	//{
//	//	//AB is a vertical line
//	//	return 0;
//	//}
//
//	//float k = (yb - ya) / (xb - xa);
//	//float a = -k, b = 1, c = k * xa - ya;   // line equation coefficients
//
//	double a = -(yb - ya);
//	double b = xb - xa;
//	double c = yb * xa - xb * ya;
//#else
//	double x0 = p.getX(), y0 = p.getY();
//	double xa = pA.getX(), ya = pA.getY();
//	double xb = pB.getX(), yb = pB.getY();
//
//	double a = -(yb - ya);
//	double b = xb - xa;
//	double c = yb * xa - xb * ya;
//#endif
//	
//	double d = abs(a*x0 + b * y0 + c) / sqrt(a*a + b * b);
//
//	return (float)(d*(carto::Const::DEG_TO_RAD*kRadEarthMeters));
//}

//==================from OsmAnd MapUtils===============================
//public static double getOrthogonalDistance(double lat, double lon, double fromLat, double fromLon, double toLat, double toLon) {
//	return getDistance(getProjection(lat, lon, fromLat, fromLon, toLat, toLon), lat, lon);
//}
float MapUtils::getOrthogonalDistance(carto::MapPos &p, carto::MapPos &from, carto::MapPos &to)
{
	carto::MapPos outP;
	getProjection(p, from, to, outP);
	return distance(outP, p);
}
void MapUtils::getProjection(carto::MapPos &p, carto::MapPos &from, carto::MapPos &to, carto::MapPos &outP)
{
	// not very accurate computation on sphere but for distances < 1000m it is ok
	double mDist = (from.getY() - to.getY()) * (from.getY() - to.getY()) + (from.getX() - to.getX()) * (from.getX() - to.getX()); 
	double projection = scalarMultiplication(from, to, p);
	double prlat;
	double prlon;
	if (projection < 0) {
		prlat = from.getY();
		prlon = from.getX();
	}
	else if (projection >= mDist) {
		prlat = to.getY();
		prlon = to.getX();
	}
	else {
		prlat = from.getY() + (to.getY() - from.getY()) * (projection / mDist);
		prlon = from.getX() + (to.getX() - from.getX()) * (projection / mDist);
	}
	outP.setX(prlon);
	outP.setY(prlat);
}
double MapUtils::scalarMultiplication(carto::MapPos &from, carto::MapPos &to, carto::MapPos &p)
{
	//double multiple = (xB - xA) * (xC - xA) + (yB- yA) * (yC -yA);
	double multiple = (to.getX() - from.getX()) * (p.getX() - from.getX()) + (to.getY() - from.getY()) * (p.getY() - from.getY());
	return multiple;
}

float MapUtils::degreesDiff(float a1, float a2) {
	float diff = a1 - a2;
	while (diff > 180) {
		diff -= 360;
	}
	while (diff <= -180) {
		diff += 360;
	}
	return diff;

}