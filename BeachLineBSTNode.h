#pragma once

#include <vld.h>

#include "util.h"
#include "Event.h"

/*
	Leaf Nodes -> Arc (Contain the site)
	Internal Nodes -> Breakpoints (Contains a tuple of site and the edge being traced)
*/
class BeachLineBSTNode
{
	BeachLineBSTNode* m_lc;
	BeachLineBSTNode* m_rc;
	BeachLineBSTNode* m_parent;

public:
	bool isLeaf;
	position2D cachedBreakPointPos;

	//Leaf
	Vertex* site;
	Event* cEvent;//Circle Event in which this arc will disappear

	//Internal
	Vertex* lSite;
	Vertex* rSite;
	HalfEdge ray;

	BeachLineBSTNode* GetLeftChild();
	BeachLineBSTNode* GetRightChild();
	BeachLineBSTNode* GetParent();

	void SetLeftChild(BeachLineBSTNode *item);
	void SetRightChild(BeachLineBSTNode *item);
	void SetParent(BeachLineBSTNode *item, bool left);

	BeachLineBSTNode();

	~BeachLineBSTNode();
};

BeachLineBSTNode* CreateArcItem(Vertex* site, Event* cEvent = nullptr);

BeachLineBSTNode* CreateBreakPointItem(Vertex* lSite, Vertex* rSite, HalfEdge& ray);