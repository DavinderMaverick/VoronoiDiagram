#pragma once


#include <vld.h>

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
/*
	Vertex *lSite;
	Vertex *rSite;*/
};

struct HalfEdge
{
	position2D orig;
	Direction dir;
	//HalfEdge *dual; //Some edges need to be completed
	//Vertex *lSite;
	bool isMaxY;//To avoid precision issues otherwise we can set orig y to max y coord available but then when checking for this special case there will be precision issues, it's best to avoid precision issues
	HalfEdge() : isMaxY(false) {}
};

//https://www.skytopia.com/project/articles/compsci/clipping.html
//Modified Liang-Barsky line clipping algo for our needs
inline position2D clipEdge(HalfEdge &ray, float edgeLeft, float edgeRight, float edgeBottom, float edgeTop)
{
	float x0 = ray.orig.x;
	float y0 = ray.orig.y;

	//Extend and then clip
	float length = (edgeRight - edgeLeft) + (edgeTop - edgeBottom);

	float x1 = ray.dir.x * length;
	float y1 = ray.dir.y * length;

	float t0 = 0.0;
	float t1 = 1.0;

	float xdelta = x1 - x0;
	float ydelta = y1 - y0;
	float p, q, r;


	for (int edge = 0; edge < 4; edge++) 
	{   // Traverse through left, right, bottom, top edges.
		if (edge == 0) { p = -xdelta;    q = -(edgeLeft - x0); }
		if (edge == 1) { p = xdelta;     q = (edgeRight - x0); }
		if (edge == 2) { p = -ydelta;    q = -(edgeBottom - y0); }
		if (edge == 3) { p = ydelta;     q = (edgeTop - y0); }
		r = q / p;

		if (p > 0)
		{
			if (r < t1) t1 = r;         // EndPoint is clipped!
		}
	}

	float x1clip = x0 + t1 * xdelta;
	float y1clip = y0 + t1 * ydelta;

	return { x1clip, y1clip };
}

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