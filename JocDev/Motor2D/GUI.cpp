#include "GUI.h"
#include "j1Render.h"
#include "j1App.h"
#include "j1GUI.h"


GUI::GUI(int x, int y, GUI_TYPES type) : position(x, y), type(type)
{
}

GUI::~GUI()
{

}

const iPoint GUI::GetPos() const
{
	return position;
}



void GUI::Draw(SDL_Texture * sprites)
{
	if (animation != nullptr)
	{
		if (type == GUI_TYPES::BUTTON)
		{
			App->render->Blit(sprites, position.x, position.y, flip, &(animation->GetCurrentFrame()));
		}
		if (type == GUI_TYPES::SPRITES)
		{
			App->render->Blit(sprites, position.x, position.y, flip, &(animation->GetCurrentFrame()),0.0f);
		}
	}
}