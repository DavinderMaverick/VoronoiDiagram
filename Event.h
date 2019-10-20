#pragma once
#include "util.h"
#include <limits.h>

enum class EventType
{
	CIRCLE,
	SITE
};

class BeachLineBSTNode;

//Make these independent of raylib library so need your own x and y coords
/*

	This is the event which is stored in Event Queue
	It could be either a site event or a circle event

	point: the point at which the current event occurs (bottom most point of circle in case of circle event, site point in case of site event)

	//events are sorted by y

	//Pointer to the arc/leaf of BST Node that will be deleted in case of circle event
*/

class Event
{
	Event();
public:

	static bool lessThan(const Event *a, const Event *b);
	static bool greaterThan(const Event *a, const Event *b);

	Vertex* point;//Point at which this event is triggered
	//Use type to identify site vs circle event
	EventType type;
	BeachLineBSTNode *arc;
	position2D circleCenter;
	int pqIndex;//Managed by EventPQ

	Event(Vertex* point); //Site Event
	Event(Vertex* point, BeachLineBSTNode *arc, position2D center); //Circle Event

	~Event();
};