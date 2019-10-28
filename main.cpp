#include <vld.h>

#include "Voronoi.h"

#include <raylib.h>
#include <vector>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

float flipY(float y)
{
	return SCREEN_HEIGHT - 1 - y;
}

Vector2 convertToScreenSpace(position2D pos)
{
	return { pos.x, (SCREEN_HEIGHT - 1 - pos.y) };
}

void DrawEdge(Edge *e)
{
	DrawLineV(convertToScreenSpace(e->start->pos), convertToScreenSpace(e->end->pos), BLUE);
}

void DrawSite(Vertex *s)
{
	float radius = 10.0f;
	DrawCircleV(convertToScreenSpace(s->pos), radius, RED);
}

void DrawVertex(Vertex *v)
{
	float radius = 5.0f;
	DrawCircleV(convertToScreenSpace(v->pos), radius, RED);
}

//Draws an arc of BST between a given Monotonic X Range
void DrawParabola(Vertex *site, float directrixY, float minX, float maxX)
{
	int i_minX = (int)minX;
	int i_maxX = (int)maxX;

	int n = i_maxX - i_minX + 1;

	if (n < 1)
		return;

	std::vector<position2D> curvePts(n, position2D());

	float x = minX;
	
	for (int i = 0; i < n; i++)
	{
		curvePts[i] = { x, GetArcYForXCoord(site, x, directrixY) };

		//curvePts[i].y = flipY(curvePts[i].y);

		x += 1.0f;
	}

	for (int i = 1; i < n; i++)
	{
		DrawLineV(convertToScreenSpace(curvePts[i - 1]), convertToScreenSpace(curvePts[i]), RED);
	}
}

void DrawRay(HalfEdge &ray, position2D &end)
{
	DrawLineV(convertToScreenSpace(ray.orig), convertToScreenSpace(end), BLACK);
}

void DrawBeachLineBST(BeachLineBSTNode *curr, float sweepLineY)
{
	if (!curr)
		return;

	//Arc
	if (curr->isLeaf)
	{
		BeachLineBSTNode *leftBreakPt = nullptr;
		BeachLineBSTNode *rightBreakPt = nullptr;
		GetLeftAndRightBreakPoints(curr, leftBreakPt, rightBreakPt);
		
		float minX = 0;
		float maxX = SCREEN_WIDTH;

		//leftBreakPt will be cached
		if (leftBreakPt)
		{
			minX = max(leftBreakPt->cachedBreakPointPos.x, minX);
		}

		//RightBreakPt pos has to be calculated
		if (rightBreakPt)
		{
			position2D intersectionPtThisFrame;
			GetParabolaRayIntersection(rightBreakPt->ray, curr->site, sweepLineY, intersectionPtThisFrame);
			rightBreakPt->cachedBreakPointPos = intersectionPtThisFrame;
			maxX = min(intersectionPtThisFrame.x, maxX);
		}

		DrawParabola(curr->site, sweepLineY, minX, maxX);
		return;
	}
	else//Edge
	{
		//Inorder Traversal
		DrawBeachLineBST(curr->GetLeftChild(), sweepLineY);

		//Use cached pt
		DrawRay(curr->ray, curr->cachedBreakPointPos);

		DrawBeachLineBST(curr->GetRightChild(), sweepLineY);
	}
}

void RenderVoronoi(VoronoiOutput &out, BeachLineBSTNode *root, float sweepLineY)
{
	//1: Draw Sites
	for (int i = 0; i < out.sites.size(); ++i)
	{
		DrawSite(out.sites[i]);
	}

	//2 : Draw Completed Edges
	for (int i = 0; i < out.edges.size(); ++i)
	{
		DrawEdge(out.edges[i]);
	}

	for (int i = 0; i < out.vertices.size(); ++i)
	{
		DrawVertex(out.vertices[i]);
	}

	//3: Draw BeachLine BST (Arcs, Rays under construction)
	DrawBeachLineBST(root, sweepLineY);
}

