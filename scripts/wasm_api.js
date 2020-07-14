let API = {};

Module['onRuntimeInitialized'] = () => {
    // world functions
    API.update_main_world = Module.cwrap('update_main_world', null, []);
    API.generate_world = Module.cwrap('generate_world_', null, []);
    // marbles getters
    API.num_marbles = Module.cwrap('num_marbles', 'number', []);
    API.get_marble_x = Module.cwrap('get_marble_x', 'number', ['number']);
    API.get_marble_y = Module.cwrap('get_marble_y', 'number', ['number']);
    API.get_marble = (i) => [API.get_marble_x(i), API.get_marble_y(i)];
    // planks getters
    API.num_planks = Module.cwrap('num_planks', 'number', []);
    API.get_plank_a_x = Module.cwrap('get_plank_a_x', 'number', ['number']);
    API.get_plank_a_y = Module.cwrap('get_plank_a_y', 'number', ['number']);
    API.get_plank_b_x = Module.cwrap('get_plank_b_x', 'number', ['number']);
    API.get_plank_b_y = Module.cwrap('get_plank_b_y', 'number', ['number']);
    API.get_plank = (i) => [[API.get_plank_a_x(i), API.get_plank_a_y(i)], [API.get_plank_b_x(i), API.get_plank_b_y(i)]];
    // other
    API.get_marble_collision = Module.cwrap('get_marble_collision', 'number', ['number']);
    API.add_time_of_hit = Module.cwrap('add_time_of_hit', null, ['number', 'number']);
};