#pragma once
#include "core/MapPos.h"
#include "utils/Const.h"
#include "utils/GeomUtils.h"

#if (defined ANDROID || defined MKR_LINUX)
#include "models/mylocation.h"
#elif defined( WIN32)
#include "mylocation.h"
#endif
class MapUtils
{
private:
	
	static double scalarMultiplication(carto::MapPos &from, carto::MapPos &to, carto::MapPos &p);
public:
	static void getProjection(carto::MapPos &p, carto::MapPos &from, carto::MapPos &to, carto::MapPos &outP);

	static float distance(carto::MapPos &p1, carto::MapPos &p2);
	static float distance(const MyLocation_T& mloc, carto::MapPos &p2);
	static float Heading(const carto::MapPos& _this ,const carto::MapPos& ll2);
	static float Heading(const MyLocation_T& mloc1, const MyLocation_T& mloc2);
	static float Heading(const MyLocation_T& mloc1, const carto::MapPos& ll2);
	//static float getDistanceToLineSegment(carto::MapPos &p0,		carto::MapPos &p1,		carto::MapPos &p);

	static float getOrthogonalDistance(carto::MapPos &p, carto::MapPos &from, carto::MapPos &to);

	static float degreesDiff(float a1, float a2);
};