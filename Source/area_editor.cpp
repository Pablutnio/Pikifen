/*
 * Copyright (c) Andr� 'Espyo' Silva 2014.
 * The following source file belongs to the open-source project
 * Pikmin fangame engine. Please read the included README file
 * for more information.
 * Pikmin is copyright (c) Nintendo.
 *
 * === FILE DESCRIPTION ===
 * Area editor-related functions.
 */

#include <iomanip>

#include <allegro5/allegro_primitives.h>

#include "area_editor.h"
#include "functions.h"
#include "LAFI/button.h"
#include "LAFI/frame.h"
#include "LAFI/gui.h"
#include "LAFI/scrollbar.h"
#include "LAFI/textbox.h"
#include "vars.h"

void area_editor::change_to_right_frame() {
    hide_widget(ed_gui->widgets["frm_main"]);
    hide_widget(ed_gui->widgets["frm_picker"]);
    hide_widget(ed_gui->widgets["frm_sectors"]);
    
    if(ed_mode == EDITOR_MODE_MAIN) {
        show_widget(ed_gui->widgets["frm_main"]);
    } else if(ed_mode == EDITOR_MODE_SECTORS) {
        show_widget(ed_gui->widgets["frm_sectors"]);
    }
}

/* ----------------------------------------------------------------------------
 * Handles the main loop, both logic and drawing.
 */
