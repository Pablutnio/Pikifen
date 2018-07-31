/*
 * Copyright (c) Andre 'Espyo' Silva 2013-2018.
 * The following source file belongs to the open-source project
 * Pikifen. Please read the included
 * README and LICENSE files for more information.
 * Pikmin is copyright (c) Nintendo.
 *
 * === FILE DESCRIPTION ===
 * Geometry-related utility functions.
 */

#include <algorithm>

#include "geometry_utils.h"
#include "functions.h"


/* ----------------------------------------------------------------------------
 * Constructs a point, given its coordinates.
 */
point::point(const float x, const float y) :
    x(x),
    y(y) {
    
}


/* ----------------------------------------------------------------------------
 * Constructs a point, with the coordinates set to 0,0.
 */
point::point() :
    x(0),
    y(0) {
    
}


/* ----------------------------------------------------------------------------
 * Adds the coordinates of two points.
 */
const point point::operator +(const point &p) const {
    return point(x + p.x, y + p.y);
}


/* ----------------------------------------------------------------------------
 * Subtracts the coordinates of two points.
 */
const point point::operator -(const point &p) const {
    return point(x - p.x, y - p.y);
}


/* ----------------------------------------------------------------------------
 * Multiplies the coordinates of two points.
 */
const point point::operator *(const point &p) const {
    return point(x * p.x, y * p.y);
}


/* ----------------------------------------------------------------------------
 * Divides the coordinates of two points.
 */
const point point::operator /(const point &p) const {
    return point(x / p.x, y / p.y);
}


/* ----------------------------------------------------------------------------
 * Adds a number to the coordinates.
 */
const point point::operator +(const float n) const {
    return point(x + n, y + n);
}


/* ----------------------------------------------------------------------------
 * Subtracts a number from each coordinates.
 */
const point point::operator -(const float n) const {
    return point(x - n, y - n);
}


/* ----------------------------------------------------------------------------
 * Divides the coordinates by a number.
 */
const point point::operator /(const float n) const {
    return point(x / n, y / n);
}


/* ----------------------------------------------------------------------------
 * Adds the coordinates of another point to this one's.
 */
point point::operator +=(const point &p) {
    x += p.x;
    y += p.y;
    return point(x, y);
}


/* ----------------------------------------------------------------------------
 * Subtracts the coordinates of another point to this one's.
 */
point point::operator -=(const point &p) {
    x -= p.x;
    y -= p.y;
    return point(x, y);
}


/* ----------------------------------------------------------------------------
 * Adds a given number to the coordinates.
 */
point point::operator +=(const float n) {
    x += n;
    y += n;
    return point(x, y);
}


/* ----------------------------------------------------------------------------
 * Multiplies the coordinates by a given number.
 */
point point::operator *=(const float n) {
    x *= n;
    y *= n;
    return point(x, y);
}


/* ----------------------------------------------------------------------------
 * Compares if two points are the same.
 */
const bool point::operator ==(const point &p) const {
    return x == p.x && y == p.y;
}


/* ----------------------------------------------------------------------------
 * Compares if two points are different.
 */
const bool point::operator !=(const point &p) const {
    return x != p.x || y != p.y;
}


/* ----------------------------------------------------------------------------
 * Multiplies the coordinates by a number.
 */
const point point::operator *(const float m) const {
    return point(x * m, y * m);
}



/* ----------------------------------------------------------------------------
 * Returns the vector coordinates of an angle.
 * angle:     The angle.
 * magnitude: Its magnitude.
 */
point angle_to_coordinates(
    const float angle, const float magnitude
) {
    return point(cos(angle) * magnitude, sin(angle) * magnitude);
}


/* ----------------------------------------------------------------------------
 * Converts angular distance to linear distance.
 */
float angular_dist_to_linear(const float angular_dist, const float radius) {
    return 2 * radius * tan(angular_dist / 2);
}


