/*
 * Copyright (c) Andre 'Espyo' Silva 2013-2017.
 * The following source file belongs to the open-source project
 * Pikmin fangame engine. Please read the included
 * README and LICENSE files for more information.
 * Pikmin is copyright (c) Nintendo.
 *
 * === FILE DESCRIPTION ===
 * Miscellaneous structures, too small
 * to warrant their own files.
 */

#include <algorithm>
#include <climits>

#include "const.h"
#include "functions.h"
#include "misc_structs.h"
#include "vars.h"

/* ----------------------------------------------------------------------------
 * Creates a structure with information about a bitmap, for the manager.
 */
bmp_info::bmp_info(ALLEGRO_BITMAP* b) :
    b(b),
    calls(1) {
    
}


/* ----------------------------------------------------------------------------
 * Returns the specified bitmap, by name.
 */
ALLEGRO_BITMAP* bmp_manager::get(const string &name, data_node* node) {
    if(name.empty()) return load_bmp("", node);
    
    if(list.find(name) == list.end()) {
        ALLEGRO_BITMAP* b = load_bmp(name, node);
        list[name] = bmp_info(b);
        return b;
    } else {
        list[name].calls++;
        return list[name].b;
    }
};


/* ----------------------------------------------------------------------------
 * Marks a bitmap to have one less call.
 * If it has 0 calls, it's automatically cleared.
 */
void bmp_manager::detach(const string &name) {
    if(name.empty()) return;
    
    auto it = list.find(name);
    if(it == list.end()) return;
    
    it->second.calls--;
    if(it->second.calls == 0) {
        if(it->second.b != bmp_error) {
            al_destroy_bitmap(it->second.b);
        }
        list.erase(it);
    }
}


/* ----------------------------------------------------------------------------
 * Adds a new button to the list.
 */
void button_manager::add(
    const size_t id, const string &name, const string &option_name,
    const string &default_control_str
) {
    button_manager::button b;
    b.id = id;
    b.name = name;
    b.option_name = option_name;
    b.default_control_str = default_control_str;
    
    list.push_back(b);
}


/* ----------------------------------------------------------------------------
 * Initializes a movement struct with all movements set to 0.
 */
movement_struct::movement_struct() :
    right(0),
    up(0),
    left(0),
    down(0) {
    
}


/* ----------------------------------------------------------------------------
 * Assuming the movement as a joystick, this returns how far away
 * from the center the joystick is tilted [0, 1].
 * Because the movement is not necessarily cirular (e.g. keyboard instead
 * of joystick), this can return values larger than 1.
 */
float movement_struct::get_intensity() {
    return dist(point(), get_coords()).to_float();
}


/* ----------------------------------------------------------------------------
 * Returns the coordinates for the movement, in the range [-1, 1];
 */
point movement_struct::get_coords() {
    return point(right - left, down - up);
}


/* ----------------------------------------------------------------------------
 * Creates a new distance number, given two points.
 */
dist::dist(const point p1, const point p2) :
    distance_squared(
        (p2.x - p1.x) * (p2.x - p1.x) +
        (p2.y - p1.y) * (p2.y - p1.y)
    ),
    has_normal_distance(false),
    normal_distance(0) {
    
}


/* ----------------------------------------------------------------------------
 * Creates a new distance number, given a non-squared distance.
 */
dist::dist(const float d) :
    distance_squared(d* d),
    has_normal_distance(true),
    normal_distance(d) {
    
}


/* ----------------------------------------------------------------------------
 * Returns the regular distance as a number.
 */
float dist::to_float() {
    if(has_normal_distance) {
        return normal_distance;
    } else {
        normal_distance = sqrt(distance_squared);
        has_normal_distance = true;
        return normal_distance;
    }
}


/* ----------------------------------------------------------------------------
 * Distance comparisons and plain operations.
 */
