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

const int MAX_TRACKS = 16;
const int PLANK_HALF_LENGTH = 18;

extern std::mt19937 rng;

// array of tracks, track is just a list of points in time when we should hit a plank
// but for now we are using only 0-th track (!)
extern std::vector<std::vector<int>> tracks;

WorldState generate_world();


#endif //MUSIC_MARBLES_WORLD_GENERATOR_HPP