/* ----------------------------------------------------------------------------
 * Checks if two spheres are colliding via a bounding-box check.
 * center1: Coordinates of the first sphere.
 * center2: Coordinates of the second sphere.
 * r:       Range of the bounding box.
 */
bool bbox_check(const point &center1, const point &center2, const float r) {
    return
        (
            fabs(center1.x - center2.x) <= r &&
            fabs(center1.y - center2.y) <= r
        );
}


/* ----------------------------------------------------------------------------
 * Checks if a rectangle and a sphere are colliding via a bounding-box check.
 * tl1:     Top-left coordinates of the rectangle.
 * br1:     Bottom-right coordinates of the rectangle.
 * center2: Coordinates of the sphere.
 * r:       Radius of the sphere.
 */
bool bbox_check(
    const point &tl1, const point &br1,
    const point &center2, const float r
) {
    return
        rectangles_intersect(
            tl1, br1,
            center2 - r, center2 + r
        );
}


/* ----------------------------------------------------------------------------
 * Returns whether a circle is touching a line segment or not.
 * circle: Coordinates of the circle.
 * radius: Radius of the circle.
 * line_*: Coordinates of the line.
 * li*:    If not NULL, the line intersection coordinates are returned here.
 */
bool circle_intersects_line(
    const point &circle, const float radius,
    const point &line_p1, const point &line_p2,
    float* lix, float* liy
) {

    //Code by
    //  http://www.melloland.com/scripts-and-tutos/
    //  collision-detection-between-circles-and-lines
    
    float vx = line_p2.x - line_p1.x;
    float vy = line_p2.y - line_p1.y;
    float xdiff = line_p1.x - circle.x;
    float ydiff = line_p1.y - circle.y;
    float a = vx * vx + vy * vy;
    float b = 2 * ((vx * xdiff) + (vy * ydiff));
    float c = xdiff * xdiff + ydiff * ydiff - radius * radius;
    float quad = b * b - (4 * a * c);
    if (quad >= 0) {
        //An infinite collision is happening, but let's not stop here
        float quadsqrt = sqrt(quad);
        for (int i = -1; i <= 1; i += 2) {
            //Returns the two coordinates of the intersection points
            float t = (i * -b + quadsqrt) / (2 * a);
            float x = line_p1.x + (i * vx * t);
            float y = line_p1.y + (i * vy * t);
            //If one of them is in the boundaries of the segment, it collides
            if (
                x >= min(line_p1.x, line_p2.x) &&
                x <= max(line_p1.x, line_p2.x) &&
                y >= min(line_p1.y, line_p2.y) &&
                y <= max(line_p1.y, line_p2.y)
            ) {
                if(lix) *lix = x;
                if(liy) *liy = y;
                return true;
            }
        }
    }
    return false;
}


/* ----------------------------------------------------------------------------
 * Returns whether a circle is touching a rotated rectangle or not.
 * circle:               Coordinates of the circle.
 * radius:               Radius of the circle.
 * rectangle:            Central coordinates of the rectangle.
 * rect_dim:             Dimensions of the rectangle.
 * rect_angle:           Angle the rectangle is facing.
 * overlap_dist:         If not NULL, the amount of overlap is returned here.
 * rectangle_side_angle: If not NULL, the angle of the side of the rectangle
 *   that the circle is on, aligned to the sides of the rectangle, is
 *   returned here.
 */
bool circle_intersects_rectangle(
    const point &circle, const float radius,
    const point &rectangle, const point &rect_dim,
    const float rect_angle,
    float* overlap_dist, float* rectangle_side_angle
) {
    point circle_rel_pos = circle - rectangle;
    
    circle_rel_pos = rotate_point(circle_rel_pos, -rect_angle);
    
    point nearest(
        clamp(circle_rel_pos.x, -rect_dim.x / 2.0, rect_dim.x / 2.0),
        clamp(circle_rel_pos.y, -rect_dim.y / 2.0, rect_dim.y / 2.0)
    );
    
    float d = dist(circle_rel_pos, nearest).to_float();
    if(overlap_dist) {
        *overlap_dist = radius - d;
    }
    
    if(rectangle_side_angle) {
        float angle = get_angle(nearest, circle_rel_pos);
        angle = floor((angle + M_PI_4) / M_PI_2) * M_PI_2;
        *rectangle_side_angle = angle + rect_angle;
    }
    
    return d < radius;
}


