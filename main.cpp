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
	DrawCircleV(convertToScreenSpace(v->pos), radius, GREEN);
}

int main(int argc, char* argv[])
{
	std::vector<position2D> sites;

	sites.push_back({ 155, 552 });
	//sites.push_back({ 405, 552 });
	sites.push_back({ 624, 463 });
	sites.push_back({ 211, 419 });
	sites.push_back({ 458, 358 });
	/*sites.push_back({ 673, 299 });
	sites.push_back({ 261, 278 });
	sites.push_back({ 88, 196 });
	sites.push_back({ 497, 177 });
	sites.push_back({ 715, 118 });
	sites.push_back({ 275,  99 });
*/
	Voronoi *vor = new Voronoi();

	VoronoiOutput out = vor->generateVoronoi(sites, 0, SCREEN_WIDTH, 0, SCREEN_HEIGHT);

	for (auto e : out.edges)
	{
		if (e->start->pos.y == FLT_MAX)
			std::cout << "F" << std::endl;
	}

	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Voronoi Output");

	SetTargetFPS(60);

	// Main game loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		// Update
		//----------------------------------------------------------------------------------
		// TODO: Update your variables here
		//----------------------------------------------------------------------------------

		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

		ClearBackground(RAYWHITE);

		for (int i = 0; i < out.edges.size(); ++i)
		{
			DrawEdge(out.edges[i]);
		}

		for (int i = 0; i < out.vertices.size(); ++i)
		{
			DrawVertex(out.vertices[i]);
		}

		for (int i = 0; i < out.sites.size(); ++i)
		{
			DrawSite(out.sites[i]);
		}

		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	// De-Initialization
	//--------------------------------------------------------------------------------------   
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}