/*
 * Copyright (c) Andre 'Espyo' Silva 2013-2018.
 * The following source file belongs to the open-source project
 * Pikifen. Please read the included
 * README and LICENSE files for more information.
 * Pikmin is copyright (c) Nintendo.
 *
 * === FILE DESCRIPTION ===
 * Header for the special mob category class.
 */

#ifndef SPECIAL_CATEGORY_INCLUDED
#define SPECIAL_CATEGORY_INCLUDED

#include <string>
#include <vector>

#include "../mobs/mob_category.h"

using namespace std;


/* ----------------------------------------------------------------------------
 * Mob category for the special mob types.
 */
class special_category : public mob_category {
public:
    virtual void get_type_names(vector<string> &list);
    virtual mob_type* get_type(const string &name);
    virtual mob_type* create_type();
    virtual void register_type(mob_type* type);
    virtual mob* create_mob(
        const point &pos, mob_type* type, const float angle
    );
    virtual void erase_mob(mob* m);
    virtual void clear_types();
    
    special_category();
    ~special_category();
};

#endif //ifndef SPECIAL_CATEGORY_INCLUDED
