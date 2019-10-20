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

	//Handle First Event and All Other Events that are in close proximity to first
	Event* firstEvent = eventPQ->top();
	eventPQ->pop();

	sweepLineY = firstEvent->point->pos.y;
	BeachLineBSTNode* firstArc = new BeachLineBSTNode();
	beachLine->root = firstArc;
	firstArc->isLeaf = true;
	firstArc->site = firstEvent->point; // Compatibility between points and sites
	firstArc->cEvent = nullptr;

	delete firstEvent;

	//Events in close proximity to first when the first one is degerate line
	while (!eventPQ->isEmpty() && (firstArc->site->pos.y - eventPQ->top()->point->pos.y < 1.0f))
	{
		Event *event = eventPQ->top();
		eventPQ->pop();

		sweepLineY = event->point->pos.y;

		BeachLineBSTNode* newArc = CreateArcItem(event->point);

		BeachLineBSTNode* arcAbove = (beachLine->root->isLeaf) ? beachLine->root : beachLine->GetArcByX2(event->point->pos.x, sweepLineY);

		HalfEdge ray;
		ray.orig = { (newArc->site->pos.x + arcAbove->site->pos.x) / 2, FLT_MAX };
		ray.dir = { 0.0f, -1.0f };
		ray.isMaxY = true;

		if (arcAbove->GetParent() == nullptr)
		{
			//Directly Root will be affected
			//Compare x
			if (arcAbove->site->pos.x <= newArc->site->pos.x)
			{
				BeachLineBSTNode* newBreakPt = CreateBreakPointItem(arcAbove->site, newArc->site, ray);
				beachLine->root = newBreakPt;
				newBreakPt->SetLeftChild(arcAbove);
				newBreakPt->SetRightChild(newArc);
			}
			else
			{
				BeachLineBSTNode* newBreakPt = CreateBreakPointItem(newArc->site, arcAbove->site, ray);
				beachLine->root = newBreakPt;
				newBreakPt->SetLeftChild(newArc);
				newBreakPt->SetRightChild(arcAbove);
			}
		}
		else
		{
			if (arcAbove->site->pos.x <= newArc->site->pos.x)
			{
				BeachLineBSTNode* newBreakPt = CreateBreakPointItem(arcAbove->site, newArc->site, ray);
				bool left = (arcAbove->GetParent()->GetLeftChild() == arcAbove) ? true : false;
				newBreakPt->SetParent(arcAbove->GetParent(), left); 
				newBreakPt->SetLeftChild(arcAbove);
				newBreakPt->SetRightChild(newArc);
			}
			else
			{
				BeachLineBSTNode* newBreakPt = CreateBreakPointItem(newArc->site, arcAbove->site, ray);
				bool left = (arcAbove->GetParent()->GetLeftChild() == arcAbove) ? true : false;
				newBreakPt->SetParent(arcAbove->GetParent(), left);
				newBreakPt->SetLeftChild(newArc);
				newBreakPt->SetRightChild(arcAbove);
			}
		}
	}

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
	
		finishRemainingEdges(curr->GetLeftChild(), edges);
		finishRemainingEdges(curr->GetRightChild(), edges);
	}
	delete curr;
}

Voronoi::~Voronoi()
{
	delete beachLine;
	delete eventPQ;
}
