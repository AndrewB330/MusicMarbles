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

#ifndef MUSIC_MARBLES_WORLD_H
#define MUSIC_MARBLES_WORLD_H

#include <vector>
#include "math.hpp"

/*
 * All the world objects and physics described in this file
 * We have two possible objects: Marble and Plank
 * Marble is just a ball that can fly, fall, affected by gravity and can bounce off the planks
 * Plank is a "thick" segment between two points, it's position is fixed
 *
 * WorldState describes all the objects that are present in the world at some point in time
 * It can simulate one tick of physics or can rewind one tick back in time if track_history was enabled
 * And for each marble we store the index of plank that was hit during the last tick, this is needed
 * to make a matching between sounds and hits on the JS side
 */

const int MS_PER_TICK = 15;
const int MICRO_TICKS_NUM = 4; // Number of sub-ticks of simulation inside one tick
const double SEC_PER_MICRO_TICK = MS_PER_TICK / 1000.0 / MICRO_TICKS_NUM;

const Vec2d GRAVITY = Vec2d(0, 9.8 * 10 * 3);

const int MARBLES_RADIUS = 16;
const int PLANKS_WIDTH = 4; // todo: in fact, this is half of the width

struct Marble {
    Vec2d position;
    Vec2d velocity;
    int stored_value = 0; // use it if you want to store some information
    double bounce_factor = 1.0;
};

struct Plank {
    Vec2d a;
    Vec2d b;
    int stored_value = 0; // use it if you want to store some information
    double bounce_factor = 1.0;
};

struct WorldState {
    std::vector<Marble> marbles;
    std::vector<Plank> planks;

    // for each marble we store an index of last collided plank during the last tick
    // or -1 if the marble didn't collide during tha last tick
    std::vector<int> marbles_collisions;

    // History, needed to rewind the WorldState back in time
    std::vector<std::vector<Marble>> marbles_history;
    std::vector<std::vector<int>> marbles_collisions_history;

    // elapsed milliseconds since the start of the simulation
    int time_elapsed = 0;

    /*
     * Initialize before(!) simulation
     */
    void start();

    /*
     * Simulate one tick
     */
    void simulate(bool track_history = true);

    /*
     * Rewind one tick back in time
     */
    void rewind_back();

    /*
     * Reset the world
     */
    void reset();
};

/*
 * Check two marbles a and b for collision, resolve the collision if they collide
 * Returns true if the collision happened and was resolved, false otherwise
 */
bool collide(Marble & a, Marble & b);

/*
 * Check marble a and plank b for collision, resolve the collision if they collide
 * Returns true if the collision happened and was resolved, false otherwise
 */
bool collide(Marble &a, Plank &b);

/*
 * Calculate distance and relative direction from point p to segment (a, b)
 * Returns pair of distance and relative direction (normalized vector)
 */
std::pair<double, Vec2d> point_to_segment(Vec2d p, Vec2d a, Vec2d b);



#endif //MUSIC_MARBLES_WORLD_H
