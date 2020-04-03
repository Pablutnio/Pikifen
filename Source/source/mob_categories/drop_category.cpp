/*
 * Copyright (c) Andre 'Espyo' Silva 2013.
 * The following source file belongs to the open-source project Pikifen.
 * Please read the included README and LICENSE files for more information.
 * Pikmin is copyright (c) Nintendo.
 *
 * === FILE DESCRIPTION ===
 * Drop mob category class.
 */

#include <algorithm>

#include "drop_category.h"

#include "../mobs/drop.h"
#include "../vars.h"


/* ----------------------------------------------------------------------------
 * Creates an instance of the drop category.
 */
drop_category::drop_category() :
    mob_category(
        MOB_CATEGORY_DROPS, "Drop", "Drops",
        "Drops", al_map_rgb(204, 145, 145)
    ) {
    
}


/* ----------------------------------------------------------------------------
 * Clears the list of registered types of drops.
 */
void drop_category::clear_types() {
    for(auto &t : drop_types) {
        delete t.second;
    }
    drop_types.clear();
}


/* ----------------------------------------------------------------------------
 * Creates a drop and adds it to the list of drops.
 */
mob* drop_category::create_mob(
    const point &pos, mob_type* type, const float angle
) {
    drop* m = new drop(pos, (drop_type*) type, angle);
    drops.push_back(m);
    return m;
}


/* ----------------------------------------------------------------------------
 * Creates a new, empty type of drop.
 */
mob_type* drop_category::create_type() {
    return new drop_type();
}


/* ----------------------------------------------------------------------------
 * Clears a drop from the list of drops.
 */
void drop_category::erase_mob(mob* m) {
    drops.erase(
        find(drops.begin(), drops.end(), (drop*) m)
    );
}


/* ----------------------------------------------------------------------------
 * Returns a type of drop given its name, or NULL on error.
 */
mob_type* drop_category::get_type(const string &name) {
    auto it = drop_types.find(name);
    if(it == drop_types.end()) return NULL;
    return it->second;
}


/* ----------------------------------------------------------------------------
 * Returns all types of drop by name.
 */
void drop_category::get_type_names(vector<string> &list) {
    for(auto &t : drop_types) {
        list.push_back(t.first);
    }
}


/* ----------------------------------------------------------------------------
 * Registers a created type of drop.
 */
void drop_category::register_type(mob_type* type) {
    drop_types[type->name] = (drop_type*) type;
}