void area_editor::do_logic() {

    //---Logic---
    
    if(ed_double_click_time > 0) {
        ed_double_click_time -= delta_t;
        if(ed_double_click_time < 0) ed_double_click_time = 0;
    }
    
    lafi_widget* wum = NULL; //Widget under mouse.
    wum = ed_gui->widgets["frm_bottom"]->mouse_over_widget;
    if(!wum) {
        if(!(ed_gui->widgets["frm_picker"]->flags & LAFI_FLAG_DISABLED)) {
            wum = ed_gui->widgets["frm_picker"]->mouse_over_widget;
        } else if(ed_mode == EDITOR_MODE_MAIN) {
            wum = ed_gui->widgets["frm_main"]->mouse_over_widget;
        }
    }
    
    ((lafi_label*) ed_gui->widgets["lbl_status_bar"])->text = (wum ? wum->description : "(" + itos(mouse_cursor_x) + "," + itos(mouse_cursor_y) + ")");
    
    //---Drawing---
    
    ed_gui->draw();
    
    ALLEGRO_TRANSFORM transform;
    al_identity_transform(&transform);
    al_translate_transform(&transform, cam_x + ((scr_w - 208) / 2 / cam_zoom), cam_y + (scr_h / 2 / cam_zoom));
    al_scale_transform(&transform, cam_zoom, cam_zoom);
    al_use_transform(&transform);
    
    al_set_clipping_rectangle(0, 0, scr_w - 208, scr_h - 16); {
        al_clear_to_color(al_map_rgb(0, 0, 64));
        
        //Grid.
        float grid_interval = 32;
        float grid_interval_2 = 64;
        
        float cam_leftmost = -cam_x - (scr_w / 2 / cam_zoom);
        float cam_topmost = -cam_y - (scr_h / 2 / cam_zoom);
        float cam_rightmost = cam_leftmost + (scr_w / cam_zoom);
        float cam_bottommost = cam_topmost + (scr_h / cam_zoom);
        
        if(cam_zoom >= ZOOM_MIN_LEVEL_EDITOR * 1.5) {
            float x = floor(cam_leftmost / grid_interval) * grid_interval;
            while(x < cam_rightmost + grid_interval) {
                ALLEGRO_COLOR c = al_map_rgb(255, 255, 255);
                bool draw_line = true;
                
                if(fmod(x, grid_interval_2) == 0) {
                    c = al_map_rgb(0, 96, 160);
                } else {
                    if(cam_zoom > ZOOM_MIN_LEVEL_EDITOR * 4) {
                        c = al_map_rgb(0, 64, 128);
                    } else {
                        draw_line = false;
                    }
                }
                
                if(draw_line) al_draw_line(x, cam_topmost, x, cam_bottommost + grid_interval, al_map_rgb(0, 64, 128), 1.0 / cam_zoom);
                x += grid_interval;
            }
            
            float y = floor(cam_topmost / grid_interval) * grid_interval;
            while(y < cam_bottommost + grid_interval) {
                ALLEGRO_COLOR c = al_map_rgb(255, 255, 255);
                bool draw_line = true;
                
                if(fmod(y, grid_interval_2) == 0) {
                    c = al_map_rgb(0, 96, 160);
                } else {
                    if(cam_zoom > ZOOM_MIN_LEVEL_EDITOR * 4) {
                        c = al_map_rgb(0, 64, 128);
                    } else {
                        draw_line = false;
                    }
                }
                
                if(draw_line) al_draw_line(cam_leftmost, y, cam_rightmost + grid_interval, y, c, 1.0 / cam_zoom);
                y += grid_interval;
            }
        }
        
        //Linedefs and vectors.
        size_t n_sectors = cur_area_map.sectors.size();
        for(size_t s = 0; s < n_sectors; s++) {
            sector* sector_ptr = cur_area_map.sectors[s];
            
            size_t n_linedefs = sector_ptr->linedefs.size();
            
            for(size_t l = 0; l < n_linedefs; l++) {
                linedef* l_ptr = sector_ptr->linedefs[l];
                al_draw_line(
                    l_ptr->vertices[0]->x,
                    l_ptr->vertices[0]->y,
                    l_ptr->vertices[1]->x,
                    l_ptr->vertices[1]->y,
                    al_map_rgb(192, 192, 192), 2.0 / cam_zoom);
                    
                al_draw_filled_circle(
                    l_ptr->vertices[0]->x,
                    l_ptr->vertices[0]->y,
                    3.0 / cam_zoom, al_map_rgb(224, 224, 224));
                    
                al_draw_filled_circle(
                    l_ptr->vertices[1]->x,
                    l_ptr->vertices[1]->y,
                    3.0 / cam_zoom, al_map_rgb(224, 224, 224));
            }
        }
        
        //0,0 marker.
        al_draw_line(-(grid_interval * 2), 0, grid_interval * 2, 0, al_map_rgb(128, 192, 255), 1.0 / cam_zoom);
        al_draw_line(0, -(grid_interval * 2), 0, grid_interval * 2, al_map_rgb(128, 192, 255), 1.0 / cam_zoom);
        
        //New sector marker.
        if(ed_new_sector_mode) {
            float x = snap_to_grid(mouse_cursor_x);
            float y = snap_to_grid(mouse_cursor_y);
            al_draw_line(x - 16 * cam_zoom, y, x + 16 * cam_zoom, y, al_map_rgb(255, 255, 255), 1);
            al_draw_line(x, y - 16 * cam_zoom, x, y + 16 * cam_zoom, al_map_rgb(255, 255, 255), 1);
        }
        
        //ToDo temp stuff.
        /*for(size_t v = 0; v < ed_temp_o.size(); v++) {
            al_draw_text(font, al_map_rgb(255, 255, 255), ed_temp_o[v]->x, ed_temp_o[v]->y - font_h, ALLEGRO_ALIGN_CENTER, ("O" + to_string((long long) v)).c_str());
        }
        for(size_t i = 0; i < ed_temp_i.size(); i++) {
            for(size_t v = 0; v < ed_temp_i[i].size(); v++) {
                al_draw_text(font, al_map_rgb(255, 255, 255), ed_temp_i[i][v]->x, ed_temp_i[i][v]->y - font_h * 2, ALLEGRO_ALIGN_CENTER, ("I" + to_string((long long) v)).c_str());
            }
        }
        for(size_t t = 0; t < cur_area_map.triangles.size(); t++) {
            al_draw_triangle(
                cur_area_map.triangles[t].points[0]->x,
                cur_area_map.triangles[t].points[0]->y,
                cur_area_map.triangles[t].points[1]->x,
                cur_area_map.triangles[t].points[1]->y,
                cur_area_map.triangles[t].points[2]->x,
                cur_area_map.triangles[t].points[2]->y,
                al_map_rgb(192, 0, 0), 1 / cam_zoom
            );
            al_draw_filled_triangle(
                cur_area_map.triangles[t].points[0]->x,
                cur_area_map.triangles[t].points[0]->y,
                cur_area_map.triangles[t].points[1]->x,
                cur_area_map.triangles[t].points[1]->y,
                cur_area_map.triangles[t].points[2]->x,
                cur_area_map.triangles[t].points[2]->y,
                al_map_rgba(0, (t % 3 == 0 ? 85 : t % 3 == 1 ? 170 : 255), 0, 128)
            );
        }*/
        
    } al_reset_clipping_rectangle();
    
    ALLEGRO_TRANSFORM id_transform;
    al_identity_transform(&id_transform);
    al_use_transform(&id_transform);
    
    al_flip_display();
}

/* ----------------------------------------------------------------------------
 * Handles the events for the area editor.
 */
