#pragma once
#include "Event.h"

#include <vector>
#include <iostream>

class EventPQ
{
	std::vector<Event *> m_arr;

	int left(int index) { return 2 * index + 1; }
	int right(int index) { return 2 * index + 2; }
	int parent(int index) { return (index - 1) / 2; }

	void Heapify(int index);
	void swap(Event* &a, Event* &b);

public:

	void push(Event* event);
	Event* top();
	void pop();
	void deleteEvent(Event* event);//Needed in case of false alarms
	void bubbleUp(int index);
	bool isEmpty() { return m_arr.size() == 0; }
	void confirmHeapProperty();

	EventPQ();
	~EventPQ();
};