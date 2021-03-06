#ifndef _GUI_H_
#define _GUI_H_


#include "p2Point.h"
#include "Animation.h"
#include "j1Audio.h"
#include "SDL/include/SDL.h"
#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )


struct SDL_Texture;
enum GUI_TYPES;
class GUI
{
public:
	GUI(int x, int y, GUI_TYPES type,GUI_TYPES subtype);
	~GUI();

	const iPoint GetPos() const;

	virtual void Update(float dt) = 0;
	void Draw(SDL_Texture* sprites,float dt);
	bool Load(pugi::xml_node &node);
	bool Save(pugi::xml_node &node) const;
	virtual bool GetPush() = 0;

protected:
	Animation * animation = nullptr;
	SDL_RendererFlip flip = SDL_RendererFlip::SDL_FLIP_NONE;
	float saveframe;
	bool colcoin;
	SDL_Texture* text_draw;
public:
	iPoint position;
	pugi::xml_document file;
	GUI_TYPES type,subtype;
	bool to_delete = false;
	uint collect;

};

#endif