void area_editor::handle_controls(ALLEGRO_EVENT ev) {

    ed_gui->handle_event(ev);
    
    if(
        ev.type == ALLEGRO_EVENT_MOUSE_AXES || ev.type == ALLEGRO_EVENT_MOUSE_WARPED ||
        ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN || ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP
    ) {
        mouse_cursor_x = ev.mouse.x / cam_zoom - cam_x - ((scr_w - 208) / 2 / cam_zoom);
        mouse_cursor_y = ev.mouse.y / cam_zoom - cam_y - (scr_h / 2 / cam_zoom);
    }
    
    
    if(ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
        if(ed_holding_m2) {
            cam_x += ev.mouse.dx / cam_zoom;
            cam_y += ev.mouse.dy / cam_zoom;
        }
        
        if(ed_moving_vertex != string::npos) {
            cur_area_map.vertices[ed_moving_vertex]->x = snap_to_grid(mouse_cursor_x);
            cur_area_map.vertices[ed_moving_vertex]->y = snap_to_grid(mouse_cursor_y);
        }
        
        if(ev.mouse.dz != 0) {
            //Zoom.
            float new_zoom = cam_zoom + (cam_zoom * ev.mouse.dz * 0.1);
            new_zoom = max(ZOOM_MIN_LEVEL_EDITOR, new_zoom);
            new_zoom = min(ZOOM_MAX_LEVEL_EDITOR, new_zoom);
            float new_mc_x = ev.mouse.x / new_zoom - cam_x - ((scr_w - 208) / 2 / new_zoom);
            float new_mc_y = ev.mouse.y / new_zoom - cam_y - (scr_h / 2 / new_zoom);
            
            cam_x -= (mouse_cursor_x - new_mc_x);
            cam_y -= (mouse_cursor_y - new_mc_y);
            mouse_cursor_x = new_mc_x;
            mouse_cursor_y = new_mc_y;
            cam_zoom = new_zoom;
        }
        
        
    } else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
        if(ev.mouse.button == 2) ed_holding_m2 = true;
        if(ev.mouse.button != 1) return;
        
        else if(ev.mouse.button == 1) {
            if(ev.mouse.x < scr_w - 208) {
            
                //Find a vertex to drag.
                for(size_t v = 0; v < cur_area_map.vertices.size(); v++) {
                    if(
                        check_dist(
                            mouse_cursor_x, mouse_cursor_y,
                            cur_area_map.vertices[v]->x, cur_area_map.vertices[v]->y,
                            6.0 / cam_zoom
                        )
                    ) {
                        ed_moving_vertex = v;
                        break;
                    }
                }
                
            }
        }
        
        if(ed_new_sector_mode) {
        
            ed_new_sector_mode = false;
            
            
        } else if(ed_moving_vertex == string::npos) {
        
            if(ed_double_click_time == 0) ed_double_click_time = 0.5;
            else {
                ed_double_click_time = 0;
                //Create a new vertex.
                
                for(size_t l = 0; l < cur_area_map.linedefs.size(); l++) {
                    linedef* l_ptr = cur_area_map.linedefs[l];
                    
                    if(
                        circle_touches_line(
                            mouse_cursor_x, mouse_cursor_y, 6,
                            l_ptr->vertices[0]->x, l_ptr->vertices[0]->y,
                            l_ptr->vertices[1]->x, l_ptr->vertices[1]->y
                        )
                    ) {
                        vertex* new_v_ptr = new vertex(mouse_cursor_x, mouse_cursor_y);
                        cur_area_map.vertices.push_back(new_v_ptr);
                        
                        linedef* new_l_ptr = new linedef(*l_ptr);
                        cur_area_map.linedefs.push_back(new_l_ptr);
                        new_l_ptr->vertex_nrs[0] = cur_area_map.vertices.size() - 1;
                        new_l_ptr->vertices[0] = new_v_ptr;
                        new_l_ptr->sectors[0]->linedef_nrs.push_back(cur_area_map.linedefs.size() - 1);
                        
                        l_ptr->vertex_nrs[1] = new_l_ptr->vertex_nrs[0];
                        l_ptr->vertices[1] = new_v_ptr;
                        
                        break;
                    }
                }
            }
            
        }
        
    } else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
        if(ev.mouse.button == 2) ed_holding_m2 = false;
        else if(ev.mouse.button == 1) {
            ed_moving_vertex = string::npos;
        }
    }
}

/* ----------------------------------------------------------------------------
 * Loads the area editor.
 */
