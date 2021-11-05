#pragma once



typedef struct
{
    int flags=0;  // bit0-available, bit1-has bearing, bit2 has speed
    double lat,lon;
    float speed,cog;
	bool hasBearing() { return (flags & 2) == 2; }
	void setBearing(float v) { cog = v; }
	float getBearing() { return (/*360.0 - */ cog); }
	bool hasFixed() { return (flags & 1) == 1; }
	bool hasSpeed() { return (flags & 4) == 4; }
	float getSpeed() { return speed; }
}MyLocation_T;

