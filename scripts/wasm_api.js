let API = {};

Module['onRuntimeInitialized'] = () => {

    // world functions
    API.init_world = Module.cwrap('init_world', null, []);
    API.update_main_world = Module.cwrap('update_main_world', null, []);
    API.regenerate_world = Module.cwrap('regenerate_world', 'number', []);
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
    // sound function
    API.num_sounds = Module.cwrap('num_sounds', 'number', []);
    API.get_octave = Module.cwrap('get_octave', 'number', ['number']);
    API.get_note = Module.cwrap('get_note', 'number', ['number']);
    API.get_duration = Module.cwrap('get_duration', 'number', ['number']);
    API._add_sound = Module.cwrap('add_sound', 'number', ['number', 'number', 'number', 'number', 'number']);
    API.add_sound = (track, time, sound) => API._add_sound(track, time, sound['note'], sound['octave'], sound['duration']);
    API.get_sounds = () => {
        let num = API.num_sounds();
        let sounds = [];
        for (let i = 0; i < num; i++) {
            sounds.push({
                note: API.get_note(i),
                octave: API.get_octave(i),
                duration: API.get_duration(i),
            })
        }
        return sounds;
    };

};