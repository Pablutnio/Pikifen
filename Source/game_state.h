/*
 * Copyright (c) André 'Espyo' Silva 2013-2015.
 * The following source file belongs to the open-source project
 * Pikmin fangame engine. Please read the included README file
 * for more information.
 * Pikmin is copyright (c) Nintendo.
 *
 * === FILE DESCRIPTION ===
 * Header for the game state class and
 * game state-related functions.
 */

#ifndef GAME_STATE_INCLUDED
#define GAME_STATE_INCLUDED

#include <vector>

#include <allegro5/allegro.h>

#include "menu_widgets.h"

using namespace std;

enum GAME_STATES {
    GAME_STATE_MAIN_MENU,
    GAME_STATE_OPTIONS_MENU,
    GAME_STATE_GAME,
    GAME_STATE_AREA_EDITOR,
    GAME_STATE_ANIMATION_EDITOR,
};

class game_state{
protected:
    void set_selected_widget(menu_widget* widget);
    void handle_widget_events(ALLEGRO_EVENT ev);
    
public:
    vector<menu_widget*> menu_widgets;
    menu_widget* selected_widget;
    
    game_state();
    virtual void load() = 0;
    virtual void unload() = 0;
    virtual void handle_controls(ALLEGRO_EVENT ev) = 0;
    virtual void do_logic() = 0;
    virtual void do_drawing() = 0;
};


class gameplay : public game_state {
public:
    gameplay();
    virtual void load();
    virtual void unload();
    virtual void handle_controls(ALLEGRO_EVENT ev);
    virtual void do_logic();
    virtual void do_drawing();
};

#endif //ifndef GAME_STATE_INCLUDED