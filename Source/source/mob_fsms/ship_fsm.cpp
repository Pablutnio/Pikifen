/*
 * Copyright (c) Andre 'Espyo' Silva 2013.
 * The following source file belongs to the open-source project Pikifen.
 * Please read the included README and LICENSE files for more information.
 * Pikmin is copyright (c) Nintendo.
 *
 * === FILE DESCRIPTION ===
 * Ship finite state machine logic.
 */

#include "ship_fsm.h"

#include "../functions.h"
#include "../game.h"
#include "../mobs/resource.h"
#include "../mobs/ship.h"
#include "../particle.h"
#include "../utils/string_utils.h"


/* ----------------------------------------------------------------------------
 * Creates the finite state machine for the ship's logic.
 * typ:
 *   Mob type to create the finite state machine for.
 */
void ship_fsm::create_fsm(mob_type* typ) {
    easy_fsm_creator efc;
    
    efc.new_state("idling", SHIP_STATE_IDLING); {
        efc.new_event(MOB_EV_ON_ENTER); {
            efc.run(ship_fsm::set_anim);
        }
        efc.new_event(MOB_EV_RECEIVE_DELIVERY); {
            efc.run(ship_fsm::receive_mob);
        }
    }
    
    typ->states = efc.finish();
    typ->first_state_nr = fix_states(typ->states, "idling");
    
    //Check if the number in the enum and the total match up.
    engine_assert(
        typ->states.size() == N_SHIP_STATES,
        i2s(typ->states.size()) + " registered, " +
        i2s(N_SHIP_STATES) + " in enum."
    );
}


/* ----------------------------------------------------------------------------
 * When a ship finishes receiving a mob carried by Pikmin.
 * m:
 *   The mob.
 * info1:
 *   Pointer to the mob.
 * info2:
 *   Unused.
 */
void ship_fsm::receive_mob(mob* m, void* info1, void* info2) {
    engine_assert(info1 != NULL, m->print_state_history());
    
    mob* delivery = (mob*) info1;
    ship* s_ptr = (ship*) m;
    
    switch(delivery->type->category->id) {
    case MOB_CATEGORY_TREASURES: {
        break;
    } case MOB_CATEGORY_RESOURCES: {
        resource* r_ptr = (resource*) delivery;
        if(
            r_ptr->res_type->delivery_result ==
            RESOURCE_DELIVERY_RESULT_ADD_POINTS
        ) {
        
        } else if(
            r_ptr->res_type->delivery_result ==
            RESOURCE_DELIVERY_RESULT_INCREASE_INGREDIENTS
        ) {
            size_t type_nr = r_ptr->res_type->spray_to_concoct;
            game.states.gameplay->spray_stats[type_nr].nr_ingredients++;
            if(
                game.states.gameplay->spray_stats[type_nr].nr_ingredients >=
                game.spray_types[type_nr].ingredients_needed
            ) {
                game.states.gameplay->spray_stats[type_nr].nr_ingredients -=
                    game.spray_types[type_nr].ingredients_needed;
                game.states.gameplay->spray_stats[type_nr].nr_sprays++;
            }
        }
        break;
    }
    }
    
    particle p(
        PARTICLE_TYPE_BITMAP, s_ptr->beam_final_pos,
        s_ptr->z + s_ptr->height, 24, 1.5, PARTICLE_PRIORITY_MEDIUM
    );
    p.bitmap = game.sys_assets.bmp_smoke;
    particle_generator pg(0, p, 15);
    pg.number_deviation = 5;
    pg.angle = 0;
    pg.angle_deviation = TAU / 2;
    pg.total_speed = 70;
    pg.total_speed_deviation = 10;
    pg.duration_deviation = 0.5;
    pg.emit(game.states.gameplay->particles);
    
}


/* ----------------------------------------------------------------------------
 * When a ship needs to enter its default "idling" animation.
 * m:
 *   The mob.
 * info1:
 *   Unused.
 * info2:
 *   Unused.
 */
void ship_fsm::set_anim(mob* m, void* info1, void* info2) {
    m->set_animation(SHIP_ANIM_IDLING);
}
