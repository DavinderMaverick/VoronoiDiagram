#include "EventPQ.h"

void EventPQ::Heapify(int index)
{
	int l = left(index);
	int r = right(index);

	int largest = index;

	if (l < m_arr.size() && Event::greaterThan(m_arr[l], m_arr[largest]))
		largest = l;
	if (r < m_arr.size() && Event::greaterThan(m_arr[r], m_arr[largest]))
		largest = r;

	if (largest != index)
	{
		swap(m_arr[index], m_arr[largest]);
		Heapify(largest);
	}
}

void EventPQ::swap(Event* &a, Event* &b)
{
	Event *temp = a;
	a = b;
	b = temp;

	int tempIdx = a->pqIndex;
	a->pqIndex = b->pqIndex;
	b->pqIndex = tempIdx;
}

void EventPQ::push(Event *event)
{
	int index = m_arr.size();
	event->pqIndex = index;
	m_arr.push_back(event);
	if(m_arr.size() > 1)
		bubbleUp(index);
	std::cout << "PUSH ";
	confirmHeapProperty();
}

Event* EventPQ::top()
{
	if (m_arr.size() > 0)
	{
		return m_arr[0];
	}
	else
		return nullptr;
}

void EventPQ::pop()
{
	if (m_arr.size() == 1)
	{
		m_arr.pop_back();
		return;
	}

	if (m_arr.size() > 0)
	{
		m_arr[0] = m_arr[m_arr.size() - 1];
		m_arr.pop_back();
		m_arr[0]->pqIndex = 0;
		Heapify(0);
	}
	std::cout << "POP ";
	confirmHeapProperty();
}

void EventPQ::deleteEvent(Event* event)
{
	event->point->pos.y = FLT_MAX;
	bubbleUp(event->pqIndex);
	pop();
}

void EventPQ::bubbleUp(int index)
{
	while (index > 0 && Event::greaterThan(m_arr[index], m_arr[parent(index)]))
	{
		swap(m_arr[index], m_arr[parent(index)]);
		index = parent(index);
	}
}

void EventPQ::confirmHeapProperty()
{
	std::cout << "Heap Property Check " << std::endl;
	int index = 1;
	if (m_arr.size() <= 1)
	{
		std::cout << "OK" << std::endl;
		return;
	}
	while (index < m_arr.size())
	{
		if (Event::greaterThan(m_arr[index], m_arr[parent(index)]))
		{
			std::cout << "Heap Not OK" << std::endl;
			return;
		}
		index++;
	}
	std::cout << "OK" << std::endl;
}

EventPQ::EventPQ()
{
}


EventPQ::~EventPQ()
{
}