bool dist::operator<(const float d2) {
    return distance_squared < (d2 * d2);
}
bool dist::operator>(const float d2) {
    return distance_squared > (d2 * d2);
}
bool dist::operator==(const float d2) {
    return distance_squared == (d2 * d2);
}
bool dist::operator<=(const float d2) {
    return !operator>(d2);
}
bool dist::operator>=(const float d2) {
    return !operator<(d2);
}
bool dist::operator!=(const float d2) {
    return !operator==(d2);
}
bool dist::operator<(const dist &d2) {
    return distance_squared < d2.distance_squared;
}
bool dist::operator>(const dist &d2) {
    return distance_squared > d2.distance_squared;
}
bool dist::operator==(const dist &d2) {
    return distance_squared == d2.distance_squared;
}
bool dist::operator<=(const dist &d2) {
    return !operator>(d2);
}
bool dist::operator>=(const dist &d2) {
    return !operator<(d2);
}
bool dist::operator!=(const dist &d2) {
    return !operator==(d2);
}
void dist::operator+=(const dist &d2) {
    distance_squared += d2.distance_squared;
    if(has_normal_distance && d2.has_normal_distance) {
        normal_distance += d2.normal_distance;
    } else {
        has_normal_distance = false;
    }
}


/* ----------------------------------------------------------------------------
 * Registers a new mob category.
 */
void mob_category_manager::register_category(
    unsigned char nr,
    string pname, string sname, string folder,
    ALLEGRO_COLOR editor_color,
    function<void (vector<string> &list)> lister,
    function<mob_type* (const string &name)> type_getter,
    function<mob_type* ()> type_constructor,
    function<void (mob_type*)> type_saver
) {
    if(nr >= categories.size()) {
        categories.insert(
            categories.end(),
            (nr + 1) - categories.size(),
            mob_category_info()
        );
    }
    categories[nr].plural_name = pname;
    categories[nr].singular_name = sname;
    categories[nr].folder = folder;
    categories[nr].editor_color = editor_color;
    categories[nr].lister = lister;
    categories[nr].type_getter = type_getter;
    categories[nr].type_constructor = type_constructor;
    categories[nr].type_saver = type_saver;
    
}


/* ----------------------------------------------------------------------------
 * Returns the number of a category given its plural name.
 * Returns 255 on error.
 */
unsigned char mob_category_manager::get_nr_from_pname(const string &pname) {
    for(unsigned char n = 0; n < categories.size(); ++n) {
        if(categories[n].plural_name == pname) return n;
    }
    return 255;
}


/* ----------------------------------------------------------------------------
 * Returns the number of a category given its singular name.
 * Returns 255 on error.
 */
unsigned char mob_category_manager::get_nr_from_sname(const string &sname) {
    for(unsigned char n = 0; n < categories.size(); ++n) {
        if(categories[n].singular_name == sname) return n;
    }
    return 255;
}


/* ----------------------------------------------------------------------------
 * Returns the plural name of a category given its number.
 * Returns an empty string on error.
 */
string mob_category_manager::get_pname(const unsigned char cat_nr) {
    if(cat_nr < categories.size()) return categories[cat_nr].plural_name;
    return "";
}


/* ----------------------------------------------------------------------------
 * Returns the singular name of a category given its number.
 * Returns an empty string on error.
 */
string mob_category_manager::get_sname(const unsigned char cat_nr) {
    if(cat_nr < categories.size()) return categories[cat_nr].singular_name;
    return "";
}


/* ----------------------------------------------------------------------------
 * Returns the color of a category on the area editor.
 * Returns transparent on error.
 */
ALLEGRO_COLOR mob_category_manager::get_editor_color(
    const unsigned char cat_nr
) {
    if(cat_nr < categories.size()) return categories[cat_nr].editor_color;
    return al_map_rgba(0, 0, 0, 0);
}


/* ----------------------------------------------------------------------------
 * Returns the folder of a category, given its number.
 * Returns an empty string on error.
 */
string mob_category_manager::get_folder(const unsigned char cat_nr) {
    if(cat_nr < categories.size()) return categories[cat_nr].folder;
    return "";
}


/* ----------------------------------------------------------------------------
 * Returns the number of registered mob categories.
 */
