#include "Voronoi.h"

Voronoi::Voronoi()
{
	state = VoronoiState::INIT;
	beachLine = new BeachLineBST();
	eventPQ = new EventPQ();
}

VoronoiOutput Voronoi::generateVoronoi(std::vector<position2D>& sitePositions, float minX, float maxX, float minY, float maxY)
{
	float dX = maxX - minX;
	float dY = maxY - minY;
	maxEdgeLength = std::sqrt(dX * dX + dY * dY);

	VoronoiOutput out;

	for (auto pos : sitePositions)
	{
		out.sites.push_back(new Vertex(pos, true));
	}

	for (auto site : out.sites)
	{
		//Create new site Event and push it to pq
		eventPQ->push(new Event(site));
	}

	//Handle the rest of events
	while (!eventPQ->isEmpty())
	{
		Event* event = eventPQ->top();
		eventPQ->pop();

		if (event->type == EventType::SITE)
		{
			//Site Event
			beachLine->InsertArc(event->point, eventPQ);
		}
		else
		{
			//Circle Event
			beachLine->RemoveArc(event, eventPQ, out.edges, out.vertices);
		}

		delete event;
	}

	finishRemainingEdges(beachLine->root, out.edges);

	return out;
}

void Voronoi::VoronoiInteractive(std::vector<position2D>& sitePositions, float minX, float maxX, float minY, float maxY, float sweepLineY, VoronoiOutput &out)
{
	switch (state)
	{
		case VoronoiState::INIT:
		{
			float dX = maxX - minX;
			float dY = maxY - minY;
			maxEdgeLength = std::sqrt(dX * dX + dY * dY);

			for (auto pos : sitePositions)
			{
				out.sites.push_back(new Vertex(pos, true));
			}

			for (auto site : out.sites)
			{
				//Create new site Event and push it to pq
				eventPQ->push(new Event(site));
			}
			state = VoronoiState::PROCESSING_EVENT_PQ;
		}

		case VoronoiState::PROCESSING_EVENT_PQ:
		{
			//Handle the rest of events
			while (!eventPQ->isEmpty())
			{
				Event* event = eventPQ->top();

				if (event->point->pos.y < minY)
					break;

				if (sweepLineY > event->point->pos.y)
					return;

				eventPQ->pop();

				if (event->type == EventType::SITE)
				{
					//Site Event
					beachLine->InsertArc(event->point, eventPQ);
				}
				else
				{
					//Circle Event
					beachLine->RemoveArc(event, eventPQ, out.edges, out.vertices);
				}

				delete event;
			}
			state = VoronoiState::COMPLETE_REMAINING_EDGES;
		}

		case VoronoiState::COMPLETE_REMAINING_EDGES:
		{
			//If EventPQ is not empty
			while (!eventPQ->isEmpty())
			{
				Event* event = eventPQ->top();
				eventPQ->pop();
				delete event;
			}
			finishRemainingEdges(beachLine->root, out.edges);
			beachLine->root = nullptr;
			state = VoronoiState::FINISHED;
		}

		default: return;
	}
}

void Voronoi::finishRemainingEdges(BeachLineBSTNode* curr, std::vector<Edge*> &edges)
{
	if (curr == nullptr)
		return;

	if (!curr->isLeaf)
	{
		finishRemainingEdges(curr->GetLeftChild(), edges);

		//curr is a breakpoint
		HalfEdge ray = curr->ray;
		Edge *e = new Edge();
		position2D edgeEnd;
		edgeEnd.x = ray.orig.x + maxEdgeLength * ray.dir.x;
		edgeEnd.y = ray.orig.y + maxEdgeLength * ray.dir.y;
		e->start = new Vertex(ray.orig);
		e->end = new Vertex(edgeEnd);
		edges.push_back(e);
	
		TempVertices.push_back(e->start);
		TempVertices.push_back(e->end);

		finishRemainingEdges(curr->GetRightChild(), edges);
	}

	delete curr;
}

Voronoi::~Voronoi()
{
	delete beachLine;
	delete eventPQ;
	for (auto &v : TempVertices)
	{
		delete v;
	}
}
