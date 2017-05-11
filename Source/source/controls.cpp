/*
 * Copyright (c) Andre 'Espyo' Silva 2013-2017.
 * The following source file belongs to the open-source project
 * Pikmin fangame engine. Please read the included
 * README and LICENSE files for more information.
 * Pikmin is copyright (c) Nintendo.
 *
 * === FILE DESCRIPTION ===
 * Control-related functions.
 */

#include <algorithm>
#include <iostream>
#include <typeinfo>

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>

#include "const.h"
#include "controls.h"
#include "drawing.h"
#include "functions.h"
#include "vars.h"

/* ----------------------------------------------------------------------------
 * Handles an Allegro event related to hardware input,
 * and triggers the corresponding controls, if any.
 */
void gameplay::handle_controls(const ALLEGRO_EVENT &ev) {
    if(ev.type == ALLEGRO_EVENT_KEY_CHAR) {
        if(ev.keyboard.keycode == ALLEGRO_KEY_T) {
        
            //Debug testing.
            //TODO remove.
            
            
        } else if(ev.keyboard.keycode == ALLEGRO_KEY_F1) {
        
            show_framerate = !show_framerate;
            
        } else if(
            (
                ev.keyboard.keycode >= ALLEGRO_KEY_F2 &&
                ev.keyboard.keycode <= ALLEGRO_KEY_F11
            ) || (
                ev.keyboard.keycode >= ALLEGRO_KEY_0 &&
                ev.keyboard.keycode <= ALLEGRO_KEY_9
            )
        ) {
        
            unsigned char id;
            if(
                ev.keyboard.keycode >= ALLEGRO_KEY_F2 &&
                ev.keyboard.keycode <= ALLEGRO_KEY_F11
            ) {
                //The first ten indexes are the F2 - F11 keys.
                id = dev_tool_keys[ev.keyboard.keycode - ALLEGRO_KEY_F2];
            } else {
                //The last ten indexes are the 0 - 9 keys.
                id = dev_tool_keys[10 + (ev.keyboard.keycode - ALLEGRO_KEY_0)];
            }
            
            if(id == DEV_TOOL_AREA_IMAGE) {
                ALLEGRO_BITMAP* bmp = draw_to_bitmap();
                if(!al_save_bitmap(dev_tool_area_image_name.c_str(), bmp)) {
                    log_error(
                        "Could not save the area onto an image,"
                        " with the name \"" +
                        dev_tool_area_image_name + "\"!"
                    );
                }
                
            } else if(id == DEV_TOOL_CHANGE_SPEED) {
                dev_tool_change_speed = !dev_tool_change_speed;
                
            } else if(id == DEV_TOOL_GEOMETRY_INFO) {
                dev_tool_geometry_info = !dev_tool_geometry_info;
                
            } else if(id == DEV_TOOL_HURT_MOB) {
                mob* m = get_closest_mob_to_cursor();
                if(m) {
                    m->health = m->health - m->type->max_health * 0.2;
                }
                
            } else if(id == DEV_TOOL_MOB_INFO) {
                mob* m = get_closest_mob_to_cursor();
                dev_tool_info_lock = (dev_tool_info_lock == m ? NULL : m);
                
            } else if(id == DEV_TOOL_NEW_PIKMIN) {
                if(pikmin_list.size() < max_pikmin_in_field) {
                    pikmin_type* new_pikmin_type = pikmin_types.begin()->second;
                    
                    auto p = pikmin_types.begin();
                    for(; p != pikmin_types.end(); ++p) {
                        if(p->second == dev_tool_last_pikmin_type) {
                            ++p;
                            if(p != pikmin_types.end()) {
                                new_pikmin_type = p->second;
                            }
                            break;
                        }
                    }
                    dev_tool_last_pikmin_type = new_pikmin_type;
                    
                    create_mob(
                        mob_categories.get(MOB_CATEGORY_PIKMIN),
                        mouse_cursor_w, new_pikmin_type, 0, "maturity=flower"
                    );
                }
                
            } else if(id == DEV_TOOL_TELEPORT) {
                cur_leader_ptr->chase(mouse_cursor_w, NULL, true);
                
            }
            
        }
    }
    
    
    for(size_t p = 0; p < MAX_PLAYERS; p++) {
        size_t n_controls = controls[p].size();
        for(size_t c = 0; c < n_controls; ++c) {
        
            control_info* con = &controls[p][c];
            
            if(
                con->type == CONTROL_TYPE_KEYBOARD_KEY &&
                (
                    ev.type == ALLEGRO_EVENT_KEY_DOWN ||
                    ev.type == ALLEGRO_EVENT_KEY_UP
                )
            ) {
                if(con->button == ev.keyboard.keycode) {
                    handle_button(
                        con->action, p,
                        (ev.type == ALLEGRO_EVENT_KEY_DOWN) ? 1 : 0
                    );
                }
                
            } else if(
                con->type == CONTROL_TYPE_MOUSE_BUTTON &&
                (
                    ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN ||
                    ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP
                )
            ) {
                if(con->button == (signed) ev.mouse.button) {
                    handle_button(
                        con->action, p,
                        (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) ? 1 : 0
                    );
                }
                
            } else if(
                con->type == CONTROL_TYPE_MOUSE_WHEEL_UP &&
                ev.type == ALLEGRO_EVENT_MOUSE_AXES
            ) {
                if(ev.mouse.dz > 0) {
                    handle_button(con->action, p, ev.mouse.dz);
                }
                
            } else if(
                con->type == CONTROL_TYPE_MOUSE_WHEEL_DOWN &&
                ev.type == ALLEGRO_EVENT_MOUSE_AXES
            ) {
                if(ev.mouse.dz < 0) {
                    handle_button(con->action, p, -ev.mouse.dz);
                }
                
            } else if(
                con->type == CONTROL_TYPE_MOUSE_WHEEL_LEFT &&
                ev.type == ALLEGRO_EVENT_MOUSE_AXES
            ) {
                if(ev.mouse.dw < 0) {
                    handle_button(con->action, p, -ev.mouse.dw);
                }
                
            } else if(
                con->type == CONTROL_TYPE_MOUSE_WHEEL_RIGHT &&
                ev.type == ALLEGRO_EVENT_MOUSE_AXES
            ) {
                if(ev.mouse.dw > 0) {
                    handle_button(con->action, p, ev.mouse.dw);
                }
                
            } else if(
                con->type == CONTROL_TYPE_JOYSTICK_BUTTON &&
                (
                    ev.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN ||
                    ev.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_UP
                )
            ) {
                if(
                    con->device_nr == joystick_numbers[ev.joystick.id] &&
                    (signed) con->button == ev.joystick.button
                ) {
                    handle_button(
                        con->action, p,
                        (ev.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN) ? 1 : 0
                    );
                }
                
            } else if(
                con->type == CONTROL_TYPE_JOYSTICK_AXIS_POS &&
                ev.type == ALLEGRO_EVENT_JOYSTICK_AXIS
            ) {
                if(
                    con->device_nr == joystick_numbers[ev.joystick.id] &&
                    con->stick == ev.joystick.stick &&
                    con->axis == ev.joystick.axis && ev.joystick.pos >= 0
                ) {
                    handle_button(con->action, p, ev.joystick.pos);
                }
                
            } else if(
                con->type == CONTROL_TYPE_JOYSTICK_AXIS_NEG &&
                ev.type == ALLEGRO_EVENT_JOYSTICK_AXIS
            ) {
                if(
                    con->device_nr == joystick_numbers[ev.joystick.id] &&
                    con->stick == ev.joystick.stick &&
                    con->axis == ev.joystick.axis && ev.joystick.pos <= 0) {
                    handle_button(con->action, p, -ev.joystick.pos);
                }
            }
        }
        
        if(ev.type == ALLEGRO_EVENT_MOUSE_AXES && mouse_moves_cursor[p]) {
            mouse_cursor_s.x = ev.mouse.x;
            mouse_cursor_s.y = ev.mouse.y;
            mouse_cursor_w = mouse_cursor_s;
            
            al_transform_coordinates(
                &screen_to_world_transform,
                &mouse_cursor_w.x, &mouse_cursor_w.y
            );
        }
    }
    
}