unsigned char mob_category_manager::get_nr_of_categories() {
    return categories.size();
}


/* ----------------------------------------------------------------------------
 * Lists the names of all mob types in a category onto a vector of strings.
 */
void mob_category_manager::get_list(vector<string> &l, unsigned char cat_nr) {
    if(cat_nr < categories.size()) categories[cat_nr].lister(l);
}


/* ----------------------------------------------------------------------------
 * Sets a mob generator's type pointer, given the type's name.
 * It uses the mob gen's existing category to search for the name.
 */
void mob_category_manager::set_mob_type_ptr(
    mob_gen* m, const string &type_name
) {
    m->type = categories[m->category].type_getter(type_name);
}


/* ----------------------------------------------------------------------------
 * Creates a new mob type of this category.
 */
mob_type* mob_category_manager::create_mob_type(const unsigned char cat_nr) {
    if(cat_nr >= categories.size()) return nullptr;
    return categories[cat_nr].type_constructor();
}


/* ----------------------------------------------------------------------------
 * Saves a mob type onto its own vector.
 */
void mob_category_manager::save_mob_type(
    const unsigned char cat_nr, mob_type* mt
) {
    if(cat_nr < categories.size()) categories[cat_nr].type_saver(mt);
}


/* ----------------------------------------------------------------------------
 * Creates a structure with info about group spots.
 */
group_spot_info::group_spot_info(
    const unsigned max_mobs, const float spot_radius
) :
    spot_radius(spot_radius) {
    
    //Center spot first.
    coords.push_back(vector<point>(1, point()));
    mobs_in_spots.push_back(vector<mob*>(1, NULL));
    
    //Total spots. Starts at 1 because we did the center spot already.
    unsigned total_spots = 1;
    //Current wheel.
    unsigned w = 1;
    while(total_spots < max_mobs) {
    
        //First, calculate how far the center
        //of these spots are from the central spot.
        float dist_from_center =
            spot_radius * w + //Spots.
            GROUP_SPOT_INTERVAL * w; //Interval between spots.
            
        /* Now we need to figure out what's the angular distance
         * between each spot. For that, we need the actual diameter
         * (distance from one point to the other),
         * and the central distance, which is distance between the center
         * and the middle of two spots.
         */
        
        /* We can get the middle distance because we know the actual diameter,
         * which should be the size of a Pikmin and one interval unit,
         * and we know the distance from one spot to the center.
         */
        float actual_diameter = spot_radius + GROUP_SPOT_INTERVAL;
        
        //Just calculate the remaining side of the triangle, now that we know
        //the hypotenuse and the actual diameter (one side of the triangle).
        float middle_distance =
            sqrt(
                (dist_from_center * dist_from_center) -
                (actual_diameter * 0.5 * actual_diameter * 0.5)
            );
            
        //Now, get the angular distance.
        float angular_dist = 2 * atan2(actual_diameter, 2 * middle_distance);
        
        //Finally, we can calculate where the other spots are.
        unsigned n_spots_on_wheel = floor(M_PI * 2 / angular_dist);
        //Get a better angle. One that can evenly distribute the spots.
        float angle = M_PI * 2 / n_spots_on_wheel;
        
        coords.push_back(vector<point>());
        mobs_in_spots.push_back(vector<mob*>());
        for(unsigned s = 0; s < n_spots_on_wheel; ++s) {
            coords.back().push_back(
                point(
                    dist_from_center * cos(angle * s) +
                    randomf(-GROUP_SPOT_INTERVAL, GROUP_SPOT_INTERVAL),
                    dist_from_center * sin(angle * s) +
                    randomf(-GROUP_SPOT_INTERVAL, GROUP_SPOT_INTERVAL)
                )
            );
            mobs_in_spots.back().push_back(NULL);
        }
        
        total_spots += n_spots_on_wheel;
        w++;
    }
    
    n_wheels = w;
    current_wheel = n_current_wheel_members = 0;
}


/* ----------------------------------------------------------------------------
 * Adds a member to a leader's group spots.
 */