void area_editor::load() {
    ed_mode = EDITOR_MODE_MAIN;
    
    load_area("test"); //ToDo non-fixed name, duh.
    
    //ToDo temporary stuff.
    cur_area_map.triangles = triangulate(cur_area_map.sectors[0]);
    
    lafi_style* s = new lafi_style(al_map_rgb(192, 192, 208), al_map_rgb(0, 0, 32), al_map_rgb(96, 128, 160));
    ed_gui = new lafi_gui(scr_w, scr_h, s);
    
    
    //Main frame.
    lafi_frame* frm_main = new lafi_frame(scr_w - 208, 0, scr_w, scr_h - 48);
    ed_gui->add("frm_main", frm_main);
    
    frm_main->easy_row();
    frm_main->easy_add("lbl_area", new lafi_label(0, 0, 0, 0, "Area:"), 100, 16);
    frm_main->easy_row();
    frm_main->easy_add("but_area", new lafi_button(0, 0, 0, 0), 100, 32);
    int y = frm_main->easy_row();
    
    lafi_frame* frm_area = new lafi_frame(scr_w - 208, y, scr_w, scr_h - 48);
    frm_main->add("frm_area", frm_area);
    frm_area->easy_row();
    frm_area->easy_add("but_sectors", new lafi_button(0, 0, 0, 0, "Edit sectors"), 100, 32);
    frm_area->easy_row();
    frm_area->easy_add("but_objects", new lafi_button(0, 0, 0, 0, "Edit objects"), 100, 32);
    frm_area->easy_row();
    
    
    //Bottom bar.
    lafi_frame* frm_bottom = new lafi_frame(scr_w - 208, scr_h - 48, scr_w, scr_h);
    ed_gui->add("frm_bottom", frm_bottom);
    frm_bottom->easy_row();
    frm_bottom->easy_add("but_toggle_hitboxes", new lafi_button(0, 0, 0, 0, "Hit"), 25, 32);
    frm_bottom->easy_add("but_load", new lafi_button(           0, 0, 0, 0, "Load"), 25, 32);
    frm_bottom->easy_add("but_save", new lafi_button(           0, 0, 0, 0, "Save"), 25, 32);
    frm_bottom->easy_add("but_quit", new lafi_button(           0, 0, 0, 0, "X"), 25, 32);
    frm_bottom->easy_row();
    
    
    //Picker frame.
    lafi_frame* frm_picker = new lafi_frame(scr_w - 208, 0, scr_w, scr_h - 48);
    hide_widget(frm_picker);
    ed_gui->add("frm_picker", frm_picker);
    
    frm_picker->add("but_back", new lafi_button(     scr_w - 200, 8, scr_w - 104, 24, "Back"));
    frm_picker->add("txt_new", new lafi_textbox(     scr_w - 200, 40, scr_w - 48, 56));
    frm_picker->add("but_new", new lafi_button(      scr_w - 40,  32, scr_w - 8,  64, "+"));
    frm_picker->add("frm_list", new lafi_frame(      scr_w - 200, 72, scr_w - 32, scr_h - 56));
    frm_picker->add("bar_scroll", new lafi_scrollbar(scr_w - 24,  72, scr_w - 8,  scr_h - 56));
    
    
    //Sectors frame.
    lafi_frame* frm_sectors = new lafi_frame(scr_w - 208, 0, scr_w, scr_h - 48);
    hide_widget(frm_sectors);
    ed_gui->add("frm_sectors", frm_sectors);
    
    frm_sectors->easy_row();
    frm_sectors->easy_add("but_back", new lafi_button(0, 0, 0, 0, "Back"), 50, 16);
    frm_sectors->easy_row();
    frm_sectors->easy_add("but_new", new lafi_button(0, 0, 0, 0, "+"), 20, 32);
    frm_sectors->easy_row();
    
    
    //Status bar.
    lafi_label* ed_gui_status_bar = new lafi_label(0, scr_h - 16, scr_w - 208, scr_h);
    ed_gui->add("lbl_status_bar", ed_gui_status_bar);
    
    
    //Properties -- main.
    frm_main->widgets["frm_area"]->widgets["but_sectors"]->left_mouse_click_handler = [] (lafi_widget*, int, int) {
        ed_mode = EDITOR_MODE_SECTORS;
        change_to_right_frame();
    };
    
    
    //Properties -- main.
    frm_sectors->widgets["but_back"]->left_mouse_click_handler = [] (lafi_widget*, int, int) {
        ed_mode = EDITOR_MODE_MAIN;
        change_to_right_frame();
    };
    frm_sectors->widgets["but_new"]->left_mouse_click_handler = [] (lafi_widget*, int, int) {
        ed_new_sector_mode = true;
    };
}

/* ----------------------------------------------------------------------------
 * Snaps a coordinate to the nearest grid space.
 */
float area_editor::snap_to_grid(const float c) {
    return round(c / 32) * 32;
}