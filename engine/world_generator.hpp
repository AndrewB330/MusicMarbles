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

#ifndef MUSIC_MARBLES_WORLD_GENERATOR_HPP
#define MUSIC_MARBLES_WORLD_GENERATOR_HPP

#include <cmath>
#include <vector>
#include <algorithm>
#include <random>
#include "engine.hpp"

struct StackState {
    int pointer;
    int simulations = 0;
    std::vector<int> permutation = {};
    int index = -1;
    bool plank_added = false;

    explicit StackState(int pointer);
};

class WorldGenerator {
protected:
    std::vector<int> track;
    WorldState world_state;
    double min_y = -350;
    double max_y = 350;
    double start_y = -340;
public:
    WorldGenerator(std::vector<int> track);

    WorldState get_world();
};

class WorldGeneratorIterative : public WorldGenerator {
private:
    std::vector<StackState> stack;
public:
    WorldGeneratorIterative(std::vector<int> track);

    double get_progress();

    void generate_limited(int iterations = 10000);
};

class WorldGeneratorRecursive : public WorldGenerator {
private:
    std::vector<int> track;

    bool recurs(int pointer = -1);

public:
    WorldGeneratorRecursive(std::vector<int> track);

    void generate();
};


#endif //MUSIC_MARBLES_WORLD_GENERATOR_HPP