void group_spot_info::add(mob* m) {
    if(n_current_wheel_members == mobs_in_spots[current_wheel].size()) {
        current_wheel++;
        n_current_wheel_members = 0;
    }
    
    size_t n_spots_in_wheel = mobs_in_spots[current_wheel].size();
    size_t chosen_spot_nr =
        randomi(0, (n_spots_in_wheel - n_current_wheel_members) - 1);
    size_t chosen_spot = 0;
    auto v = &mobs_in_spots[current_wheel];
    for(unsigned s = 0, c = 0; s < n_spots_in_wheel; ++s) {
        if((*v)[s]) continue;
        if(c == chosen_spot_nr) {
            chosen_spot = s;
            break;
        }
        c++;
    }
    
    mobs_in_spots[current_wheel][chosen_spot] = m;
    
    n_current_wheel_members++;
    
    m->group_spot = coords[current_wheel][chosen_spot];
}


/* ----------------------------------------------------------------------------
 * Removes a member from a leader's group spots.
 */
void group_spot_info::remove(mob* m) {
    //Wheel number of the mob we're trying to remove.
    unsigned mob_wheel = UINT_MAX;
    //Spot number of the mob we're trying to remove.
    unsigned mob_spot = UINT_MAX;
    
    size_t n_wheels = mobs_in_spots.size();
    for(size_t w = 0; w < n_wheels; ++w) {
    
        size_t n_spots = mobs_in_spots[w].size();
        for(size_t s = 0; s < n_spots; ++s) {
        
            if(mobs_in_spots[w][s] == m) {
                mob_wheel = w;
                mob_spot = s;
                break;
            }
            
        }
        
        if(mob_wheel != UINT_MAX) break;
    }
    
    //If the member to remove is the only one
    //from the outermost wheel, let it go.
    if(n_current_wheel_members == 1 && current_wheel == mob_wheel) {
        if(current_wheel == 0) {
            n_current_wheel_members = 0;
        } else {
            current_wheel--;
            n_current_wheel_members = mobs_in_spots[current_wheel].size();
        }
        mobs_in_spots[mob_wheel][mob_spot] = NULL;
    } else {
        //If it's not from the outermost wheel,
        //find some other mob (from the outermost wheel) to replace it.
        unsigned replacement_spot;
        unsigned n_spots = mobs_in_spots[current_wheel].size();
        
        do {
            replacement_spot = randomi(0, n_spots - 1);
        } while(
            !mobs_in_spots[current_wheel][replacement_spot] ||
            (current_wheel == mob_wheel && replacement_spot == mob_spot)
        );
        
        mobs_in_spots[mob_wheel][mob_spot] =
            mobs_in_spots[current_wheel][replacement_spot];
        mobs_in_spots[mob_wheel][mob_spot]->group_spot =
            coords[mob_wheel][mob_spot];
        mobs_in_spots[current_wheel][replacement_spot] = NULL;
        
        //TODO remove this temporary hack:
        mobs_in_spots[mob_wheel][mob_spot]->chase_offset =
            point(
                coords[mob_wheel][mob_spot].x,
                coords[mob_wheel][mob_spot].y + 30
            );
            
        n_current_wheel_members--;
        if(n_current_wheel_members == 0) {
            current_wheel--;
            n_current_wheel_members = mobs_in_spots[current_wheel].size();
        }
    }
}



/* ----------------------------------------------------------------------------
 * Creates a "reader setter".
 * dn: Pointer to the base data node.
 */
reader_setter::reader_setter(data_node* dn) :
    node(dn) {
    
}


/* ----------------------------------------------------------------------------
 * Reads a child node's value, and uses it to set a variable.
 * Will not do anything if the child's value is empty.
 * child: Name of the child node.
 * var:   The var to set. This is an Allegro color.
 */
void reader_setter::set(const string &child, ALLEGRO_COLOR &var) {
    string s = node->get_child_by_name(child)->value;
    if(s.empty()) return;
    var = s2c(s);
}