int main(int argc, char* argv[])
{
	std::vector<position2D> sites;

	int testCase = 0;

	int speedPerSec = -30.0f;//#Pixels Per Sec

	float sweepLineY = SCREEN_HEIGHT - 1;

	switch (testCase)
	{
	case 0:
		// Example case: The points shown in the GIF of Fortune's algorithm on wikipedia
		sites.push_back({ 155, 552 });
		sites.push_back({ 405, 552 });
		sites.push_back({ 624, 463 });
		sites.push_back({ 211, 419 });
		sites.push_back({ 458, 358 });
		sites.push_back({ 673, 299 });
		sites.push_back({ 261, 278 });
		sites.push_back({ 88, 196 });
		sites.push_back({ 497, 177 });
		sites.push_back({ 715, 118 });
		sites.push_back({ 275,  99 });
		break;
	case 1:
		// Test case 1: Points with equal x
		sites.push_back({ 300, 300 });
		sites.push_back({ 300, 400 });
		sites.push_back({ 400, 350 });
		break;

	case 2:
		// Test case 2: Points with equal y (where those points are not the first points)
		sites.push_back({ 300, 300 });
		sites.push_back({ 200, 200 });
		sites.push_back({ 400, 200 });
		break;

	case 3: //fix by FLT_MAX causes floating point issues
		// Test case 3: Points with equal y (where those points are the first points)
		//              With a third point that is slightly off to one side.
		//              Requires a special case for the first points to prevent errors in finding the replacedarc
		sites.push_back({ 320, 200 });
		sites.push_back({ 200, 300 });
		sites.push_back({ 400, 300 });
		break;

	case 31://CheckRayIntersection last case
		// Test case 3a: Points with equal y (where those points are the first points).
		//               With a third point that exactly lines up with the edge between the first 2.
		//               Requires the special case for edges that intersect at both of their starting points (they should not be counted as intersecting).
		sites.push_back({ 300, 200 });
		sites.push_back({ 200, 300 });
		sites.push_back({ 400, 300 });
		break;

	case 32: // Requires correct way to compare items in Event PQ
		// Test case 3b: 3 points with equal y (and nothing else)
		sites.push_back({ 300, 300 });
		sites.push_back({ 200, 300 });
		sites.push_back({ 400, 300 });
		sites.push_back({ 500, 300 });
		break;

	case 4:
		// Test case 4: A completely-surrounded site
		sites.push_back({ 100, 100 });
		sites.push_back({ 500, 150 });
		sites.push_back({ 300, 300 });
		sites.push_back({ 100, 550 });
		sites.push_back({ 500, 500 });
		break;

	case 5:
		// Test case 5: An arc gets squeezed by a later-created arc before it would be squeezed by its original edges.
		//              Requires handling of events that get "pre-empted" and are no longer required by the time they would execute.
		sites.push_back({ 300, 500 });
		sites.push_back({ 200, 450 });
		sites.push_back({ 400, 450 });
		sites.push_back({ 300, 400 });
		break;

	default:
		break;
	}

	
	/*for (auto e : out.edges)
	{
		if (e->start->pos.y == 1000.0f)
			std::cout << "F" << std::endl;
	}

	std::cout << "# SITES "<< out.sites.size() << std::endl;
	std::cout << "# VERTICES " << out.vertices.size() << std::endl;
	std::cout << "# EDGES "<< out.edges.size() << std::endl;
	*/
	
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Voronoi Output");

	SetTargetFPS(60);

	// Main game loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{

		Voronoi *vor = new Voronoi();

		//VoronoiOutput out;

		VoronoiOutput out = vor->generateVoronoi(sites, 0, SCREEN_WIDTH, 0, SCREEN_HEIGHT);

		BeginDrawing();

		ClearBackground(RAYWHITE);

		//Update Voronoi State

		//vor->VoronoiInteractive(sites, 0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, sweepLineY, out);

		//Use State/ Render Voronoi State
		//RenderVoronoi(out, vor->beachLine->root, sweepLineY);

		/*if (!vor->isEventQueueEmpty)
		{
			sweepLineY += speedPerSec * GetFrameTime();
		}

		if (!vor->isEventQueueEmpty)
		{
			DrawLineV(convertToScreenSpace({ 0, sweepLineY }), convertToScreenSpace({ SCREEN_WIDTH - 1, sweepLineY }), GREEN);
		}*/
		
		for (int i = 0; i < out.edges.size(); ++i)
		{
			DrawEdge(out.edges[i]);
		}		

		for (int i = 0; i < out.sites.size(); ++i)
		{
			DrawSite(out.sites[i]);
		}

		for (int i = 0; i < out.vertices.size(); ++i)
		{
			DrawVertex(out.vertices[i]);
		}


		EndDrawing();

		out.MemoryCleanUp();
		delete vor;

		//----------------------------------------------------------------------------------
	}

	// De-Initialization
	//--------------------------------------------------------------------------------------   
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	//Clean Up Memory

	system("pause");

	return 0;
}