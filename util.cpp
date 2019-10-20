#include "util.h"

int Vertex::siteCount = 0;

std::ostream& operator<<(std::ostream &out, position2D &pos)
{
	out << "[" << pos.x << "," << pos.y << "]";
	return out;
}