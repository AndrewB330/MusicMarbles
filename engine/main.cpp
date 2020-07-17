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

#include <cmath>
#include "world_generator.hpp"

// World that is displayed on the frontend
WorldState main_world;
WorldGeneratorIterative generator({});


const int MAX_TRACKS = 16;
// array of tracks, track is just a list of points in time when we should hit a plank
// but for now we are using only 0-th track (!)
std::vector<std::vector<int>> TRACKS(MAX_TRACKS);

// WASM interface
extern "C" {

void update_main_world() {
    main_world.simulate(false);
}

void generate_world_limited() {
    generator.generate(50);
    main_world = generator.get_world();
}

int get_generator_progress() {
    return generator.get_progress();
}

void init_generator() {
    generator = WorldGeneratorIterative(TRACKS[0]);
}

/*
 * Deprecated
 * todo: remove
 */
void generate_world_() {
    WorldGeneratorRecursive gen(TRACKS[0]);
    gen.generate();
    main_world = gen.get_world();
}

int num_marbles() {
    return main_world.marbles.size();
}

int num_planks() {
    return main_world.planks.size();
}

int get_marble_x(size_t i) {
    return main_world.marbles[i].position.x;
}

int get_marble_y(size_t i) {
    return main_world.marbles[i].position.y;
}

int get_plank_a_x(size_t i) {
    return main_world.planks[i].a.x;
}

int get_plank_a_y(size_t i) {
    return main_world.planks[i].a.y;
}

int get_plank_b_x(size_t i) {
    return main_world.planks[i].b.x;
}

int get_plank_b_y(size_t i) {
    return main_world.planks[i].b.y;
}

int get_marble_collision(int i) {
    return main_world.marbles_collisions[i];
}

void add_time_of_hit(int track, int time) {
    TRACKS[track].push_back(time);
}

void reset_tracks() {
    TRACKS.clear();
    TRACKS.resize(MAX_TRACKS);
}

}

int main() {
    return 0;
}