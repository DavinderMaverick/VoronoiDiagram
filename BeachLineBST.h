#pragma once
#include "util.h"
#include "Event.h"
#include "EventPQ.h"
#include "BeachLineBSTNode.h"

#include <iostream>
#include <math.h>
#include <algorithm>
#include <assert.h>

class BeachLineBST
{
	//When there are 2 or more sites with same Y coord as the first site event, then special code is needed because there is no arc above the second site yet.
	//This variable is used in such cases
	float firstEventY;
	std::vector <Vertex *> untrackedVertices;

public:

	BeachLineBSTNode* root;
	BeachLineBST();
	void InsertArc(Vertex* site, EventPQ* eventPQ, float directrixY);//On Site Event
	void RemoveArc(Event *cEvent, EventPQ* eventPQ, float directrixY, std::vector<Edge*> &edges, std::vector<Vertex*> &vertices);//On Circle Event

	bool GetParabolaRayIntersection(HalfEdge &ray, Vertex *site, float directrixY, position2D &intersectionPt);

	float GetXOfBreakPoint(BeachLineBSTNode* breakpoint, float directrixY);//Returns x coord of breakpoint between left and right parabolas
	
	/*
		Checks For Circle Event where the item is aLeaf/Arc of Beachline and is the middle of triplet

		Check 0
		Left and Right Arc should exist to form a triplet

		Check 1
		If left and right arc of the middle arc are same then they won't converge. This happens when a new site is added and above it there is an existing arc

		Check 2
		Checks if intersection is possible
		There can be consecutive triples where the breakpoints, edges do not converge
	*/
	void CheckAndAddCircleEvent(BeachLineBSTNode* item, EventPQ* eventPQ, float directrixY);
	
	BeachLineBSTNode* GetArcByX(float x, float directrixY);//Returns the leaf node(Arch of Parabola) that is above a new site in the current beachline

	BeachLineBSTNode* GetArcByX2(float x, float directrixY);

	void print2DUtil(BeachLineBSTNode *curr, int space, float directrixY);

	void print2D(float directrixY);

	~BeachLineBST();
};

//Useful when we need to find left and right arcs and check for circle events and also in case of deleting events
//What is the arc immediately to the left of curr arc in the beachline sequence
BeachLineBSTNode* GetLeftArc(BeachLineBSTNode* curr);

//Useful when we need to find left and right arcs and check for circle events and also in case of deleting events
//What is the arc immediately to the right of curr arc in the beachline sequence
BeachLineBSTNode* GetRightArc(BeachLineBSTNode* curr);

//Useful when removing an arc in case of circle event
//To Find left and right BreakPoints for the arc to be deleted
//One of the BreakPoints will be directly the parent
//Other will be among one of the grandparents
void GetLeftAndRightBreakPoints(BeachLineBSTNode* curr, BeachLineBSTNode* &leftBreakPt, BeachLineBSTNode* &rightBreakPt);

//Useful for drawing BeachLine
//As well as getting intersection point on beach line when a new site event occurs
float GetArcYForXCoord(Vertex* site, float x, float directrixY);

//rays == half edges == edges currently being traced out by beach line
//https://stackoverflow.com/a/2932601
bool CheckRayIntersection(HalfEdge &rayA, HalfEdge &rayB, position2D &pos);