/* ----------------------------------------------------------------------------
 * Converts an angle from degrees to radians.
 */
float deg_to_rad(const float deg) {
    return (M_PI / 180.0f) * deg;
}


/* ----------------------------------------------------------------------------
 * Returns the angle between two points.
 * In other words, this is the angle "center" is facing when it is looking
 * at "focus".
 */
float get_angle(const point &center, const point &focus) {
    return atan2(focus.y - center.y, focus.x - center.x);
}


/* ----------------------------------------------------------------------------
 * Returns the closest point in a line to a given point.
 *   l1, l2:      Points of the line.
 * p:             Reference point.
 * segment_ratio: If not NULL, the ratio from l1 to l2 is returned here.
 *   Between 0 and 1, it belongs to the line segment. If not, it doesn't.
 */
point get_closest_point_in_line(
    const point &l1, const point &l2, const point &p, float* segment_ratio
) {

    //Code by http://stackoverflow.com/a/3122532
    
    point l1_to_p = p - l1;
    point l1_to_l2 = l2 - l1;
    
    float l1_to_l2_squared =
        l1_to_l2.x * l1_to_l2.x +
        l1_to_l2.y * l1_to_l2.y;
        
    float l1_to_p_dot_l1_to_l2 =
        l1_to_p.x * l1_to_l2.x +
        l1_to_p.y * l1_to_l2.y;
        
    float r = l1_to_p_dot_l1_to_l2 / l1_to_l2_squared;
    
    if(segment_ratio) *segment_ratio = r;
    
    return point(l1.x + l1_to_l2.x * r, l1.y + l1_to_l2.y * r);
}


/* ----------------------------------------------------------------------------
 * Gets the bounding box coordinates of a rectangle that has undergone
 * translation, scale, and/or rotation transformations, and places it
 * in the specified point structs.
 */
void get_transformed_rectangle_bounding_box(
    const point &center, const point &dimensions, const float angle,
    point* min_coords, point* max_coords
) {

    if(!min_coords || !max_coords) return;
    bool got_min_x = false;
    bool got_max_x = false;
    bool got_min_y = false;
    bool got_max_y = false;
    
    for(unsigned char p = 0; p < 4; ++p) {
        point corner, final_corner;
        
        if(p == 0 || p == 1) corner.x = center.x - (dimensions.x * 0.5);
        else                 corner.x = center.x + (dimensions.x * 0.5);
        if(p == 0 || p == 2) corner.y = center.y - (dimensions.y * 0.5);
        else                 corner.y = center.y + (dimensions.y * 0.5);
        
        corner -= center;
        final_corner = rotate_point(corner, angle);
        final_corner += center;
        
        if(final_corner.x < min_coords->x || !got_min_x) {
            min_coords->x = final_corner.x;
            got_min_x = true;
        }
        if(final_corner.y < min_coords->y || !got_min_y) {
            min_coords->y = final_corner.y;
            got_min_y = true;
        }
        if(final_corner.x > max_coords->x || !got_max_x) {
            max_coords->x = final_corner.x;
            got_max_x = true;
        }
        if(final_corner.y > max_coords->y || !got_max_y) {
            max_coords->y = final_corner.y;
            got_max_y = true;
        }
    }
}


/* ----------------------------------------------------------------------------
 * Converts linear distance to angular distance.
 */
float linear_dist_to_angular(const float linear_dist, const float radius) {
    return 2 * atan(linear_dist / (2 * radius));
}


