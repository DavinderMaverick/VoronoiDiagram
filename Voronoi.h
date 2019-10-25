#pragma once
#include "util.h"
#include "BeachLineBST.h"
#include "EventPQ.h"
#include "Event.h"

#include <vector>

struct VoronoiOutput
{
	std::vector<Edge*> edges;
	std::vector<Vertex*> sites;
	std::vector<Vertex*> vertices;
	
	void MemoryCleanUp()
	{
		for (auto &e : edges) { delete e; }
		for (auto &s : sites) { delete s; }
		for (auto &v : vertices) { delete v; }
	}
};

class Voronoi
{
	float sweepLineY;
	//std::vector<VoronoiCell*> voronoiCells;
	BeachLineBST* beachLine;
	EventPQ* eventPQ;
	float maxEdgeLength;
	
	std::vector<Vertex*> TempVertices;

public:
	Voronoi();

	//Site Positions and Extents of Bounding Box in Cartesian Coords
	VoronoiOutput generateVoronoi(std::vector<position2D>& sitePositions, float minX, float maxX, float minY, float maxY);

	//Convert Remaining Rays in BeachLineBST to edges by clipping them against the BoundingRect
	//Cleans Up BST along the way i.e. Does Inorder Traversal, deletes leaves, convertes Breakpoint Rays to Edges and deletes them as we traverse the BST
	void finishRemainingEdges(BeachLineBSTNode* curr, std::vector<Edge*> &edges);

	~Voronoi();
};