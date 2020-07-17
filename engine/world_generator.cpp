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

#include "world_generator.hpp"

#define REWIND_BACK while(stack_state.simulations--) {world_state.rewind_back();}

#define RETURN_FALSE stack.pop_back(); continue;

const int PRNG_SEED = 0;
std::mt19937 GLOBAL_PRNG(PRNG_SEED);
const int PLANK_HALF_LENGTH = 14;

const std::vector<double> PLANK_ROTATIONS = {0, +0.15, -0.15, +0.6, -0.6, +0.3, -0.3, +0.9, -0.9, +1.1, -1.1};

std::vector<int> get_random_permutation(int n) {
    std::vector<int> permutation(n);
    for (int i = 0; i < n; i++) permutation[i] = i;
    std::shuffle(permutation.begin(), permutation.end(), GLOBAL_PRNG);
    return permutation;
}

StackState::StackState(int pointer) : pointer(pointer) {}

WorldGenerator::WorldGenerator(std::vector<int> track) : track(track) {
    world_state.marbles.push_back(Marble{Vec2d(0, start_y)});
    world_state.start();
}

WorldState WorldGenerator::get_world() {
    WorldState res;
    res.planks = world_state.planks;
    res.marbles.push_back(Marble{Vec2d(0, start_y)});
    res.start();
    return res;
}

WorldGeneratorIterative::WorldGeneratorIterative(std::vector<int> track_) : WorldGenerator(track_) {
    stack.push_back(StackState(-1));
}

int WorldGeneratorIterative::get_progress() {
    if (track.empty())
        return 100;
    return world_state.planks.size() * 100 / track.size();
}

void WorldGeneratorIterative::generate(int iterations) {
    for (int iter = 0; iter < iterations && !stack.empty(); iter++) {
        // Keep track of the max number of planks generated, and number of iterations without improvements
        if (world_state.planks.size() > max_planks_num_generated) {
            max_planks_num_generated = world_state.planks.size();
            num_iterations_no_improvement = 0;
        } else {
            num_iterations_no_improvement++;
        }


        StackState &stack_state = stack.back();

        if (stack_state.index == -1) {
            world_state.simulate();
            stack_state.simulations++;
            if (world_state.marbles_collisions.at(0) != stack_state.pointer) {
                REWIND_BACK;
                RETURN_FALSE;
            }
            stack_state.pointer++;
            if (stack_state.pointer >= track.size()) {
                bool ok = true;
                for (int i = 0; i < 200; i++) {
                    world_state.simulate();
                    stack_state.simulations++;
                    if (world_state.marbles_collisions.at(0) != -1) {
                        ok = false;
                        break;
                    }
                }
                if (!ok) {
                    REWIND_BACK;
                    RETURN_FALSE;
                }
                stack.clear();
                while (world_state.time_elapsed > 0) {
                    world_state.rewind_back();
                }
                continue;
            }
            bool ok = true;
            while (world_state.time_elapsed + MS_PER_TICK <= track.at(stack_state.pointer)) {
                world_state.simulate();
                stack_state.simulations++;
                if (world_state.marbles_collisions.at(0) != -1) {
                    ok = false;
                    break;
                }
            }
            if (!ok) {
                REWIND_BACK;
                RETURN_FALSE;
            }

            if (world_state.marbles.at(0).position.y > max_y || world_state.marbles.at(0).position.y < min_y) {
                REWIND_BACK;
                RETURN_FALSE;
            }
            stack_state.permutation = get_random_permutation(PLANK_ROTATIONS.size());
        }
        if (stack_state.plank_added) {
            world_state.planks.pop_back();
            stack_state.plank_added = false;
        }
        // Reset 10% of the generated map to shake the generator
        if (num_iterations_no_improvement > 2000) {
            if (world_state.planks.size() > max_planks_num_generated - 0.1 * track.size()) {
                REWIND_BACK;
                RETURN_FALSE;
            } else {
                num_iterations_no_improvement = 0;
                max_planks_num_generated = world_state.planks.size();
            }
        }
        stack_state.index++;
        if (stack_state.index >= stack_state.permutation.size()) {
            REWIND_BACK;
            RETURN_FALSE;
        }

        auto position = world_state.marbles.at(0).position;
        auto direction = world_state.marbles.at(0).velocity.normal();
        auto plank_direction = direction.rot(PLANK_ROTATIONS.at(stack_state.permutation.at(stack_state.index)));
        auto plank_center = position + plank_direction * (MARBLES_RADIUS + PLANKS_WIDTH + 1e-3);
        auto plank_a = plank_center + plank_direction.rot90() * PLANK_HALF_LENGTH;
        auto plank_b = plank_center - plank_direction.rot90() * PLANK_HALF_LENGTH;

        auto collide_with_marble_earlier = true;
        for (auto &history_slice : world_state.marbles_history) {
            for (auto &marble: history_slice) {
                if (point_to_segment(marble.position, plank_a, plank_b).first <= MARBLES_RADIUS + PLANKS_WIDTH) {
                    collide_with_marble_earlier = false;
                    break;
                }
            }
        }
        if (!collide_with_marble_earlier) continue;

        world_state.planks.push_back(Plank{plank_a, plank_b});
        stack_state.plank_added = true;
        stack.push_back(StackState(stack_state.pointer));
    }
}

bool WorldGeneratorRecursive::recurs(int pointer) {
    int simulations = 0;

    auto simulate = [&]() {
        world_state.simulate();
        simulations++;
    };
    auto rewind_back = [&] {
        while (simulations--) {
            world_state.rewind_back();
        }
    };

    simulate();
    if (world_state.marbles_collisions[0] != pointer) {
        rewind_back();
        return false;
    }

    pointer++;

    if (pointer >= track.size()) {
        for (int i = 0; i < 200; i++) {
            simulate();
            if (world_state.marbles_collisions[0] != -1) {
                rewind_back();
                return false;
            }
        }
        rewind_back();
        return true;
    }

    while (world_state.time_elapsed + MS_PER_TICK <= track[pointer]) {
        simulate();
        if (world_state.marbles_collisions[0] != -1) {
            rewind_back();
            return false;
        }
    }

    auto position = world_state.marbles[0].position;
    auto direction = world_state.marbles[0].velocity.normal();

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

    std::shuffle(plank_directions.begin(), plank_directions.end(), GLOBAL_PRNG);

    for (auto d : plank_directions) {
        auto plank_center = position + d * (MARBLES_RADIUS + PLANKS_WIDTH + 1e-3);
        auto plank_a = plank_center + d.rot90() * PLANK_HALF_LENGTH;
        auto plank_b = plank_center - d.rot90() * PLANK_HALF_LENGTH;

        // Check if new plank could collide with marble back in time
        auto collide_with_marble_earlier = true;
        for (auto &history_slice : world_state.marbles_history) {
            for (auto &marble: history_slice) {
                if (point_to_segment(marble.position, plank_a, plank_b).first <= MARBLES_RADIUS + PLANKS_WIDTH) {
                    collide_with_marble_earlier = false;
                    break;
                }
            }
        }
        if (!collide_with_marble_earlier) continue;


        world_state.planks.push_back(Plank{plank_a, plank_b});
        if (recurs(pointer)) {
            rewind_back(); // Undo simulation 1
            return true;
        }

        world_state.planks.pop_back();
    }
    rewind_back(); // Undo simulation 1
    return false;
}

WorldGeneratorRecursive::WorldGeneratorRecursive(std::vector<int> track) : WorldGenerator(track) {
}

void WorldGeneratorRecursive::generate() {
    recurs();
}