/* ----------------------------------------------------------------------------
 * Returns the movement necessary to move a point.
 * start:        Coordinates of the initial point.
 * target:       Coordinates of the target point.
 * speed:        Speed at which the point can move.
 * reach_radius: If the point is within this range of the target,
 *   consider it as already being there.
 * mov:          Variable to return the amount of movement to.
 * angle:        Variable to return the angle the point faces to.
 * reached:      Variable to return whether the point reached the target.
 * delta_t:      Duration of the current tick.
 */
void move_point(
    const point &start, const point &target,
    const float speed, const float reach_radius,
    point* mov, float* angle, bool* reached, const float delta_t
) {
    point dif = target - start;
    float dis = sqrt(dif.x * dif.x + dif.y * dif.y);
    
    if(dis > reach_radius) {
        float move_amount =
            min((double) (dis / delta_t / 2.0f), (double) speed);
            
        dif *= (move_amount / dis);
        
        if(mov) *mov = dif;
        if(angle) *angle = atan2(dif.y, dif.x);
        if(reached) *reached = false;
        
    } else {
    
        if(mov) *mov = point();
        if(reached) *reached = true;
    }
}


/* ----------------------------------------------------------------------------
 * Normalizes an angle so that it's between 0 and M_PI * 2.
 */
float normalize_angle(float a) {
    a = fmod((double) a, M_PI * 2);
    if(a < 0) a += M_PI * 2;
    return a;
}


/* ----------------------------------------------------------------------------
 * Converts an angle from radians to degrees.
 */
float rad_to_deg(const float rad) {
    return (180.0f / M_PI) * rad;
}


/* ----------------------------------------------------------------------------
 * Returns whether a rectangle intersects with a line segment.
 * Also returns true if the line is fully inside the rectangle.
 * r*: Rectangle coordinates.
 * l*: Line coordinates.
 */
bool rectangle_intersects_line(
    const point &r1, const point &r2,
    const point &l1, const point &l2
) {
    //Line crosses left side?
    if(
        lines_intersect(
            l1, l2, point(r1.x, r1.y), point(r1.x, r2.y), NULL, NULL
        )
    ) {
        return true;
    }
    //Line crosses right side?
    if(
        lines_intersect(
            l1, l2, point(r2.x, r1.y), point(r2.x, r2.y), NULL, NULL
        )
    ) {
        return true;
    }
    //Line crosses top side?
    if(
        lines_intersect(
            l1, l2, point(r1.x, r1.y), point(r2.x, r1.y), NULL, NULL
        )
    ) {
        return true;
    }
    //Line crosses bottom side?
    if(
        lines_intersect(
            l1, l2, point(r1.x, r2.y), point(r2.x, r2.y), NULL, NULL
        )
    ) {
        return true;
    }
    
    //Are both points inside the rectangle?
    if(
        (l1.x >= r1.x && l2.x >= r1.x) &&
        (l1.x <= r2.x && l2.x <= r2.x) &&
        (l1.y >= r1.y && l2.y >= r1.y) &&
        (l1.y <= r2.y && l2.y <= r2.y)
    ) {
        return true;
    }
    
    return false;
    
}


/* ----------------------------------------------------------------------------
 * Checks if two rectangles are colliding.
 * tl1: Coordinates of the first box's top-left.
 * br1: Coordinates of the first box's bottom-right.
 * tl2: Coordinates of the second box's top-left.
 * br2: Coordinates of the second box's bottom-right.
 */
bool rectangles_intersect(
    const point &tl1, const point &br1,
    const point &tl2, const point &br2
) {
    if(tl1.x > br2.x) return false;
    if(br1.x < tl2.x) return false;
    if(tl1.y > br2.y) return false;
    if(br1.y < tl2.y) return false;
    return true;
}


/* ----------------------------------------------------------------------------
 * Rotates a point by an angle.
 * The x and y are meant to represent the difference
 * between the point and the center of the rotation.
 */
point rotate_point(const point &coords, const float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return point(c * coords.x - s * coords.y, s * coords.x + c * coords.y);
}
