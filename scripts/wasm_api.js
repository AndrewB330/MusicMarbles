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

let API = {};

Module['onRuntimeInitialized'] = () => {
    // world functions
    API.update_main_world = Module.cwrap('update_main_world', null, []);
    API.generate_world = Module.cwrap('generate_world_', null, []);
    API.generate_world_limited = Module.cwrap('generate_world_limited', null, []);
    API.init_generator = Module.cwrap('init_generator', null, []);
    API.get_generator_progress = Module.cwrap('get_generator_progress', 'number', []);
    // marbles getters
    API.num_marbles = Module.cwrap('num_marbles', 'number', []);
    API.get_marble_x = Module.cwrap('get_marble_x', 'number', ['number']);
    API.get_marble_y = Module.cwrap('get_marble_y', 'number', ['number']);
    API.get_marble = (i) => [API.get_marble_x(i), API.get_marble_y(i)];
    API.get_marbles = () => {
        let marbles = [];
        for (let i = 0; i < API.num_marbles(); i++) {
            marbles.push(API.get_marble(i));
        }
        return marbles;
    }
    API.set_max_y = Module.cwrap('set_max_y', null, ['number']);
    API.set_min_y = Module.cwrap('set_min_y', null, ['number']);
    // planks getters
    API.num_planks = Module.cwrap('num_planks', 'number', []);
    API.get_plank_a_x = Module.cwrap('get_plank_a_x', 'number', ['number']);
    API.get_plank_a_y = Module.cwrap('get_plank_a_y', 'number', ['number']);
    API.get_plank_b_x = Module.cwrap('get_plank_b_x', 'number', ['number']);
    API.get_plank_b_y = Module.cwrap('get_plank_b_y', 'number', ['number']);
    API.get_plank = (i) => [[API.get_plank_a_x(i), API.get_plank_a_y(i)], [API.get_plank_b_x(i), API.get_plank_b_y(i)]];
    API.get_planks = () => {
        let planks = [];
        for (let i = 0; i < API.num_planks(); i++) {
            planks.push(API.get_plank(i));
        }
        return planks;
    }
    // collisions
    API.get_marble_collision = Module.cwrap('get_marble_collision', 'number', ['number']);
    API.get_marble_collisions = () => {
        let collisions = [];
        for (let i = 0; i < API.num_marbles(); i++) {
            collisions.push(API.get_marble_collision(i));
        }
        return collisions;
    }
    // track
    API.add_time_of_hit = Module.cwrap('add_time_of_hit', null, ['number', 'number']);
    API.reset_tracks = Module.cwrap('reset_tracks', null, []);
};