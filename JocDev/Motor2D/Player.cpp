#include "j1App.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Render.h"
#include "Player.h"
#include "j1Audio.h"
#include "j1Map.h"
#include "p2Log.h"
#include "j1Map.h"
#include "j1Entity.h"
#include "j1Collisions.h"
#include "p2Log.h"
#include "j1Scene.h"
#include <stdio.h>



Player::Player(int x, int y):Entity(x,y)
{
	pugi::xml_parse_result result = file.load_file("Entities.xml");

	if (result != NULL)
	{
		pugi::xml_node node = file.child("entities");

		speed.x = node.child("player").child("speed").attribute("x").as_float();
		speed.y = node.child("player").child("speed").attribute("y").as_float();

		player_size.x = node.child("player").child("idle_anim").attribute("w1").as_int();
		player_size.y = node.child("player").child("idle_anim").attribute("h1").as_int();

		gravity = node.child("player").child("speed").attribute("gravity").as_float();
		normal_jump = node.child("player").child("jump").attribute("normal").as_float();
		boosted_jump = node.child("player").child("jump").attribute("boost").as_float();

		pugi::xml_node anim = node.child("player");

		idle.PushBack({ anim.child("idle_anim").attribute("x1").as_int(),anim.child("idle_anim").attribute("y1").as_int(),anim.child("idle_anim").attribute("w1").as_int(),anim.child("idle_anim").attribute("h1").as_int() });
		idle.PushBack({ anim.child("idle_anim").attribute("x2").as_int(),anim.child("idle_anim").attribute("y2").as_int(),anim.child("idle_anim").attribute("w2").as_int(),anim.child("idle_anim").attribute("h2").as_int() });
		idle.PushBack({ anim.child("idle_anim").attribute("x3").as_int(),anim.child("idle_anim").attribute("y3").as_int(),anim.child("idle_anim").attribute("w3").as_int(),anim.child("idle_anim").attribute("h3").as_int() });
		idle.PushBack({ anim.child("idle_anim").attribute("x4").as_int(),anim.child("idle_anim").attribute("y4").as_int(),anim.child("idle_anim").attribute("w4").as_int(),anim.child("idle_anim").attribute("h4").as_int() });
		idle.loop = true;
		idle.speed = anim.child("idle_anim").attribute("speed").as_float();

		run.PushBack({ anim.child("run_anim").attribute("x1").as_int(),anim.child("run_anim").attribute("y1").as_int(),anim.child("run_anim").attribute("w1").as_int(),anim.child("run_anim").attribute("h1").as_int() });
		run.PushBack({ anim.child("run_anim").attribute("x2").as_int(),anim.child("run_anim").attribute("y2").as_int(),anim.child("run_anim").attribute("w2").as_int(),anim.child("run_anim").attribute("h2").as_int() });
		run.PushBack({ anim.child("run_anim").attribute("x3").as_int(),anim.child("run_anim").attribute("y3").as_int(),anim.child("run_anim").attribute("w3").as_int(),anim.child("run_anim").attribute("h3").as_int() });
		run.PushBack({ anim.child("run_anim").attribute("x4").as_int(),anim.child("run_anim").attribute("y4").as_int(),anim.child("run_anim").attribute("w4").as_int(),anim.child("run_anim").attribute("h4").as_int() });
		run.PushBack({ anim.child("run_anim").attribute("x5").as_int(),anim.child("run_anim").attribute("y5").as_int(),anim.child("run_anim").attribute("w5").as_int(),anim.child("run_anim").attribute("h5").as_int() });
		run.PushBack({ anim.child("run_anim").attribute("x6").as_int(),anim.child("run_anim").attribute("y6").as_int(),anim.child("run_anim").attribute("w6").as_int(),anim.child("run_anim").attribute("h6").as_int() });
		run.loop = true;
		run.speed = anim.child("run_anim").attribute("speed").as_float();

		jump_anim.PushBack({ anim.child("jump_anim").attribute("x1").as_int(),anim.child("jump_anim").attribute("y1").as_int(),anim.child("jump_anim").attribute("w1").as_int(),anim.child("jump_anim").attribute("h1").as_int() });
		jump_anim.PushBack({ anim.child("jump_anim").attribute("x2").as_int(),anim.child("jump_anim").attribute("y2").as_int(),anim.child("jump_anim").attribute("w2").as_int(),anim.child("jump_anim").attribute("h2").as_int() });
		jump_anim.loop = false;
		jump_anim.speed = anim.child("jump_anim").attribute("speed").as_float();

		//fall
		fall.PushBack({ anim.child("fall_anim").attribute("x1").as_int(),anim.child("fall_anim").attribute("y1").as_int(),anim.child("fall_anim").attribute("w1").as_int(),anim.child("fall_anim").attribute("h1").as_int() });
		fall.PushBack({ anim.child("fall_anim").attribute("x2").as_int(),anim.child("fall_anim").attribute("y2").as_int(),anim.child("fall_anim").attribute("w2").as_int(),anim.child("fall_anim").attribute("h2").as_int() });
		fall.loop = false;
		fall.speed = anim.child("fall_anim").attribute("speed").as_float();

		collider = App->collisions->AddCollider({ position.x,position.y,player_size.x,player_size.y - 5 }, COLLIDER_PLAYER, (j1Module*)App->entities);
		entityflip = SDL_RendererFlip::SDL_FLIP_NONE;
		
		top_jump = true;

		App->audio->LoadFx("audio/fx/Teleport.wav");
		App->audio->LoadFx("audio/fx/Death.wav");
	}
}

