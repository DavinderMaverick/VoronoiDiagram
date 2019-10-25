#include "BeachLineBST.h"
#define COUNT 10

BeachLineBST::BeachLineBST()
{
}

void BeachLineBST::InsertArc(Vertex * s, EventPQ * eventPQ, float directrixY)
{
	//Inserting First Item in Tree
	if (!root)
	{
		firstEventY = s->pos.y;
		root = CreateArcItem(s);
		return;
	}

	//Sites in close proximity to first when the first one is degerate line
	if(firstEventY - s->pos.y < 1.0f)
	{
		BeachLineBSTNode* newArc = CreateArcItem(s);

		BeachLineBSTNode* arcAbove = (root->isLeaf) ? root : GetArcByX2(s->pos.x, directrixY);

		HalfEdge ray;

		ray.orig = { (newArc->site->pos.x + arcAbove->site->pos.x) / 2, 1000.0f };
		ray.dir = { 0.0f, -1.0f };
		ray.isMaxY = true;

		if (arcAbove->GetParent() == nullptr)
		{
			//Directly Root will be affected
			//Compare x
			if (arcAbove->site->pos.x <= newArc->site->pos.x)
			{
				BeachLineBSTNode* newBreakPt = CreateBreakPointItem(arcAbove->site, newArc->site, ray);
				root = newBreakPt;
				newBreakPt->SetLeftChild(arcAbove);
				newBreakPt->SetRightChild(newArc);
			}
			else
			{
				BeachLineBSTNode* newBreakPt = CreateBreakPointItem(newArc->site, arcAbove->site, ray);
				root = newBreakPt;
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
		return;
	}

	position2D t = s->pos;

	//BeachLineBSTNode* arcToReplace = GetArcByX(t.x, directrixY);
	BeachLineBSTNode* arcToReplace = GetArcByX2(t.x, directrixY);

	if (arcToReplace->cEvent)
	{
		Event *temp = arcToReplace->cEvent;
		eventPQ->deleteEvent(temp);
		delete temp;
		arcToReplace->cEvent = nullptr;
	}

	BeachLineBSTNode* arcLeft = CreateArcItem(arcToReplace->site);
	BeachLineBSTNode* arcRight = CreateArcItem(arcToReplace->site);
	BeachLineBSTNode* newArc = CreateArcItem(s);

	float intersectionY = GetArcYForXCoord(arcToReplace->site, t.x, directrixY);

	//Making 2 new edges for 2 new breakpoints

	position2D start = { t.x, intersectionY };

	HalfEdge edgeLeft;
	HalfEdge edgeRight;

	edgeLeft.orig = start;
	edgeRight.orig = start;

	//TODO: Dual Fix

	position2D dir = { arcToReplace->site->pos.x - newArc->site->pos.x,
	arcToReplace->site->pos.y - newArc->site->pos.y };

	position2D bisectorDir = normalize({ -dir.y, dir.x });

	edgeLeft.dir = { bisectorDir.x, bisectorDir.y };
	edgeRight.dir = { -bisectorDir.x, -bisectorDir.y };

	BeachLineBSTNode *breakPtLeftEdge = CreateBreakPointItem(arcToReplace->site, newArc->site, edgeLeft);

	BeachLineBSTNode *breakPtRightEdge = CreateBreakPointItem(newArc->site, arcToReplace->site, edgeRight);

	if (arcToReplace->GetParent() == nullptr)
	{
		//Directly Root will be affected
		root = breakPtLeftEdge;
		
		breakPtLeftEdge->SetLeftChild(arcLeft);
		breakPtLeftEdge->SetRightChild(breakPtRightEdge);
		breakPtRightEdge->SetLeftChild(newArc);
		breakPtRightEdge->SetRightChild(arcRight);
	}
	else
	{
		bool left = (arcToReplace->GetParent()->GetLeftChild() == arcToReplace) ? true : false;
		breakPtLeftEdge->SetParent(arcToReplace->GetParent(), left);
		breakPtLeftEdge->SetLeftChild(arcLeft);
		breakPtLeftEdge->SetRightChild(breakPtRightEdge);
		breakPtRightEdge->SetLeftChild(newArc);
		breakPtRightEdge->SetRightChild(arcRight);
	}

	delete arcToReplace;

	//Check For Circle Events
	CheckAndAddCircleEvent(arcLeft, eventPQ, directrixY);
	CheckAndAddCircleEvent(arcRight, eventPQ, directrixY);

	print2D(directrixY);
}

void BeachLineBST::RemoveArc(Event * cEvent, EventPQ * eventPQ, float directrixY, std::vector<Edge*>& edges, std::vector<Vertex*>& vertices)
{
	BeachLineBSTNode* toDeleteArc = cEvent->arc;

	//Find the left and right breakpoint
	//If there is a circle event then there will be left and right breakpoints Assertion
	BeachLineBSTNode* leftBreakPt = nullptr;
	BeachLineBSTNode* rightBreakPt = nullptr;

	GetLeftAndRightBreakPoints(toDeleteArc, leftBreakPt, rightBreakPt);
	assert(leftBreakPt != rightBreakPt);

	BeachLineBSTNode* leftArc = GetLeftArc(toDeleteArc);
	BeachLineBSTNode* rightArc = GetRightArc(toDeleteArc);
	std::cout << leftArc << " " << rightArc << std::endl;
	assert(leftArc != rightArc);

	//Delete the Circle Events of the predecessor and successor arcs
	if (leftArc && leftArc->cEvent)
	{
		Event *temp = leftArc->cEvent;
		eventPQ->deleteEvent(temp);
		delete temp;
		leftArc->cEvent = nullptr;
	}

	if (rightArc && rightArc->cEvent)
	{
		Event *temp = rightArc->cEvent;
		eventPQ->deleteEvent(temp);
		delete temp;
		rightArc->cEvent = nullptr;
	}
	
	Vertex *vertex = new Vertex(cEvent->circleCenter);
	vertices.push_back(vertex);

	//From breakpoints we also get rays

	//These rays will now be converted to line segments/final edges, and also vertex records will be added in Voronoi Diags
	//The vertex will be center of the circle of the circle Event
	//This vertex will also be orig of the new edge of new breakpoint

	Edge *leftEdge = new Edge();
	leftEdge->start = new Vertex(leftBreakPt->ray.orig);
	untrackedVertices.push_back(leftEdge->start);
	leftEdge->end = vertex;

	//position2D dirTest = normalize({leftBreakPt->ray.orig.x - vertex->pos.x, leftBreakPt->ray.orig.y - vertex->pos.y});

	//assert(dirTest.x == leftBreakPt->ray.dir.x && dirTest.y == leftBreakPt->ray.dir.y);

	Edge *rightEdge = new Edge();
	rightEdge->start = new Vertex(rightBreakPt->ray.orig);
	untrackedVertices.push_back(rightEdge->start);
	rightEdge->end = vertex;

	if (leftBreakPt->ray.isMaxY)
	{
		leftEdge->start->pos.y = 1000.0f;
	}
	if (rightBreakPt->ray.isMaxY)
	{
		rightEdge->start->pos.y = 1000.0f;
	}

	//TODO::Fixing Dual
	edges.push_back(leftEdge);
	edges.push_back(rightEdge);

	HalfEdge newRay;
	newRay.orig = vertex->pos;

	//Finding new Edge direction
	position2D dir = { leftArc->site->pos.x - rightArc->site->pos.x,
	leftArc->site->pos.y - rightArc->site->pos.y };

	position2D bisectorDir = normalize({ -dir.y, dir.x });

	newRay.dir = { bisectorDir.x, bisectorDir.y };

	//Fix the BeachLineBST
	//Becoz we need to delete 2 breakpoints and add one breakpoint with new edge
	//TODO: Do this

	/*
	Now because of the way we insert items, both edges/breakpoints must be a (grand)parent of the arc that we want to remove and in particular, one of them must be that arc’s immediate parent.
	Before we remove the arc’s parent, we need to first do something with it’s sibling. So we’ll put it in it’s parent’s place as one of it’s parent’s parent’s children. We can then safely remove the arc and parent edge from the tree, but there is another parent somewhere higher up the tree. This higher edge simply gets substituted with the new replacement edge, inheriting the same parent and children.
	*/

	//Finding out which breakpoint is an immediate parent
	BeachLineBSTNode *immediateParent = (toDeleteArc->GetParent() == leftBreakPt) ? leftBreakPt: rightBreakPt;
	BeachLineBSTNode *ancestorParent = (toDeleteArc->GetParent() != leftBreakPt) ? leftBreakPt : rightBreakPt; //Way up the hierarchy

	//Is Item we trying to delete is root then fix root(Here ancestor parent may be a root)

	//Fix Immediate Parent
	BeachLineBSTNode *sibling = (immediateParent->GetLeftChild() == toDeleteArc) ? immediateParent->GetRightChild(): immediateParent->GetLeftChild();

	if (immediateParent->GetParent()->GetLeftChild() == immediateParent)
	{
		immediateParent->GetParent()->SetLeftChild(sibling);
	}
	else
	{
		immediateParent->GetParent()->SetRightChild(sibling);
	}

	delete immediateParent;
	delete toDeleteArc;

	//Fix Ancestor Parent
	BeachLineBSTNode *newBreakPt = CreateBreakPointItem(leftArc->site, rightArc->site, newRay);
	if (ancestorParent->GetParent() == nullptr)
	{
		//ancesotor Parent is root
		root = newBreakPt;
	}
	else
	{
		bool left = (ancestorParent->GetParent()->GetLeftChild() == ancestorParent) ? true : false;
		newBreakPt->SetParent(ancestorParent->GetParent(), left);
	}

	newBreakPt->SetLeftChild(ancestorParent->GetLeftChild());
	newBreakPt->SetRightChild(ancestorParent->GetRightChild());

	delete ancestorParent;
	//Tree is Fixed Now

	//Check circle events again
	CheckAndAddCircleEvent(leftArc, eventPQ, directrixY);
	CheckAndAddCircleEvent(rightArc, eventPQ, directrixY);


	print2D(directrixY);
}

bool BeachLineBST::GetParabolaRayIntersection(HalfEdge &ray, Vertex *site, float directrixY, position2D &intersectionPt)
{
	//Input Ray is the ray from BreakPt
	//Input arc is one of the arcs from the tuple of Breakpt

	//Case: Edge is a vertical Line
	if (ray.dir.x == 0.0f)
	{
		//Arc is also vertical line(When sweep line is on directrix) 
		if (directrixY == site->pos.y)
		{
			if (ray.orig.x == site->pos.x)
			{
				intersectionPt = site->pos;
				return true;
			}
			else
			{
				return false;
			}
		}

		//Edge is vertical line
		//Just put it's x value in Parabola Eqn
		float arcY = GetArcYForXCoord(site, ray.orig.x, directrixY);
		intersectionPt = { ray.orig.x, arcY };
		return true;
	}

	//Line Equation => Point-Slope Form to Slope-Intercept Form
	//Ray is in Point-Slope Form

	//Slope Intercept Form
	//y = mx + c

	float m1 = ray.dir.y / ray.dir.x;
	float c1 = ray.orig.y - m1 * ray.orig.x;

	//Case: Arc is currently a vertical line(directrixY == arc.focus.y)
	if (site->pos.y == directrixY)
	{
		//Check if the intersection is in the same direction as edge
		if ((site->pos.x - ray.orig.x) * ray.dir.x < 0)
		{
			return false;
		}

		intersectionPt.x = site->pos.x;
		intersectionPt.y = m1 * site->pos.x + c1;
		return true;
	}

	//Equate Line and Parabola equation at intersection point
	// For the edge; y = m1 x + c1 where m is gradient, c is y intercept
	//For the parabola; y = a2 x ^ 2 + b2 x + c2

	//Find the parabola first
	float yf_d = 2.0f * (site->pos.y - directrixY);

	float a2 = 1.0f / yf_d;
	float b2 = -2.0f * site->pos.x / yf_d;
	float c2 = site->pos.x * site->pos.x / yf_d + (site->pos.y + directrixY) * 0.5f;

	//At Intersection  y = a2 x^2 + (b2 - m1)x + (c2 - c1) 

	float a = a2;
	float b = b2 - m1;
	float c = c2 - c1;

	float discriminant = b * b - 4.0f * a * c;

	if (discriminant < 0)
		return false;

	float sqDisc = std::sqrt(discriminant);

	float x1 = (-b + sqDisc) / (2.0f * a);
	float x2 = (-b - sqDisc) / (2.0f * a);

	//This gives us two locations where the line crosses the parabola. Remember we are not dealing with lines, but technically with rays, they have a starting point and a direction. In that case we only want one of these x values. The one I select depends on the direction of the edge, if it's pointing right we select the highest value, if it's pointing left we select the minimum value: 
	float x;

	float x1Offset = x1 - ray.orig.x;
	float x2Offset = x2 - ray.orig.x;
	float x1Dot = x1Offset * ray.dir.x;
	float x2Dot = x2Offset * ray.dir.x;

	if ((x1Dot >= 0.0f) && (x2Dot < 0.0f)) x = x1;
	else if ((x1Dot < 0.0f) && (x2Dot >= 0.0f)) x = x2;
	else if ((x1Dot >= 0.0f) && (x2Dot >= 0.0f))
	{
		if (x1Dot < x2Dot) x = x1;
		else x = x2;
	}
	else // (x1Dot < 0.0f) && (x2Dot < 0.0f)
	{
		if (x1Dot < x2Dot) x = x2;
		else x = x1;
	}

	float y = GetArcYForXCoord(site, x, directrixY);

	intersectionPt = { x, y };

	return true;
}

//Solved by Equating Parabola equation on both sides and making a quadratic equation
float BeachLineBST::GetXOfBreakPoint(BeachLineBSTNode * breakpoint, float directrixY)
{
	float breakPtX = 0.0f;

	position2D l = breakpoint->lSite->pos;
	position2D r = breakpoint->rSite->pos;

	//Parabola equation y = ax^2 + bx + c
	//First find both parabola equation coefficients
	float yf_d = 2.0f * (l.y - directrixY);

	float a1 = 1.0f / yf_d;
	float b1 = -2.0f * l.x / yf_d;
	float c1 = l.x * l.x / yf_d + (l.y + directrixY) * 0.5f;

	//Find 2nd parabola equation coefficients
	yf_d = 2.0f * (r.y - directrixY);
	float a2 = 1.0f / yf_d;
	float b2 = -2.0f * r.x / yf_d;
	float c2 = r.x * r.x / yf_d + (r.y + directrixY) * 0.5f;

	float a = a1 - a2;
	float b = b1 - b2;
	float c = c1 - c2;

	//2 parabolas of same orientation (like in out case upwards) can intersect at 2 points
	float discriminant = b * b - 4 * a * c;
	
	float x1 = (-b + std::sqrt(discriminant)) / (2*a);
	float x2 = (-b - std::sqrt(discriminant)) / (2*a);

	if (l.y < r.y)
	{
		breakPtX = std::max(x1, x2);
	}
	else
	{
		breakPtX = std::min(x1, x2);
	}

	return breakPtX;
}

void BeachLineBST::CheckAndAddCircleEvent(BeachLineBSTNode * item, EventPQ * eventPQ, float directrixY)
{
	BeachLineBSTNode *leftBreakPt = nullptr; 
	BeachLineBSTNode *rightBreakPt = nullptr;

	GetLeftAndRightBreakPoints(item, leftBreakPt, rightBreakPt);
	assert(leftBreakPt != rightBreakPt);

	//Check 0
	if (!leftBreakPt || !rightBreakPt)
		return;

	//Assert leftBreakPt->rSite == rightBreakPt->lSite

	//Check 1
	if (leftBreakPt->lSite == rightBreakPt->rSite)
		return;

	//Check 2

	position2D intersectionPt;

	if (CheckRayIntersection(leftBreakPt->ray, rightBreakPt->ray, intersectionPt))
	{
		//Intersection Point Found
		//Circle Event Detected
		//TODO: Discard Circle Events outside bounding box

		float radius = magnitude({ intersectionPt.x - item->site->pos.x, intersectionPt.y - item->site->pos.y });
		float circleEventY = intersectionPt.y - radius;
		////New Code
		//if(circleEventY > directrixY)
		//	return;
		////New

		////New 2
		//if (item->cEvent != nullptr)
		//{
		//	if (item->cEvent->point->pos.y >= circleEventY)
		//	{
		//		return;
		//	}
		//}
		////New 2

		assert(isfinite(circleEventY));
		Event* cEvent = new Event(new Vertex({ intersectionPt.x, circleEventY }), item, intersectionPt);
		eventPQ->push(cEvent);
		//Assert(item->cEvent is NULL)
		item->cEvent = cEvent;
	}
}

BeachLineBSTNode * BeachLineBST::GetArcByX(float x, float directrixY)
{
	BeachLineBSTNode* curr = root;

	while (!curr->isLeaf)
	{
		float breakptX = GetXOfBreakPoint(curr, directrixY);
		if (x >= breakptX) //Biased towards right
		{
			curr = curr->GetRightChild();
		}
		else
		{
			curr = curr->GetLeftChild();
		}
	}

	//Assert curr is leaf
	return curr;
}

BeachLineBSTNode * BeachLineBST::GetArcByX2(float x, float directrixY)
{
	BeachLineBSTNode* curr = root;

	while (!curr->isLeaf)
	{
		position2D breakpt;
		bool didLeftIntersect = GetParabolaRayIntersection(curr->ray, curr->lSite, directrixY, breakpt);
		if(!didLeftIntersect)
			bool didRightIntersect = GetParabolaRayIntersection(curr->ray, curr->rSite, directrixY, breakpt);
		
		if (x >= breakpt.x) //Biased towards right
		{
			curr = curr->GetRightChild();
		}
		else
		{
			curr = curr->GetLeftChild();
		}
	}

	//Assert curr is leaf
	return curr;
}

void BeachLineBST::print2DUtil(BeachLineBSTNode * curr, int space, float directrixY)
{
	// Base case  
	if (curr == nullptr)
		return;

	// Increase distance between levels  
	space += COUNT;

	// Process right child first  
	print2DUtil(curr->GetRightChild(), space, directrixY);

	// Print current node after space  
	// count  
	std::cout << std::endl;
	for (int i = COUNT; i < space; i++)
		std::cout << " ";
	if (!curr->isLeaf)
	{
		position2D breakpt;
		bool didLeftIntersect = GetParabolaRayIntersection(curr->ray, curr->lSite, directrixY, breakpt);
		if (!didLeftIntersect)
			bool didRightIntersect = GetParabolaRayIntersection(curr->ray, curr->rSite, directrixY, breakpt);

		std::cout << "(" << breakpt.x << ")"  << curr->lSite->pos << curr->rSite->pos << std::endl;
	}
	else
	{
		std::cout << "Arc: " << curr->site->siteID << std::endl;
	}
	// Process left child  
	print2DUtil(curr->GetLeftChild(), space, directrixY);
}

void BeachLineBST::print2D(float directrixY)
{
	print2DUtil(root, 0, directrixY);
	std::cout << std::endl;
	for(int i = 0; i < 3; i++)
		std::cout << "====================================================================================" << std::endl;
	std::cout << std::endl;
}

BeachLineBST::~BeachLineBST()
{
	for (auto &v : untrackedVertices)
	{
		delete v;
	}
}

BeachLineBSTNode* GetLeftArc(BeachLineBSTNode* curr)
{
	BeachLineBSTNode* leftArc = nullptr;
	//curr should be leaf/arc

	//Immediately Above Breakpt
	if (curr->GetParent() != nullptr && curr->GetParent()->GetRightChild() == curr)
	{
		leftArc = curr->GetParent()->GetLeftChild();
		
		while (!leftArc->isLeaf)
		{
			leftArc = leftArc->GetRightChild();
		}

		assert(leftArc->isLeaf);

		return leftArc;
	}

	//Go Up and find first parent who has the child as right
	while (curr->GetParent() != nullptr && curr->GetParent()->GetLeftChild() == curr)
	{
		curr = curr->GetParent();
	}

	curr = curr->GetParent();

	if (curr != nullptr)
	{
		if (curr->GetLeftChild() != nullptr)
		{
			//Go Right Till Leaf
			curr = curr->GetLeftChild();
			while (!curr->isLeaf)
			{
				curr = curr->GetRightChild();
			}
			leftArc = curr;
		}
	}

	assert(leftArc->isLeaf);

	return leftArc;
}

BeachLineBSTNode* GetRightArc(BeachLineBSTNode* curr)
{
	BeachLineBSTNode* rightArc = nullptr;
	//curr should be leaf/arc

	//Immediately Above Breakpt
	if (curr->GetParent() != nullptr && curr->GetParent()->GetLeftChild() == curr)
	{
		rightArc = curr->GetParent()->GetRightChild();
		
		while (!rightArc->isLeaf)
		{
			rightArc = rightArc->GetLeftChild();
		}
		
		assert(rightArc->isLeaf);
		return rightArc;
	}


	//Go Up and find first parent who has the child as left
	while (curr->GetParent() != nullptr && curr->GetParent()->GetRightChild() == curr)
	{
		curr = curr->GetParent();
	}

	curr = curr->GetParent();

	if (curr != nullptr)
	{
		if (curr->GetRightChild() != nullptr)
		{
			//Go Right Till Leaf
			curr = curr->GetRightChild();
			while (!curr->isLeaf)
			{
				curr = curr->GetLeftChild();
			}
			rightArc = curr;
		}
	}

	assert(rightArc->isLeaf);

	return rightArc;
}

void GetLeftAndRightBreakPoints(BeachLineBSTNode* curr, BeachLineBSTNode* &leftBreakPt, BeachLineBSTNode* &rightBreakPt)
{
	BeachLineBSTNode* temp = curr;
	leftBreakPt = nullptr;
	rightBreakPt = nullptr;
	//Assert Curr will be input as leaf

	if (curr->GetParent() != nullptr)
	{
		if (curr->GetParent()->GetLeftChild() == curr)
		{
			//Then the parent is the rightBreakPt
			rightBreakPt = curr->GetParent();
			curr = curr->GetParent();
			//Assert both of the breakpoints should not be a leaf
			//Now find the leftBreakPt //First right turn while climbing up the tree
			while (curr->GetParent() != nullptr && curr->GetParent()->GetLeftChild() == curr)
			{
				curr = curr->GetParent();
			}
			leftBreakPt = curr->GetParent();
		}
		else
		{
			//Then the parent is the leftBreakPt
			leftBreakPt = curr->GetParent();
			curr = curr->GetParent();
			//Assert Both of the break points should not be a leaf
			//Now find the rightBreakPt //First left turn while climbing up the tree
			while (curr->GetParent() != nullptr && curr->GetParent()->GetRightChild() == curr)
			{
				curr = curr->GetParent();
			}
			rightBreakPt = curr->GetParent();
		}
	}

	if (leftBreakPt)
		assert(leftBreakPt->rSite == temp->site);

	if (rightBreakPt)
		assert(rightBreakPt->lSite == temp->site);

	assert(leftBreakPt != rightBreakPt);
}

float GetArcYForXCoord(Vertex* site, float x, float directrixY)
{
	float a = 1.0f / (2.0f*(site->pos.y - directrixY));
	float c = (site->pos.y + directrixY)*0.5f;

	float w = x - site->pos.x;
	return a * w*w + c;
}

bool CheckRayIntersection(HalfEdge &rayA, HalfEdge &rayB, position2D &pos)
{
	position2D as = rayA.orig;
	position2D ad = { rayA.dir.x, rayA.dir.y };
	position2D bs = rayB.orig;
	position2D bd = { rayB.dir.x, rayB.dir.y };

	float dx = bs.x - as.x;
	float dy = bs.y - as.y;

	float det = bd.x * ad.y - bd.y * ad.x;

	//if (det == 0) //Rays do not intersect at all
	//	return false;

	float u = (dy * bd.x - dx * bd.y) / det;
	float v = (dy * ad.x - dx * ad.y) / det;

	if (u < 0 && !rayA.isMaxY)
		return false;
	if (v < 0 && !rayB.isMaxY)
		return false;

	if ((u == 0.0f) && (v == 0.0f) && !rayA.isMaxY && !rayB.isMaxY) return false;

	pos = { as.x + u * ad.x, as.y + u * ad.y };
	return true;
}