/* ----------------------------------------------------------------------------
 * Reads a child node's value, and uses it to set a variable.
 * Will not do anything if the child's value is empty.
 * child: Name of the child node.
 * var:   The var to set. This is a string.
 */
void reader_setter::set(const string &child, string &var) {
    string s = node->get_child_by_name(child)->value;
    if(s.empty()) return;
    var = s;
}


/* ----------------------------------------------------------------------------
 * Reads a child node's value, and uses it to set a variable.
 * Will not do anything if the child's value is empty.
 * child: Name of the child node.
 * var:   The var to set. This is an integer.
 */
void reader_setter::set(const string &child, size_t &var) {
    string s = node->get_child_by_name(child)->value;
    if(s.empty()) return;
    var = s2i(s);
}


/* ----------------------------------------------------------------------------
 * Reads a child node's value, and uses it to set a variable.
 * Will not do anything if the child's value is empty.
 * child: Name of the child node.
 * var:   The var to set. This is an integer.
 */
void reader_setter::set(const string &child, int &var) {
    string s = node->get_child_by_name(child)->value;
    if(s.empty()) return;
    var = s2i(s);
}


/* ----------------------------------------------------------------------------
 * Reads a child node's value, and uses it to set a variable.
 * Will not do anything if the child's value is empty.
 * child: Name of the child node.
 * var:   The var to set. This is an unsigned char.
 */
void reader_setter::set(const string &child, unsigned char &var) {
    string s = node->get_child_by_name(child)->value;
    if(s.empty()) return;
    var = s2i(s);
}


/* ----------------------------------------------------------------------------
 * Reads a child node's value, and uses it to set a variable.
 * Will not do anything if the child's value is empty.
 * child: Name of the child node.
 * var:   The var to set. This is a boolean.
 */
void reader_setter::set(const string &child, bool &var) {
    string s = node->get_child_by_name(child)->value;
    if(s.empty()) return;
    var = s2b(s);
}


/* ----------------------------------------------------------------------------
 * Reads a child node's value, and uses it to set a variable.
 * Will not do anything if the child's value is empty.
 * child: Name of the child node.
 * var:   The var to set. This is a float.
 */
void reader_setter::set(const string &child, float &var) {
    string s = node->get_child_by_name(child)->value;
    if(s.empty()) return;
    var = s2f(s);
}



/* ----------------------------------------------------------------------------
 * Creates a structure with sample info.
 */
sample_struct::sample_struct(ALLEGRO_SAMPLE* s, ALLEGRO_MIXER* mixer) :
    sample(s),
    instance(NULL) {
    
    if(!s) return;
    instance = al_create_sample_instance(s);
    al_attach_sample_instance_to_mixer(instance, mixer);
}


/* ----------------------------------------------------------------------------
 * Play the sample.
 * max_override_pos: Override the currently playing sound
   * only if it's already in this position, or beyond.
   * This is in seconds. 0 means always override. -1 means never override.
 * loop: Loop the sound?
 * gain: Volume, 0 - 1.
 * pan: Panning, 0 - 1 (0.5 is centered).
 * speed: Playing speed.
 */
void sample_struct::play(
    const float max_override_pos, const bool loop, const float gain,
    const float pan, const float speed
) {
    if(!sample || !instance) return;
    
    if(max_override_pos != 0 && al_get_sample_instance_playing(instance)) {
        float secs = al_get_sample_instance_position(instance) / (float) 44100;
        if(
            (secs < max_override_pos && max_override_pos > 0) ||
            max_override_pos == -1
        ) {
            return;
        }
    }
    
    al_set_sample_instance_playmode(
        instance, (loop ? ALLEGRO_PLAYMODE_LOOP : ALLEGRO_PLAYMODE_ONCE)
    );
    al_set_sample_instance_gain(instance, gain);
    al_set_sample_instance_pan(instance, pan);
    al_set_sample_instance_speed(instance, speed);
    
    al_set_sample_instance_position(instance, 0);
    al_set_sample_instance_playing( instance, true);
}


/* ----------------------------------------------------------------------------
 * Stops a playing sample instance.
 */
