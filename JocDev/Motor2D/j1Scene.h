#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"

struct SDL_Texture;


class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	void death();
	void respawnEnemies();
	void respawnGUI();
	void GUImenu();
	void StartGameV();
	void StartGameT();
public:
	bool volcan_map = true;
	bool map_saved = true;
	bool die = false;
	uint deathsfx;
	bool changemenu = false;
	bool stayinmenu = true;
	bool do_load = false;
	bool go_out = false;
private:
	_Mix_Music* Stage_music = nullptr;
	SDL_Texture *menuBackground;
	SDL_Texture *menuBackgroundR;
	SDL_Rect rect,rectR;
	p2SString menu_file_name;
	p2SString menu_file_name2;
	iPoint life, scoin, bar, progresbar,pausepos;
	int buttonx, buttonxpause, button1, button2, button3, button4,buttonypause1,buttonypause2;
	
	

};

#endif // __j1SCENE_H__