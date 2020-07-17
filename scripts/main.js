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

let track = [];
let planks = [];
let planks_color = [];
let marbles = [];
let marbles_trace = [];
let collisions = []; // mapping from marbles to planks that were hit
let particles = []; // additional effects
let ready = false; // True if world fully generated
let main_loop_running = false;

function load_track(midi_json) {
    API.reset_tracks();

    let track_id = 0;

    midi_json['tracks'].forEach((track, idx) => {
        if (track.notes.length > midi_json['tracks'][track_id].notes.length) {
            track_id = idx;
        }
    });

    let time_offset = 0.5 - midi_json['tracks'][track_id].notes[0].time;

    let unmerged = midi_json['tracks'][track_id].notes.map(sound => {
        return {
            note_names: [sound.name],
            time: sound.time + time_offset,
            duration: sound.duration
        }
    });
    track = [];
    for (let i = 0; i < unmerged.length; i++) {
        if (i > 0 && unmerged[i - 1].time + 4 < unmerged[i].time) {
            break;
        }
        if (i > 0 && unmerged[i - 1].time === unmerged[i].time) {
            track[track.length - 1].note_names.push(unmerged[i].note_names[0]);
        } else {
            console.log(unmerged[i].time * 1000);
            API.add_time_of_hit(0, Math.round(unmerged[i].time * 1000));
            track.push(unmerged[i]);
        }
    }
}

function synchronize() {
    // we assume that array of planks do change during the simulation
    planks = API.get_planks();
    while (planks_color.length < planks.length)
        planks_color.push(BLUE);

    marbles = API.get_marbles();
    marbles.forEach((marble, index) => {
        if (marbles_trace[index] === undefined)
            marbles_trace[index] = [];
        marbles_trace[index].push(marble);
        if (marbles_trace[index].length > TRACE_SIZE) {
            marbles_trace[index].shift();
        }
    });
    collisions = API.get_marble_collisions();
}

function update() {
    let sum_x = (marbles[0][1] > 2000 ? 0 : marbles[0][0]);
    let targetX = -sum_x + canvas.width / 2;
    offset[0] = (offset[0] * 0.98 + targetX * 0.02);

    particles.forEach(p => {
        p.time_left -= MS_PER_TICK;
        p.position[0] += p.velocity[0];
        p.position[1] += p.velocity[1];
        p.velocity[1] += 0.5;
    })
    particles = particles.filter(p => p.time_left >= 0);
}

function draw() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    marbles.forEach((marble, index) => {
        let color = (index === 0 ? RED : '#404040');
        G.drawCircleFilled(marble, MARBLE_RADIUS + 1, color);
        ctx.globalAlpha = 0.2;
        let trace_len = marbles_trace[index].length;
        for (let j = 0; j < trace_len; j++) {
            G.drawCircleFilled(
                marbles_trace[index][trace_len - j - 1],
                MARBLE_RADIUS - j * RADIUS_DECAY - 1,
                color
            );
        }
        ctx.globalAlpha = 1.0;
    });

    planks.forEach((plank, index) => {
        let color = planks_color[index];
        G.drawLine(plank, color, PLANK_WIDTH * 2);
        G.drawCircleFilled(plank[0], PLANK_WIDTH, color);
        G.drawCircleFilled(plank[1], PLANK_WIDTH, color);
    });

    particles.forEach(p => {
        ctx.globalAlpha = Math.sqrt(p.time_left / p.time_total);
        ctx.drawImage(
            NOTE_IMAGES[p.image],
            p.position[0] - PARTICLE_SIZE / 2 + offset[0],
            p.position[1] - PARTICLE_SIZE / 2 + offset[1],
            PARTICLE_SIZE,
            PARTICLE_SIZE
        );
    })
    ctx.globalAlpha = 1.0;
}

function generate_particle(center, plank_index, big = false) {
    let position = [center[0] + (Math.random() - 0.5) * 5, center[1] + (Math.random() - 0.5) * 5];
    let velocity = [(Math.random() - 0.5) * 5, -Math.random() * 2 - 3];

    particles.push({
        image: (big ? 1 : 0),
        time_total: track[plank_index].duration * 1000 * (big ? 3 : 2),
        time_left: track[plank_index].duration * 1000 * (big ? 3 : 2),
        velocity: velocity,
        position: position
    });
}

function play_sounds() {
    for (let i = 0; i < marbles.length; i++) {
        let plank_index = collisions[i];
        if (plank_index === -1)
            continue;
        if (i === 0) {
            planks_color[plank_index] = RED;
            console.log(plank_index);
        }
        let plank = planks[plank_index];
        let particle_position = [
            (plank[0][0] + plank[1][0]) * 0.5,
            (plank[0][1] + plank[1][1]) * 0.5
        ];
        PIANO.triggerAttackRelease(track[plank_index].note_names, track[plank_index].duration);
        for (let j = 0; j < track[plank_index].note_names.length; j++) {
            generate_particle(particle_position, plank_index, j > 1);
        }
    }
}


let lastTime = Date.now();

function main_loop() {
    let time = Date.now();
    lastTime = time;

    if (!ready) {
        API.generate_world_limited();
        let progress = API.get_generator_progress()
        if (progress >= 100) {
            ready = true;
        }
        document.getElementById('progress-bar').style.width = `${progress}%`;
        document.getElementById('progress-bar-text').textContent = `${progress}%`;
    } else {
        API.update_main_world()
    }

    synchronize();
    update();
    play_sounds();
    draw();

    setTimeout(main_loop, Math.max(5, Math.min(2 * MS_PER_TICK - (time - lastTime), MS_PER_TICK)));
}

function run_all() {
    ready = false;
    planks_color = [];
    API.init_generator();
    if (!main_loop_running) {
        main_loop();
        main_loop_running = true;
    }
}

window.addEventListener('load', () => {
    let list = document.getElementById('music-list');
    music.forEach((m, i) => {
        let element = document.createElement('div');
        element.classList.add('music');
        element.innerText = m['header']['name'];
        element.onclick = () => {
            load_track(music[i]);
            run_all();
        };
        list.append(element);
    })
});