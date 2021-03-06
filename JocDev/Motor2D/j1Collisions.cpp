#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Collisions.h"
#include "j1Player.h"
#include "p2Log.h"
#include "Brofiler/Brofiler.h"

j1Collisions::j1Collisions()
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		colliders[i] = nullptr;

	matrix[COLLIDER_WALL][COLLIDER_WALL] = false;
	matrix[COLLIDER_WALL][COLLIDER_FINISH] = true;
	matrix[COLLIDER_WALL][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_WALL][COLLIDER_BOOST] = true;
	matrix[COLLIDER_WALL][COLLIDER_LIMIT_CAMERA] = false;
	matrix[COLLIDER_WALL][COLLIDER_ENEMY] = true;

	matrix[COLLIDER_PLAYER][COLLIDER_WALL] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_FINISH] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_BOOST] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_LIMIT_CAMERA] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_ENEMY] = true;

	matrix[COLLIDER_BOOST][COLLIDER_WALL] = false;
	matrix[COLLIDER_BOOST][COLLIDER_FINISH] = false;
	matrix[COLLIDER_BOOST][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_BOOST][COLLIDER_BOOST] = false;
	matrix[COLLIDER_BOOST][COLLIDER_LIMIT_CAMERA] = false;
	matrix[COLLIDER_BOOST][COLLIDER_ENEMY] = false;

	matrix[COLLIDER_ENEMY][COLLIDER_WALL] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_FINISH] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_BOOST] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_LIMIT_CAMERA] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_ENEMY] = false;

	matrix[COLLIDER_FINISH][COLLIDER_WALL] = false;
	matrix[COLLIDER_FINISH][COLLIDER_FINISH] = false;
	matrix[COLLIDER_FINISH][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_FINISH][COLLIDER_BOOST] = false;
	matrix[COLLIDER_FINISH][COLLIDER_LIMIT_CAMERA] = false;
	matrix[COLLIDER_FINISH][COLLIDER_ENEMY] = false;
}

// Destructor
j1Collisions::~j1Collisions()
{}

bool j1Collisions::PreUpdate()
{
	BROFILER_CATEGORY("PreUpdateCollisions", Profiler::Color::Blue);

	// Remove all colliders scheduled for deletion
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr && colliders[i]->to_delete == true)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	// Calculate collisions
	Collider* c1;
	Collider* c2;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		// skip empty colliders
		if (colliders[i] == nullptr)
			continue;

		c1 = colliders[i];

		// avoid checking collisions already checked
		for (uint k = i + 1; k < MAX_COLLIDERS; ++k)
		{
			// skip empty colliders
			if (colliders[k] == nullptr)
				continue;

			c2 = colliders[k];

			if (c1->CheckCollision(c2->rect) == true)
			{
				if (matrix[c1->type][c2->type] && c1->callback)
					c1->callback->OnCollision(c1, c2);

				if (matrix[c2->type][c1->type] && c2->callback)
					c2->callback->OnCollision(c2, c1);
			}
		}
	}

	return true;
}

// Called before render is available
bool j1Collisions::Update(float dt)
{
	BROFILER_CATEGORY("UpdateCollisions", Profiler::Color::AliceBlue);


	DebugDraw();
	if (App->input->GetKey(SDL_SCANCODE_F10))
		god_mode = !god_mode;

	if (god_mode)
	{
		matrix[COLLIDER_PLAYER][COLLIDER_ENEMY] = false;		
		matrix[COLLIDER_ENEMY][COLLIDER_PLAYER] =false;

	}
	else
	{
		matrix[COLLIDER_PLAYER][COLLIDER_ENEMY] = true;
		matrix[COLLIDER_ENEMY][COLLIDER_PLAYER] = true;

	}

	return true;
}

void j1Collisions::DebugDraw()
{
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		debug = !debug;

	if (debug == false)
		return;

	Uint8 alpha = 80;
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
			continue;

		switch (colliders[i]->type)
		{
		case COLLIDER_NONE: 
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 255, alpha);
			break;
		case COLLIDER_WALL:
			App->render->DrawQuad(colliders[i]->rect, 0, 0, 255, alpha);
			break;
		case COLLIDER_FINISH:
			App->render->DrawQuad(colliders[i]->rect, 0, 0, 0, alpha);
			break;
		case COLLIDER_PLAYER:
			App->render->DrawQuad(colliders[i]->rect, 0, 255, 0, alpha);
			break;
		case COLLIDER_ENEMY:
			App->render->DrawQuad(colliders[i]->rect, 100, 100, 100, alpha);
			break;
		case COLLIDER_BOOST:
			App->render->DrawQuad(colliders[i]->rect, 255, 0, 0, alpha);
			break;
		}
	}
}

// Called before quitting
bool j1Collisions::CleanUp()
{
	LOG("Freeing all colliders");

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	return true;
}

Collider* j1Collisions::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback)
{
	Collider* ret = nullptr;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
		{
			ret = colliders[i] = new Collider(rect, type, callback);
			break;
		}
	}

	return ret;
}

// -----------------------------------------------------

bool Collider::CheckCollision(const SDL_Rect& r) const
{
	// TODO 0: Return true if there is an overlap
	return (rect.x < r.x + r.w &&
		rect.x + rect.w > r.x &&
		rect.y < r.y + r.h &&
		rect.h + rect.y > r.y);
}