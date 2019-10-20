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