void sample_struct::stop() {
    al_set_sample_instance_playing(instance, false);
}


/* ----------------------------------------------------------------------------
 * Destroys a structure with sample info.
 */
void sample_struct::destroy() {
    //TODO uncommenting this is causing a crash.
    //al_detach_sample_instance(instance);
    al_destroy_sample_instance(instance);
    al_destroy_sample(sample);
}


/* ----------------------------------------------------------------------------
 * Registers a new type of sector.
 */
void sector_types_manager::register_type(unsigned char nr, string name) {
    if(nr >= names.size()) {
        names.insert(names.end(), (nr + 1) - names.size(), "");
    }
    names[nr] = name;
}


/* ----------------------------------------------------------------------------
 * Returns the number of a sector type, given its name.
 * Returns 255 on error.
 */
unsigned char sector_types_manager::get_nr(const string &name) {
    for(unsigned char n = 0; n < names.size(); ++n) {
        if(names[n] == name) return n;
    }
    return 255;
}


/* ----------------------------------------------------------------------------
 * Returns the name of a sector type, given its number.
 * Returns an empty string on error.
 */
string sector_types_manager::get_name(const unsigned char nr) {
    if(nr < names.size()) return names[nr];
    return "";
}


/* ----------------------------------------------------------------------------
 * Returns the number of sector types registered.
 */
unsigned char sector_types_manager::get_nr_of_types() {
    return names.size();
}


/* ----------------------------------------------------------------------------
 * Creates a timer.
 */
timer::timer(float duration, const function<void()> &on_end) :
    duration(duration),
    time_left(0),
    on_end(on_end) {
    
    
}


/* ----------------------------------------------------------------------------
 * Destroys a timer.
 */
timer::~timer() {
    //TODO Valgrind detects a leak with on_end...
}


/* ----------------------------------------------------------------------------
 * Starts a timer.
 * can_restart: If false, calling this while the timer is still ticking down
   * will not do anything.
 */
void timer::start(const bool can_restart) {
    if(!can_restart && time_left > 0) return;
    time_left = duration;
}


/* ----------------------------------------------------------------------------
 * Starts a timer, but sets a new duration.
 */
void timer::start(const float new_duration) {
    duration = new_duration;
    start();
}


/* ----------------------------------------------------------------------------
 * Ticks a timer.
 * amount: Time to tick.
 */
void timer::tick(const float amount) {
    if(time_left == 0.0f) return;
    time_left = max(time_left - amount, 0.0f);
    if(time_left == 0.0f && on_end) {
        on_end();
    }
}


/* ----------------------------------------------------------------------------
 * Returns the ratio of time left (i.e. 0 if done, 1 if all time is left).
 */
float timer::get_ratio_left() {
    return time_left / duration;
}


const float fade_manager::FADE_DURATION = 0.15f;

/* ----------------------------------------------------------------------------
 * Creates a fade manager.
 */
fade_manager::fade_manager() :
    time_left(0),
    fade_in(false),
    on_end(nullptr) {
    
}


/* ----------------------------------------------------------------------------
 * Sets up the start of a fade.
 */
void fade_manager::start_fade(const bool fade_in, function<void()> on_end) {
    time_left = FADE_DURATION;
    this->fade_in = fade_in;
    this->on_end = on_end;
}


/* ----------------------------------------------------------------------------
 * Returns whether the current fade is a fade in or fade out.
 */
bool fade_manager::is_fade_in() {
    return fade_in;
}


/* ----------------------------------------------------------------------------
 * Returns whether or not a fade is currently in progress.
 */
bool fade_manager::is_fading() {
    return time_left > 0;
}


/* ----------------------------------------------------------------------------
 * Returns the percentage of progress left in the current fade.
 */
float fade_manager::get_perc_left() {
    return time_left / FADE_DURATION;
}


/* ----------------------------------------------------------------------------
 * Ticks the fade manager by one frame.
 */
