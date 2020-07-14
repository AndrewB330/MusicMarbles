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
#include <vector>
#include <algorithm>
#include <random>
#include "math.hpp"
#include "world.hpp"

const int MAX_TRACKS = 16;
const int PLANK_HALF_LENGTH = 18;
std::mt19937 rng(0);

// array of tracks, track is just a list of points in time when we should hit a plank
std::vector<std::vector<int>> tracks(MAX_TRACKS);

// World that is displayed on the frontend
WorldState main_world;

extern "C" {

void update_main_world() {
    main_world.simulate(false);
}

bool recurs(WorldState &state, int pointer = -1) {
    int simulations = 0;

    auto simulate = [&state, &simulations]() {
        state.simulate();
        simulations++;
    };
    auto rewind_back = [&state, &simulations] {
        while(simulations--) {
            state.rewind_back();
        }
    };

    simulate();
    if (state.marbles_collisions[0] != pointer) {
        rewind_back();
        return false;
    }

    pointer++;

    if (pointer >= tracks[0].size()) {
        for (int i = 0; i < 200; i++) {
            simulate();
            if (state.marbles_collisions[0] != -1) {
                rewind_back();
                return false;
            }
        }
        rewind_back();
        return true;
    }

    while (state.time_elapsed + MS_PER_TICK <= tracks[0][pointer]) {
        simulate();
        if (state.marbles_collisions[0] != -1) {
            rewind_back();
            return false;
        }
    }

    auto position = state.marbles[0].position;
    auto direction = state.marbles[0].velocity.normal();

    // todo: Hardcoded restrictions, not good
    if (abs(position.y) > 350) {
        rewind_back();
        return false;
    }
    /*
    if (pos.x - state.trace[0][track].pos.x > (30 * state.time_elapsed) / 1000 + 420)
        return false;
    */

    std::vector<Vec2d> plank_directions = {direction};

    for (double angle : {0.15, 0.6, 0.3, 0.9, 1.3}) {
        plank_directions.push_back(direction.rot(angle));
        plank_directions.push_back(direction.rot(-angle));
    }

    std::shuffle(plank_directions.begin(), plank_directions.end(), rng);

    for (auto d : plank_directions) {
        auto plank_center = position + d * (MARBLES_RADIUS + PLANKS_WIDTH + 1e-3);
        auto plank_a = plank_center + d.rot90() * PLANK_HALF_LENGTH;
        auto plank_b = plank_center - d.rot90() * PLANK_HALF_LENGTH;

        // Check if new plank could collide with marble back in time
        auto collide_with_marble_earlier = true;
        for (auto &history_slice : state.marbles_history) {
            for (auto &marble: history_slice) {
                if (point_to_segment(marble.position, plank_a, plank_b).first <= MARBLES_RADIUS + PLANKS_WIDTH) {
                    collide_with_marble_earlier = false;
                    break;
                }
            }
        }
        if (!collide_with_marble_earlier) continue;


        state.planks.push_back(Plank{plank_a, plank_b});
        if (recurs(state, pointer)) {
            rewind_back(); // Undo simulation 1
            return true;
        }

        state.planks.pop_back();
    }
    rewind_back(); // Undo simulation 1
    return false;
}

void regenerate_world() {
    for (auto &track : tracks) {
        std::sort(track.begin(), track.end());
    }
    Marble initial = Marble{Vec2d(0, -340), Vec2d()};

    WorldState state;
    state.reset();
    state.marbles.push_back(initial);
    state.start();

    recurs(state);

    main_world.planks = state.planks;
    main_world.marbles.push_back(initial);
    main_world.start();
}

void init_world() {
    main_world.reset();
    // Add some objects
    // Placeholder function
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
    tracks[track].push_back(time);
}

}

int main() {
    return 0;
}