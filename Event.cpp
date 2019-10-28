#include "Event.h"

Event::Event(Vertex* point)
{
	this->point = point;
	this->type = EventType::SITE;
	this->arc = nullptr;
}

Event::Event(Vertex* point, BeachLineBSTNode *arc, position2D center)
{
	this->point = point;
	this->type = EventType::CIRCLE;
	this->arc = arc;
	this->circleCenter = center;
}

Event::~Event()
{
	if (this->type == EventType::CIRCLE)
	{
		delete point;
	}
}

bool Event::lessThan(const Event *a, const Event *b)
{
	if (a->point->pos.y == b->point->pos.y)
		return a->point->pos.x > b->point->pos.x;
	return a->point->pos.y < b->point->pos.y;
}

bool Event::greaterThan(const Event *a, const Event *b)
{
	if (a->point->pos.y == b->point->pos.y)
		return a->point->pos.x < b->point->pos.x;
	return a->point->pos.y > b->point->pos.y;
}