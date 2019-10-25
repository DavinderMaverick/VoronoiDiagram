#include "BeachLineBSTNode.h"

BeachLineBSTNode::BeachLineBSTNode()
	: site(nullptr), cEvent(nullptr), lSite(nullptr), rSite(nullptr), m_lc(nullptr), m_rc(nullptr), m_parent(nullptr)
{
}

BeachLineBSTNode* BeachLineBSTNode::GetLeftChild()
{
	return this->m_lc;
}

BeachLineBSTNode* BeachLineBSTNode::GetRightChild()
{
	return this->m_rc;
}

BeachLineBSTNode* BeachLineBSTNode::GetParent()
{
	return this->m_parent;
}

void BeachLineBSTNode::SetLeftChild(BeachLineBSTNode *item)
{
	item->m_parent = this;
	this->m_lc = item;
}

void BeachLineBSTNode::SetRightChild(BeachLineBSTNode *item)
{
	item->m_parent = this;
	this->m_rc = item;
}

//Is this unecessary?
void BeachLineBSTNode::SetParent(BeachLineBSTNode *item, bool left)
{
	if (left)
		item->m_lc = this;
	else
		item->m_rc = this;
	this->m_parent = item;
}

BeachLineBSTNode::~BeachLineBSTNode()
{
}

BeachLineBSTNode* CreateArcItem(Vertex* site, Event* cEvent)
{
	BeachLineBSTNode* arc = new BeachLineBSTNode();

	arc->isLeaf = true;
	arc->site = site;
	arc->cEvent = cEvent;

	return arc;
}

BeachLineBSTNode* CreateBreakPointItem(Vertex* lSite, Vertex* rSite, HalfEdge& ray)
{
	BeachLineBSTNode* breakPt = new BeachLineBSTNode();

	breakPt->isLeaf = false;
	breakPt->lSite = lSite;
	breakPt->rSite = rSite;
	breakPt->ray = ray;

	return breakPt;
}