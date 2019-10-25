#include "Voronoi.h"

Voronoi::Voronoi()
{
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

	beachLine->print2D(sweepLineY);

	//Handle the rest of events
	while (!eventPQ->isEmpty())
	{
		Event* event = eventPQ->top();
		eventPQ->pop();

		sweepLineY = event->point->pos.y;

		if (event->type == EventType::SITE)
		{
			//Site Event
			beachLine->InsertArc(event->point, eventPQ, sweepLineY);
		}
		else
		{
			//Circle Event
			beachLine->RemoveArc(event, eventPQ, sweepLineY, out.edges, out.vertices);
		}

		delete event;
	}

	finishRemainingEdges(beachLine->root, out.edges);

	if (beachLine->root)
	{
		std::cout << "WHY?" << std::endl;
	}

	return out;
}

void Voronoi::finishRemainingEdges(BeachLineBSTNode* curr, std::vector<Edge*> &edges)
{
	if (curr == nullptr)
		return;

	if (!curr->isLeaf)
	{
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

		finishRemainingEdges(curr->GetLeftChild(), edges);
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
	std::cout << " VOR Destructor" << std::endl;
}