Player::~Player()
{}


bool Player::CleanUp()
{
	LOG("Unloading player");
	App->audio->UnloadFx(1);
	App->audio->UnloadFx(2);
	App->tex->UnLoad(player_tex);
	App->tex->UnLoad(teleport_tex);
	player_tex = nullptr;
	teleport_tex = nullptr;
	return true;

}


// Update: draw background
void Player::Update(float dt)
{

	//pos_collidery = position.y + 30;
	current_animation = &idle;

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		current_animation = &run;
		App->entities->enemyflip = SDL_RendererFlip::SDL_FLIP_NONE;
		position.x += speed.x;
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		current_animation = &run;
		App->entities->enemyflip = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
		position.x -= speed.x;
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		top_jump = true;
		attack = true;
	}
	//jump
	if (stay_in_platform)
	{
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
		{
			attack = false;
			start_jump = false;
			if (start_jump == false)
			{
				jump_anim.Reset();
				distance_to_jump = position.y - normal_jump;
				position.y -= speed.y + gravity;
				start_jump = true;
				stay_in_platform = false;
				top_jump = false;
			}
		}
	}
	else
	{
		if (position.y > distance_to_jump && top_jump == false)
		{
			current_animation = &jump_anim;
			position.y -= speed.y + gravity;
		}
		if (position.y == distance_to_jump)
			top_jump = true;
		if (top_jump == true)
		{
			current_animation = &fall;
			if (attack = true)
				position.y += gravity;
		}
	}
	position.y += gravity;
	stay_in_platform = false;

	//tp mechanic
	if (App->scene->volcan_map)
	{

		if (position.x > App->map->data.tile_width * App->map->data.width - 7 * App->map->data.tile_width)
		{
			teleport.Reset();
			App->audio->PlayFx(2, 0);
			position.x = 7 * App->map->data.tile_width;
			current_animation = &teleport;
		}

		else if (position.x < 7 * App->map->data.tile_width)
		{
			teleport.Reset();
			position.x = App->map->data.tile_width * App->map->data.width - 7 * App->map->data.tile_width;
			App->audio->PlayFx(2, 0);
			current_animation = &teleport;
		}
	}
	else
	{
		if (position.x > App->map->data.tile_width * App->map->data.width - 8 * App->map->data.tile_width)
		{
			teleport.Reset();
			App->audio->PlayFx(2, 0);
			position.x = 8 * App->map->data.tile_width;
			current_animation = &teleport;
		}

		else if (position.x < 8 * App->map->data.tile_width)
		{
			teleport.Reset();
			position.x = App->map->data.tile_width * App->map->data.width - 8 * App->map->data.tile_width;
			App->audio->PlayFx(2, 0);
			current_animation = &teleport;
		}
	}

	//colliders player
	collider->SetPos(position.x, position.y);

	//cameralimit->SetPos(App->render->camera.x, -App->render->camera.y);

}

void Player::OnCollision(Collider* collider)
{
	if (collider->type == COLLIDER_WALL)
	{
		if(position.x <= collider->SetPos.rect.x + collider->SetPos.rect.w)
		position.x += speed.x;
		if (position.x + player_size.x >= collider->SetPos.rect.x)
			position.x -= speed.x;
		if (position.y <= collider->SetPos.rect.y + collider->SetPos.rect.h)
		{
			stay_in_platform = true;
			position.y -= gravity;
		}
		if (position.y + player_size.y >= collider->SetPos.rect.y)
			top_jump = true;
	}
	if (collider->type == COLLIDER_BOOST)
	{
		if (stay_in_platform == true && App->input->GetKey(SDL_SCANCODE_SPACE))
		{
			start_jump = false;
			jump_anim.Reset();
			distance_to_jump = position.y - boosted_jump;
			position.y -= speed.y;
			start_jump = true;
			stay_in_platform = false;
			top_jump = false;
		}
	}
	if (collider->type == COLLIDER_ENEMY)
	{
		if (position.y <= collider->SetPos.rect.y + collider->SetPos.rect.h && !attack)
		{
			App->scene->death();
		}
	}

}


bool Player::Load(pugi::xml_node& data)
{
	position.x = data.child("player").attribute("x").as_int();
	position.y = data.child("player").attribute("y").as_int();

	return true;
}

// Save Game State
bool Player::Save(pugi::xml_node& data) const
{
	pugi::xml_node pos = data.append_child("player");

	pos.append_attribute("x") = position.x;
	pos.append_attribute("y") = position.y;

	return true;
}