/* ----------------------------------------------------------------------------
 * Handles a button "press". Technically, it could also be a button release.
 * button: The button's ID. Use BUTTON_*.
 * pos:    The position of the button, i.e., how much it's "held".
 *   0 means it was released. 1 means it was fully pressed.
 *   For controls with more sensibility, values between 0 and 1 are important.
 *   Like a 0.5 for the group movement makes it move at half distance.
 */
void handle_button(
    const unsigned int button, const unsigned char player, float pos
) {

    if(!ready_for_input) return;
    
    if(cur_message.empty()) {
    
        if(
            button == BUTTON_RIGHT ||
            button == BUTTON_UP ||
            button == BUTTON_LEFT ||
            button == BUTTON_DOWN
        ) {
        
            /*******************
            *               O_ *
            *   Move   --->/|  *
            *              V > *
            *******************/
            
            if(pos != 0) active_control();
            
            if(     button == BUTTON_RIGHT) leader_movement.right = pos;
            else if(button == BUTTON_LEFT)  leader_movement.left =  pos;
            else if(button == BUTTON_UP)    leader_movement.up =    pos;
            else if(button == BUTTON_DOWN)  leader_movement.down =  pos;
            
        } else if(
            button == BUTTON_CURSOR_RIGHT ||
            button == BUTTON_CURSOR_UP ||
            button == BUTTON_CURSOR_LEFT ||
            button == BUTTON_CURSOR_DOWN
        ) {
            /********************
            *             .-.   *
            *   Cursor   ( = )> *
            *             '-'   *
            ********************/
            
            if(button == BUTTON_CURSOR_RIGHT) {
                cursor_movement.right = pos;
            } else if(button == BUTTON_CURSOR_LEFT) {
                cursor_movement.left = pos;
            } else if(button == BUTTON_CURSOR_UP) {
                cursor_movement.up = pos;
            } else if(button == BUTTON_CURSOR_DOWN) {
                cursor_movement.down = pos;
            }
            
        } else if(
            button == BUTTON_GROUP_RIGHT ||
            button == BUTTON_GROUP_UP ||
            button == BUTTON_GROUP_LEFT ||
            button == BUTTON_GROUP_DOWN
        ) {
            /******************
            *            ***  *
            *   Group   ****O *
            *            ***  *
            ******************/
            
            active_control();
            
            if(button == BUTTON_GROUP_RIGHT) {
                group_movement.right = pos;
            } else if(button == BUTTON_GROUP_LEFT) {
                group_movement.left = pos;
            } else if(button == BUTTON_GROUP_UP) {
                group_movement.up = pos;
            } else if(button == BUTTON_GROUP_DOWN) {
                group_movement.down = pos;
            }
            
            if(group_movement.get_intensity() != 0) {
                cur_leader_ptr->signal_group_move_start();
            } else {
                cur_leader_ptr->signal_group_move_end();
            }
            
        } else if(button == BUTTON_GROUP_CURSOR) {
        
            active_control();
            
            if(pos > 0) {
                group_move_cursor = true;
                group_move_intensity = 1;
                cur_leader_ptr->signal_group_move_start();
            } else {
                group_move_cursor = false;
                group_move_intensity = 0;
                cur_leader_ptr->signal_group_move_end();
            }
            
        } else if(button == BUTTON_THROW) {
        
            /*******************
            *             .-.  *
            *   Throw    /   O *
            *           &      *
            *******************/
            
            if(pos > 0) { //Button press.
            
                active_control();
                
                bool done = false;
                
                //First check if the leader should pluck a Pikmin.
                dist d;
                pikmin* p =
                    get_closest_buried_pikmin(
                        cur_leader_ptr->pos, &d, false
                    );
                if(p && d <= pluck_range) {
                    cur_leader_ptr->fsm.run_event(
                        LEADER_EVENT_GO_PLUCK, (void*) p
                    );
                    done = true;
                }
                
                //Now check if the leader should read an info spot.
                if(!done) {
                    size_t n_info_spots = info_spots.size();
                    for(size_t i = 0; i < n_info_spots; ++i) {
                        info_spot* i_ptr = info_spots[i];
                        if(i_ptr->opens_box) {
                            if(
                                dist(cur_leader_ptr->pos, i_ptr->pos) <=
                                info_spot_trigger_range
                            ) {
                                start_message(i_ptr->text, NULL);
                                done = true;
                                break;
                            }
                        }
                    }
                }
                
                //Now check if the leader should open an Onion's menu.
                if(!done) {
                    size_t n_onions = onions.size();
                    for(size_t o = 0; o < n_onions; ++o) {
                        if(
                            dist(cur_leader_ptr->pos, onions[o]->pos) <=
                            onion_open_range
                        ) {
                            pikmin_type* pik_type =
                                onions[o]->oni_type->pik_type;
                            if(
                                pikmin_list.size() < max_pikmin_in_field &&
                                pikmin_in_onions[pik_type] > 0
                            ) {
                                pikmin_in_onions[pik_type]--;
                                mob* new_pik =
                                    create_mob(
                                        mob_categories.get(MOB_CATEGORY_PIKMIN),
                                        onions[o]->pos, pik_type, 0, ""
                                    );
                            }
                            done = true;
                        }
                    }
                }
                
                //Now check if the leader should heal themselves on the ship.
                if(!done) {
                    size_t n_ships = ships.size();
                    for(size_t s = 0; s < n_ships; ++s) {
                        if(
                            ships[s]->shi_type->can_heal &&
                            dist(
                                cur_leader_ptr->pos, ships[s]->beam_final_pos
                            ) <= ships[s]->shi_type->beam_radius
                        ) {
                            cur_leader_ptr->health =
                                cur_leader_ptr->type->max_health;
                            done = true;
                        }
                    }
                }
                
                //Now check if the leader should grab a Pikmin.
                if(!done) {
                    if(
                        !cur_leader_ptr->holding_pikmin &&
                        cur_leader_ptr->group->cur_standby_type &&
                        !closest_group_member_distant
                    ) {
                    
                        done = grab_closest_group_member();
                    }
                }
                
                //Now check if the leader should punch.
                if(!done) {
                }
                
            } else { //Button release.
                mob* holding_ptr = cur_leader_ptr->holding_pikmin;
                if(holding_ptr) {
                    cur_leader_ptr->fsm.run_event(LEADER_EVENT_THROW);
                }
            }
            
        } else if(button == BUTTON_WHISTLE) {
        
            /********************
            *              .--= *
            *   Whistle   ( @ ) *
            *              '-'  *
            ********************/
            
            active_control();
            
            if(pos > 0) {
                //Button pressed.
                cur_leader_ptr->fsm.run_event(LEADER_EVENT_START_WHISTLE);
                
            } else {
                //Button released.
                cur_leader_ptr->fsm.run_event(LEADER_EVENT_STOP_WHISTLE);
                
            }
            
        } else if(
            button == BUTTON_NEXT_LEADER ||
            button == BUTTON_PREV_LEADER
        ) {
        
            /******************************
            *                    \O/  \O/ *
            *   Switch leader     | -> |  *
            *                    / \  / \ *
            ******************************/
            
            if(pos == 0 || leaders.size() == 1) return;
            
            size_t new_leader_nr = cur_leader_nr;
            leader* new_leader_ptr = nullptr;
            bool search_new_leader = true;
            
            if(!cur_leader_ptr->fsm.get_event(LEADER_EVENT_UNFOCUSED)) {
                //This leader isn't ready to be switched out of. Forget it.
                return;
            }
            
            //We'll send the switch event to the next leader on the list.
            //If they accept, they run a function to change leaders.
            //If not, we try the next leader.
            //If we return to the current leader without anything being
            //changed, then stop trying; no leader can be switched to.
            
            size_t original_leader_nr = cur_leader_nr;
            
            while(search_new_leader) {
                new_leader_nr =
                    sum_and_wrap(
                        new_leader_nr,
                        (button == BUTTON_NEXT_LEADER ? 1 : -1),
                        leaders.size()
                    );
                new_leader_ptr = leaders[new_leader_nr];
                
                if(new_leader_nr == original_leader_nr) {
                    //Back to the original; stop trying.
                    return;
                }
                
                new_leader_ptr->fsm.run_event(LEADER_EVENT_FOCUSED);
                
                //If after we called the event, the leader is the same,
                //then that means the leader can't be switched to.
                //Try a new one.
                if(cur_leader_nr != original_leader_nr) {
                    search_new_leader = false;
                }
            }
            
        } else if(button == BUTTON_DISMISS) {
        
            /***********************
            *             \O/ / *  *
            *   Dismiss    |   - * *
            *             / \ \ *  *
            ***********************/
            
            if(pos == 0 || cur_leader_ptr->holding_pikmin) return;
            
            active_control();
            
            cur_leader_ptr->fsm.run_event(LEADER_EVENT_DISMISS);
            
        } else if(button == BUTTON_PAUSE) {
        
            /********************
            *           +-+ +-+ *
            *   Pause   | | | | *
            *           +-+ +-+ *
            ********************/
            
            if(pos == 0) return;
            
            is_game_running = false;
            //paused = true;
            
        } else if(button == BUTTON_USE_SPRAY_1) {
        
            /*******************
            *             +=== *
            *   Sprays   (   ) *
            *             '-'  *
            *******************/
            
            if(pos == 0 || cur_leader_ptr->holding_pikmin) return;
            
            active_control();
            
            if(spray_types.size() == 1 || spray_types.size() == 2) {
                size_t spray_nr = 0;
                cur_leader_ptr->fsm.run_event(
                    LEADER_EVENT_SPRAY, (void*) &spray_nr
                );
            }
            
        } else if(button == BUTTON_USE_SPRAY_2) {
        
            if(pos == 0 || cur_leader_ptr->holding_pikmin) return;
            
            active_control();
            
            if(spray_types.size() == 2) {
                size_t spray_nr = 1;
                cur_leader_ptr->fsm.run_event(
                    LEADER_EVENT_SPRAY, (void*) &spray_nr
                );
            }
            
        } else if(
            button == BUTTON_NEXT_SPRAY ||
            button == BUTTON_PREV_SPRAY
        ) {
        
            if(pos == 0 || cur_leader_ptr->holding_pikmin) return;
            
            if(spray_types.size() > 2) {
                if(button == BUTTON_NEXT_SPRAY) {
                    selected_spray = (selected_spray + 1) % spray_types.size();
                } else {
                    if(selected_spray == 0) {
                        selected_spray = spray_types.size() - 1;
                    } else {
                        selected_spray--;
                    }
                }
            }
            
        } else if(button == BUTTON_USE_SPRAY) {
        
            if(pos == 0 || cur_leader_ptr->holding_pikmin) return;
            
            active_control();
            
            if(spray_types.size() > 2) {
                cur_leader_ptr->fsm.run_event(
                    LEADER_EVENT_SPRAY,
                    (void*) &selected_spray
                );
            }
            
        } else if(button == BUTTON_CHANGE_ZOOM) {
        
            /***************
            *           _  *
            *   Zoom   (_) *
            *          /   *
            ***************/
            
            if(pos == 0) return;
            
            if(cam_final_zoom < zoom_mid_level) {
                cam_final_zoom = zoom_max_level;
            } else if(cam_final_zoom > zoom_mid_level) {
                cam_final_zoom = zoom_mid_level;
            } else {
                if(zoom_mid_level == zoom_min_level) {
                    cam_final_zoom = zoom_max_level;
                } else {
                    cam_final_zoom = zoom_min_level;
                }
            }
            
            sfx_camera.play(0, false);
            
        } else if(button == BUTTON_ZOOM_IN || button == BUTTON_ZOOM_OUT) {
        
            if(
                (
                    cam_final_zoom >= zoom_max_level &&
                    button == BUTTON_ZOOM_IN
                ) ||
                (
                    cam_final_zoom <= zoom_min_level &&
                    button == BUTTON_ZOOM_OUT
                )
            ) {
                return;
            }
            
            pos = floor(pos);
            
            if(button == BUTTON_ZOOM_IN) {
                cam_final_zoom = cam_final_zoom + 0.1 * pos;
            } else {
                cam_final_zoom = cam_final_zoom - 0.1 * pos;
            }
            
            if(cam_final_zoom > zoom_max_level) cam_final_zoom = zoom_max_level;
            if(cam_final_zoom < zoom_min_level) cam_final_zoom = zoom_min_level;
            
            sfx_camera.play(-1, false);
            
        } else if(button == BUTTON_LIE_DOWN) {
        
            /**********************
            *                     *
            *   Lie down  -()/__/ *
            *                     *
            **********************/
            
            if(pos == 0) return;
            
            cur_leader_ptr->fsm.run_event(LEADER_EVENT_LIE_DOWN);
            
            
        } else if(
            button == BUTTON_NEXT_TYPE ||
            button == BUTTON_PREV_TYPE
        ) {
        
            /****************************
            *                     -->   *
            *   Switch type   <( )> (o) *
            *                           *
            ****************************/
            
            if(pos == 0) return;
            
            active_control();
            
            if(cur_leader_ptr->group->members.empty()) return;
            
            subgroup_type* starting_subgroup_type =
                cur_leader_ptr->group->cur_standby_type;
                
            bool switch_successful;
            
            if(!cur_leader_ptr->holding_pikmin) {
                //If the leader isn't holding anybody.
                switch_successful =
                    cur_leader_ptr->group->set_next_cur_standby_type(
                        button == BUTTON_PREV_TYPE
                    );
                    
            } else {
                //If the leader is holding a Pikmin, we can't let it
                //swap to a Pikmin that's far away.
                //So, every time that happens, skip that subgroup and
                //try the next. Also, make sure to cancel everything if
                //the loop already went through all types.
                
                bool finish = false;
                do {
                    switch_successful =
                        cur_leader_ptr->group->set_next_cur_standby_type(
                            button == BUTTON_PREV_TYPE
                        );
                        
                    if(
                        !switch_successful ||
                        cur_leader_ptr->group->cur_standby_type ==
                        starting_subgroup_type
                    ) {
                        //Reached around back to the first subgroup...
                        switch_successful = false;
                        finish = true;
                        
                    } else {
                        //Switched to a new subgroup.
                        update_closest_group_member();
                        if(!closest_group_member_distant) {
                            finish = true;
                        }
                        
                    }
                    
                } while(!finish);
                
                if(switch_successful) {
                    cur_leader_ptr->swap_held_pikmin(closest_group_member);
                }
            }
            
            if(switch_successful) {
                sfx_switch_pikmin.play(0, false);
            }
            
        } else if(
            button == BUTTON_NEXT_MATURITY ||
            button == BUTTON_PREV_MATURITY
        ) {
        
            /**********************************
            *                      V  -->  *  *
            *   Switch maturity    |       |  *
            *                     ( )     ( ) *
            **********************************/
            
            if(
                pos == 0 ||
                !cur_leader_ptr->holding_pikmin ||
                cur_leader_ptr->holding_pikmin->type->category->id !=
                MOB_CATEGORY_PIKMIN
            ) {
                return;
            }
            
            pikmin* held_p_ptr = (pikmin*) cur_leader_ptr->holding_pikmin;
            
            pikmin* closest_members[N_MATURITIES];
            dist closest_dists[N_MATURITIES];
            for(size_t m = 0; m < N_MATURITIES; ++m) {
                closest_members[m] = NULL;
            }
            
            for(size_t m = 0; m < cur_leader_ptr->group->members.size(); ++m) {
                mob* m_ptr = cur_leader_ptr->group->members[m];
                if(m_ptr->type != held_p_ptr->type) continue;
                
                pikmin* p_ptr = (pikmin*) m_ptr;
                if(p_ptr->maturity == held_p_ptr->maturity) continue;
                
                dist d(cur_leader_ptr->pos, p_ptr->pos);
                if(
                    !closest_members[p_ptr->maturity] ||
                    d < closest_dists[p_ptr->maturity]
                ) {
                    closest_members[p_ptr->maturity] = p_ptr;
                    closest_dists[p_ptr->maturity] = d;
                }
                
            }
            
            size_t next_maturity = held_p_ptr->maturity;
            mob* new_pikmin = NULL;
            bool finished = false;
            do {
                next_maturity =
                    sum_and_wrap(
                        next_maturity,
                        (button == BUTTON_NEXT_MATURITY ? 1 : -1),
                        N_MATURITIES
                    );
                    
                //Back to the start?
                if(next_maturity == held_p_ptr->maturity) break;
                
                if(!closest_members[next_maturity]) continue;
                
                new_pikmin = closest_members[next_maturity];
                finished = true;
                
            } while(!finished);
            
            if(new_pikmin) {
                cur_leader_ptr->swap_held_pikmin(new_pikmin);
            }
            
        }
        
    } else { //Displaying a message.
    
        if((button == BUTTON_THROW || button == BUTTON_PAUSE) && pos == 1) {
            size_t stopping_char =
                cur_message_stopping_chars[cur_message_section + 1];
            if(cur_message_char == stopping_char) {
                if(stopping_char == cur_message.size()) {
                    start_message("", NULL);
                } else {
                    cur_message_section++;
                }
            } else {
                cur_message_char = stopping_char;
            }
        }
        
    }
    
}


