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

/*
 * Base class for world generator
 * doing nothing useful
 */
class WorldGenerator {
protected:
    std::vector<int> track;
    WorldState world_state;
    double start_y;
    double min_y;
    double max_y;
public:
    WorldGenerator(std::vector<int> track, double min_y = -350, double max_y = 350);

    /*
     * Returns a generated world. With 0 elapsed time and one marble at the beginning of the path
     */
    WorldState get_world();
};

/*
 * Recursive world generator. First, original implementation of the algorithm
 */
class WorldGeneratorRecursive : public WorldGenerator {
private:
    bool recurs(int pointer = -1);

public:
    WorldGeneratorRecursive(std::vector<int> track);

    void generate();
};

/*
 * Helper structure for iterative world generator
 */
struct StackState {
    int pointer;
    int simulations = 0;
    std::vector<int> permutation = {};
    int index = -1;
    bool plank_added = false;

    explicit StackState(int pointer);
};

/*
 * Recursive algorithm, rewritten to work without an actual recursion.
 * Allows to generate world gradually. "Non-blocking" world generator
 *
 * Important feature: if it can't find any improvement for a long time it
 * will reset a part of generated world (about 10%) to make a "shake-up"
 */
class WorldGeneratorIterative : public WorldGenerator {
private:
    std::vector<StackState> stack;
    int max_planks_num_generated = 0;
    int num_iterations_no_improvement = 0;
public:
    WorldGeneratorIterative(std::vector<int> track, double min_y = -350, double max_y = 350);

    /*
     * Returns progress - number from 0 to 100. Where 100 - means generation is finished
     * But you can always call get_world() to get a partial result, even when progress < 100
     */
    int get_progress();

    /*
     * Call this function while progress is less than 100
     */
    void generate(int iterations = 1000);
};



#endif //MUSIC_MARBLES_WORLD_GENERATOR_HPP