void fade_manager::tick(const float time) {
    if(time_left == 0) return;
    time_left -= time;
    if(time_left <= 0) {
        time_left = 0;
        if(on_end) on_end();
    }
}


/* ----------------------------------------------------------------------------
 * Draws the fade overlay, if there is a fade in progress.
 */
void fade_manager::draw() {
    if(is_fading()) {
        unsigned char alpha = (fade_mgr.get_perc_left()) * 255;
        al_draw_filled_rectangle(
            0, 0, scr_w, scr_h,
            al_map_rgba(
                0, 0, 0, (fade_mgr.is_fade_in() ? alpha : 255 - alpha)
            )
        );
    }
}


/* ----------------------------------------------------------------------------
 * Initializes a sprite effect properties struct.
 */
sprite_effect_props::sprite_effect_props() :
    translation(0, 0),
    rotation(0),
    scale(1, 1),
    tint_color(al_map_rgb(255, 255, 255)),
    glow_color(al_map_rgb(0, 0, 0)) {
    
}


/* ----------------------------------------------------------------------------
 * Initializes a sprite effect struct.
 */
sprite_effect::sprite_effect() :
    cur_time(0) {
    
}


/* ----------------------------------------------------------------------------
 * Adds a keyframe to the effect.
 * i.e. What the properties should be at a given point in time.
 */
void sprite_effect::add_keyframe(const float time, sprite_effect_props props) {
    keyframes[time] = props;
}


/* ----------------------------------------------------------------------------
 * Sets the current life time on the effect.
 */
void sprite_effect::set_cur_time(const float cur_time) {
    this->cur_time = cur_time;
}


/* ----------------------------------------------------------------------------
 * Returns what the properties should be at the specified time.
 * These values are interpolated using the keyframes.
 */
sprite_effect_props sprite_effect::get_final_properties() {
    if(keyframes.empty()) {
        log_error("ENGINE WARNING: Sprite effect has no keyframes!");
        return sprite_effect_props();
        
    } else if(keyframes.size() == 1) {
        return keyframes[0];
        
    } else {
        //Find the previous and next keyframes.
        float prev_time = 0;
        float next_time = 0;
        sprite_effect_props* prev_keyframe = &keyframes[0];
        sprite_effect_props* next_keyframe = NULL;
        for(auto k = keyframes.begin(); k != keyframes.end(); ++k) {
            if(k->first > cur_time) {
                next_keyframe = &k->second;
                next_time = k->first;
                break;
            } else {
                prev_keyframe = &k->second;
                prev_time = k->first;
            }
        }
        if(!next_keyframe) next_keyframe = prev_keyframe;
        
        sprite_effect_props final_props;
        final_props.translation.x =
            interpolate_number(
                cur_time, prev_time, next_time,
                prev_keyframe->translation.x, next_keyframe->translation.x
            );
        final_props.translation.y =
            interpolate_number(
                cur_time, prev_time, next_time,
                prev_keyframe->translation.y, next_keyframe->translation.y
            );
        final_props.rotation =
            interpolate_number(
                cur_time, prev_time, next_time,
                prev_keyframe->rotation, next_keyframe->rotation
            );
        final_props.scale.x =
            interpolate_number(
                cur_time, prev_time, next_time,
                prev_keyframe->scale.x, next_keyframe->scale.x
            );
        final_props.scale.y =
            interpolate_number(
                cur_time, prev_time, next_time,
                prev_keyframe->scale.y, next_keyframe->scale.y
            );
        final_props.tint_color =
            interpolate_color(
                cur_time, prev_time, next_time,
                prev_keyframe->tint_color, next_keyframe->tint_color
            );
        final_props.glow_color =
            interpolate_color(
                cur_time, prev_time, next_time,
                prev_keyframe->glow_color, next_keyframe->glow_color
            );
            
        return final_props;
    }
}


/* ----------------------------------------------------------------------------
 * Adds an effect to the manager.
 */
void sprite_effect_manager::add_effect(sprite_effect effect) {
    effects.push_back(effect);
}