/* ----------------------------------------------------------------------------
 * Call this whenever an "active" control is inputted.
 * An "active" control is anything that moves the leader in some way.
 * This function makes the leader wake up from lying down,
 * stop auto-plucking, etc.
 */
void active_control() {
    cur_leader_ptr->fsm.run_event(LEADER_EVENT_CANCEL);
}


/* ----------------------------------------------------------------------------
 * Creates information about a control.
 * action: The action this control does in-game. Use BUTTON_*.
 * player: Player number.
 * s:      The textual code that represents the hardware inputs.
 */
control_info::control_info(unsigned char action, string s) :
    action(action),
    type(CONTROL_TYPE_NONE),
    device_nr(0),
    button(0),
    stick(0),
    axis(0) {
    vector<string> parts = split(s, "_");
    size_t n_parts = parts.size();
    
    if(n_parts == 0) return;
    if(parts[0] == "k") {   //Keyboard.
        if(n_parts > 1) {
            type = CONTROL_TYPE_KEYBOARD_KEY;
            button = s2i(parts[1]);
        }
        
    } else if(parts[0] == "mb") { //Mouse button.
        if(n_parts > 1) {
            type = CONTROL_TYPE_MOUSE_BUTTON;
            button = s2i(parts[1]);
        }
        
    } else if(parts[0] == "mwu") { //Mouse wheel up.
        type = CONTROL_TYPE_MOUSE_WHEEL_UP;
        
    } else if(parts[0] == "mwd") { //Mouse wheel down.
        type = CONTROL_TYPE_MOUSE_WHEEL_DOWN;
        
    } else if(parts[0] == "mwl") { //Mouse wheel left.
        type = CONTROL_TYPE_MOUSE_WHEEL_LEFT;
        
    } else if(parts[0] == "mwr") { //Mouse wheel right.
        type = CONTROL_TYPE_MOUSE_WHEEL_RIGHT;
        
    } else if(parts[0] == "jb") { //Joystick button.
        if(n_parts > 2) {
            type = CONTROL_TYPE_JOYSTICK_BUTTON;
            device_nr = s2i(parts[1]);
            button = s2i(parts[2]);
        }
        
    } else if(parts[0] == "jap") { //Joystick axis, positive.
        if(n_parts > 3) {
            type = CONTROL_TYPE_JOYSTICK_AXIS_POS;
            device_nr = s2i(parts[1]);
            stick = s2i(parts[2]);
            axis = s2i(parts[3]);
        }
    } else if(parts[0] == "jan") { //Joystick axis, negative.
        if(n_parts > 3) {
            type = CONTROL_TYPE_JOYSTICK_AXIS_NEG;
            device_nr = s2i(parts[1]);
            stick = s2i(parts[2]);
            axis = s2i(parts[3]);
        }
    } else {
        log_error(
            "Unrecognized control type \"" + parts[0] + "\""
            " (value=\"" + s + "\")!");
    }
}


