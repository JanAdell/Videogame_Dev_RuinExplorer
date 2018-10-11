#ifndef __j1Player_H__
#define __j1Player_H__

#include "j1Module.h"
#include "p2Point.h"
#include "Animation.h"
#include "SDL_mixer/include/SDL_mixer.h"
#pragma comment(lib, "SDL_mixer/libx86/SDL2_mixer.lib")

struct SDL_Texture;

class j1Player : public j1Module
{
public:
	j1Player();
	~j1Player();

	bool Awake(pugi::xml_node& conf);
	bool Start();
	bool Update(float dt);
	bool CleanUp();
	//void OnCollision(Collider* c1, Collider* c2);

public:
	//Mix_Chunk* player_death = nullptr;
	SDL_Texture* player_tex = nullptr;
	iPoint position;
	fPoint speed;

private:
	SDL_Rect sprite_player;
	j1Textures* tex_player = nullptr;
};

#endif
