/*
 * Copyright (c) Andre 'Espyo' Silva 2013-2018.
 * The following source file belongs to the open-source project
 * Pikifen. Please read the included
 * README and LICENSE files for more information.
 * Pikmin is copyright (c) Nintendo.
 *
 * === FILE DESCRIPTION ===
 * Resource type class and resource type-related functions.
 */

#include "../functions.h"
#include "../mobs/resource.h"
#include "../mobs/resource_fsm.h"
#include "resource_type.h"
#include "../utils/string_utils.h"

/* ----------------------------------------------------------------------------
 * Creates a type of resource.
 */
resource_type::resource_type() :
    mob_type(MOB_CATEGORY_RESOURCES),
    vanish_on_drop(false),
    return_to_pile_on_vanish(false),
    time_to_vanish(0.0f),
    delivery_result(RESOURCE_DELIVERY_RESULT_ADD_POINTS),
    damage_mob_amount(1.0f),
    spray_to_concoct(INVALID),
    point_amount(1.0f) {
    
    resource_fsm::create_fsm(this);
}


/* ----------------------------------------------------------------------------
 * Loads parameters from a data file.
 */
void resource_type::load_parameters(data_node* file) {
    reader_setter rs(file);
    string delivery_result_str;
    string spray_to_concoct_str;
    rs.set("vanish_on_drop", vanish_on_drop);
    rs.set("return_to_pile_on_vanish", return_to_pile_on_vanish);
    rs.set("time_to_vanish", time_to_vanish);
    rs.set("delivery_result", delivery_result_str);
    rs.set("damage_mob_amount", damage_mob_amount);
    rs.set("spray_to_concoct", spray_to_concoct_str);
    rs.set("point_amount", point_amount);
    
    if(delivery_result_str == "damage_mob") {
        delivery_result = RESOURCE_DELIVERY_RESULT_DAMAGE_MOB;
    } else if(delivery_result_str == "increase_ingredients") {
        delivery_result = RESOURCE_DELIVERY_RESULT_INCREASE_INGREDIENTS;
    } else if(delivery_result_str == "add_points") {
        delivery_result = RESOURCE_DELIVERY_RESULT_ADD_POINTS;
    } else {
        log_error(
            "Unknown delivery result \"" + delivery_result_str + "\"!",
            file
        );
    }
    
    if(delivery_result == RESOURCE_DELIVERY_RESULT_INCREASE_INGREDIENTS) {
        for(size_t s = 0; s < spray_types.size(); ++s) {
            if(spray_types[s].name == spray_to_concoct_str) {
                spray_to_concoct = s;
                break;
            }
        }
        if(spray_to_concoct == INVALID) {
            log_error(
                "Unknown spray type \"" + spray_to_concoct_str + "\"!",
                file
            );
        }
    }
}


/* ----------------------------------------------------------------------------
 * Returns the vector of animation conversions.
 */
anim_conversion_vector resource_type::get_anim_conversions() {
    anim_conversion_vector v;
    v.push_back(make_pair(RESOURCE_ANIM_IDLING, "idling"));
    return v;
}


resource_type::~resource_type() { }
