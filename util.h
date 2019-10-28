#pragma once
//#define DEBUG

#include <math.h>
#include <ostream>

class position2D
{
public:
	float x;
	float y;
	
	friend std::ostream& operator<<(std::ostream &out, position2D &pos);
};

class Vertex
{
public:
	position2D pos;
	bool isSite;
	static int siteCount;
	int siteID;
	Vertex()
	{
		pos.x = pos.y = 0.0f;
		isSite = false;
	}
	
	Vertex(position2D p, bool isSite = false)
	{
		pos = p;
		this->isSite = isSite;
		if (isSite)
		{
			siteID = siteCount++;
		}
	}
};

struct Direction
{
	float x;
	float y;
};

inline float magnitude(position2D vec)
{
	return sqrt(vec.x * vec.x + vec.y * vec.y);
}

inline position2D normalize(position2D vec)
{
	float length = magnitude(vec);

	position2D result;
	result.x = vec.x / length;
	result.y = vec.y / length;

	return result;
}

struct Edge
{
	Vertex *start;
	Vertex *end;
};

struct HalfEdge
{
	position2D orig;
	Direction dir;
	//HalfEdge *dual; //Some edges need to be completed
	bool isMaxY;//To avoid precision issues otherwise we can set orig y to max y coord available but then when checking for this special case there will be precision issues, it's best to avoid precision issues
	HalfEdge() : isMaxY(false) {}
};

static float clampf(float val, float min, float max)
{
	if (val < min) return min;
	if (val > max) return max;
	return val;
}

static float max(float a, float b)
{
	if (a > b) return a;
	return b;
}

static float min(float a, float b)
{
	if (a > b) return b;
	return a;
}