/* ----------------------------------------------------------------------------
 * Returns the final sprite effect properties, at the current time.
 * This is a combination of all current effects.
 */
sprite_effect_props sprite_effect_manager::get_final_properties() {
    sprite_effect_props final_props;
    ALLEGRO_COLOR glow_color_sum;
    glow_color_sum = al_map_rgba(0, 0, 0, 0);
    size_t n_glow_colors = 0;
    
    for(size_t e = 0; e < effects.size(); ++e) {
        sprite_effect_props props = effects[e].get_final_properties();
        
        final_props.translation = final_props.translation + props.translation;
        final_props.rotation += props.rotation;
        final_props.scale = final_props.scale * props.scale;
        final_props.tint_color.r *= props.tint_color.r;
        final_props.tint_color.g *= props.tint_color.g;
        final_props.tint_color.b *= props.tint_color.b;
        final_props.tint_color.a *= props.tint_color.a;
        
        if(props.glow_color.a > 0) {
            glow_color_sum.r += props.glow_color.r;
            glow_color_sum.g += props.glow_color.g;
            glow_color_sum.b += props.glow_color.b;
            glow_color_sum.a += props.glow_color.a;
            n_glow_colors++;
        }
        
    }
    
    if(n_glow_colors > 0) {
        final_props.glow_color.r = glow_color_sum.r / n_glow_colors;
        final_props.glow_color.g = glow_color_sum.g / n_glow_colors;
        final_props.glow_color.b = glow_color_sum.b / n_glow_colors;
        final_props.glow_color.a = glow_color_sum.a / n_glow_colors;
    }
    
    return final_props;
    
}


/* ----------------------------------------------------------------------------
 * Registers a new type of subgroup.
 * category: The category of subgroup type. Pikmin, leader, bomb-rock, etc.
 * pik_type: Type of Pikmin. Only necessary for Pikmin category.
 */
void subgroup_type_manager::register_type(
    const SUBGROUP_TYPE_CATEGORIES category,
    pikmin_type* pik_type
) {
    subgroup_type* new_sg_type = new subgroup_type();
    
    new_sg_type->category = category;
    new_sg_type->pik_type = pik_type;
    
    types.push_back(new_sg_type);
}


/* ----------------------------------------------------------------------------
 * Returns the type of subgroup corresponding to the parameters.
 * Returns NULL if not found.
 * category: The category of subgroup type. Pikmin, leader, bomb-rock, etc.
 * pik_type: Type of Pikmin. Only necessary for Pikmin category.
 */
subgroup_type* subgroup_type_manager::get_type(
    const SUBGROUP_TYPE_CATEGORIES category,
    pikmin_type* pik_type
) {
    for(size_t t = 0; t < types.size(); ++t) {
        subgroup_type* t_ptr = types[t];
        if(t_ptr->category == category && t_ptr->pik_type == pik_type) {
            return t_ptr;
        }
    }
    return NULL;
}


/* ----------------------------------------------------------------------------
 * Returns the first registered subgroup type.
 */
subgroup_type* subgroup_type_manager::get_first_type() {
    return types.front();
}


/* ----------------------------------------------------------------------------
 * Returns the subgroup type that comes before the given type.
 */
subgroup_type* subgroup_type_manager::get_prev_type(
    subgroup_type* sgt
) {
    for(size_t t = 0; t < types.size(); ++t) {
        if(types[t] == sgt) {
            return get_prev_in_vector(types, t);
        }
    }
    return NULL;
}


/* ----------------------------------------------------------------------------
 * Returns the subgroup type that comes after the given type.
 */
subgroup_type* subgroup_type_manager::get_next_type(
    subgroup_type* sgt
) {
    for(size_t t = 0; t < types.size(); ++t) {
        if(types[t] == sgt) {
            return get_next_in_vector(types, t);
        }
    }
    return NULL;
}


/* ----------------------------------------------------------------------------
 * Clears the list of registered subgroup types.
 */
void subgroup_type_manager::clear() {
    for(size_t t = 0; t < types.size(); ++t) {
        delete types[t];
    }
    types.clear();
}