/* ----------------------------------------------------------------------------
 * Converts a control info's hardware input data into a string,
 * used in the options file.
 */
string control_info::stringify() {
    if(type == CONTROL_TYPE_KEYBOARD_KEY) {
        return "k_" + i2s(button);
    } else if(type == CONTROL_TYPE_MOUSE_BUTTON) {
        return "mb_" + i2s(button);
    } else if(type == CONTROL_TYPE_MOUSE_WHEEL_UP) {
        return "mwu";
    } else if(type == CONTROL_TYPE_MOUSE_WHEEL_DOWN) {
        return "mwd";
    } else if(type == CONTROL_TYPE_MOUSE_WHEEL_LEFT) {
        return "mwl";
    } else if(type == CONTROL_TYPE_MOUSE_WHEEL_RIGHT) {
        return "mwr";
    } else if(type == CONTROL_TYPE_JOYSTICK_BUTTON) {
        return "jb_" + i2s(device_nr) + "_" + i2s(button);
    } else if(type == CONTROL_TYPE_JOYSTICK_AXIS_POS) {
        return "jap_" + i2s(device_nr) + "_" + i2s(stick) + "_" + i2s(axis);
    } else if(type == CONTROL_TYPE_JOYSTICK_AXIS_NEG) {
        return "jan_" + i2s(device_nr) + "_" + i2s(stick) + "_" + i2s(axis);
    }
    
    return "";
}
