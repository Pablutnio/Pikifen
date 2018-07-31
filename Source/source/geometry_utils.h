/*
 * Copyright (c) Andre 'Espyo' Silva 2013-2018.
 * The following source file belongs to the open-source project
 * Pikifen. Please read the included
 * README and LICENSE files for more information.
 * Pikmin is copyright (c) Nintendo.
 *
 * === FILE DESCRIPTION ===
 * Header for geometry-related utility functions.
 */

#ifndef GEOMETRY_UTILS_INCLUDED
#define GEOMETRY_UTILS_INCLUDED

#define _USE_MATH_DEFINES
#include <cstddef>
#include <math.h>

/* ----------------------------------------------------------------------------
 * Simple 2D point.
 */
struct point {
    float x, y;
    
    point(const float x, const float y);
    point();
    const point operator +(const point &p) const;
    const point operator -(const point &p) const;
    const point operator *(const point &p) const;
    const point operator /(const point &p) const;
    const point operator +(const float n) const;
    const point operator -(const float n) const;
    const point operator /(const float n) const;
    point operator +=(const point &p);
    point operator -=(const point &p);
    point operator +=(const float n);
    point operator *=(const float n);
    const bool operator ==(const point &p) const;
    const bool operator !=(const point &p) const;
    const point operator *(const float m) const;
};


point angle_to_coordinates(
    const float angle, const float magnitude
);
float angular_dist_to_linear(const float angular_dist, const float radius);
bool bbox_check(const point &center1, const point &center2, const float r);
bool bbox_check(
    const point &tl1, const point &br1,
    const point &center2, const float r
);
bool circle_intersects_line(
    const point &circle, const float cr,
    const point &line_p1, const point &line_p2,
    float* lix = NULL, float* liy = NULL
);
bool circle_intersects_rectangle(
    const point &circle, const float cr,
    const point &rectangle, const point &rect_dim,
    const float rect_angle,
    float* overlap_dist = NULL, float* rectangle_side_angle = NULL
);
float deg_to_rad(const float rad);
float get_angle(const point &center, const point &focus);
point get_closest_point_in_line(
    const point &l1, const point &l2, const point &p,
    float* segment_ratio = NULL
);
void get_transformed_rectangle_bounding_box(
    const point &center, const point &dimensions, const float angle,
    point* min_coords, point* max_coords
);
float linear_dist_to_angular(const float linear_dist, const float radius);
void move_point(
    const point &start, const point &target,
    const float speed, const float reach_radius, point* mov,
    float* angle, bool* reached, const float delta_t
);
float normalize_angle(float a);
float rad_to_deg(const float deg);
bool rectangle_intersects_line(
    const point &r1, const point &r2,
    const point &l1, const point &l2
);
bool rectangles_intersect(
    const point &tl1, const point &br1,
    const point &tl2, const point &br2
);
point rotate_point(const point &coords, const float angle);

#endif //ifndef GEOMETRY_UTILS_INCLUDED
