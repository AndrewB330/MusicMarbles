/* MIT License
 * Copyright (c) 2020 Andrii Borziak
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "world.hpp"


std::pair<double, Vec2d> point_to_segment(Vec2d p, Vec2d a, Vec2d b) {
    // todo: there is possible some optimizations
    double proj = (p - a) * (b - a);
    double dist;
    Vec2d normal;
    if (proj < 0) {
        // a is the nearest point of segment to p
        dist = (p - a).len();
        normal = (p - a).normal();
    } else if (proj > (b - a).len_sqr()) {
        // b is the nearest point of segment to p
        dist = (p - b).len();
        normal = (p - b).normal();
    } else {
        // perpendicular projection of p on (a,b) is the nearest point
        dist = (p - a) ^ (b - a).normal();
        normal = -(b - a).rot90().normal();
        if (dist < 0) {
            normal = -normal;
            dist = -dist;
        }
    }
    return {dist, normal};
}

bool collide(Marble & a, Marble & b) {
    double dist_sqr = (a.position - b.position).len_sqr();
    if (dist_sqr >= (MARBLES_RADIUS * 2) * (MARBLES_RADIUS * 2)) {
        return false;
    }
    double dist = sqrt(dist_sqr);

    Vec2d relative_direction = (b.position - a.position);
    Vec2d relative_velocity = (b.velocity - a.velocity);

    // Note: masses are equal
    Vec2d impulse = (relative_direction * relative_velocity) * relative_direction / dist_sqr;

    a.velocity += impulse;
    b.velocity -= impulse;

    // Push the marbles apart of each other if they intersect
    double penetration_depth = (2 * MARBLES_RADIUS - dist);

    Vec2d normalized = relative_direction / dist;
    a.position -= normalized * penetration_depth * 0.5;
    b.position += normalized * penetration_depth * 0.5;

    return true;
}

bool collide(Marble &a, Plank &b) {
    double dist;
    Vec2d normal;
    std::tie(dist, normal) = point_to_segment(a.position, b.a, b.b);

    if (dist >= MARBLES_RADIUS + PLANKS_WIDTH) {
        return false;
    }

    double bounce_factor = (a.bounce_factor + b.bounce_factor) * 0.5;
    Vec2d impulse = (a.velocity * normal) * normal * (1 + bounce_factor);
    a.velocity -= impulse;

    // Push the marble out of plank
    // Note: plank has fixed position, we do not need to push it
    double depth = MARBLES_RADIUS + PLANKS_WIDTH - dist;
    a.position += normal * depth;

    return true;
}


void WorldState::start() {
    time_elapsed = 0;

    marbles_collisions.assign(marbles.size(), -1);

    marbles_history.push_back(marbles);
    marbles_collisions_history.push_back(marbles_collisions);
}

void WorldState::update(bool track_history) {
    marbles_collisions.assign(marbles.size(), -1);
    for (int m_tick = 0; m_tick < MICRO_TICKS_NUM; m_tick++) {
        for (int i = 0; i < marbles.size(); i++) {
            for (int j = i + 1; j < marbles.size(); j++) {
                collide(marbles[i], marbles[j]);
            }
            for (int j = 0; j < planks.size(); j ++) {
                if (collide(marbles[i], planks[j])) {
                    marbles_collisions[i] = j;
                }
            }
        }

        for (auto &marble : marbles) {
            marble.position += marble.velocity * SEC_PER_MICRO_TICK + GRAVITY * (SEC_PER_MICRO_TICK * SEC_PER_MICRO_TICK * 0.5);
            marble.velocity += GRAVITY * SEC_PER_MICRO_TICK;
        }
        if (track_history)
            marbles_history.push_back(marbles);
    }
    if (track_history)
        marbles_collisions_history.push_back(marbles_collisions);
    time_elapsed += MS_PER_TICK;
}

void WorldState::rewind_back() {
    time_elapsed -= MS_PER_TICK;
    for (int i = 0; i < MICRO_TICKS_NUM; i++) {
        marbles_history.pop_back();
    }
    marbles_collisions_history.pop_back();

    marbles_collisions = marbles_collisions_history.back();
    marbles = marbles_history.back();
}

void WorldState::reset() {
    time_elapsed = 0;
    marbles.clear();
    planks.clear();
    marbles_history.clear();
    marbles_collisions.clear();
    marbles_collisions_history.clear();
}