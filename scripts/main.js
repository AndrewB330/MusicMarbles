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

class Plank {
    constructor(a, b) {
        this.a = a;
        this.b = b;
        this.active = 0;

        let delta_x = a[0] - b[0];
        let delta_y = a[1] - b[1];
        this.center_x = (a[0] + b[0]) * 0.5;
        this.center_y = (a[1] + b[1]) * 0.5;
        this.angle = Math.atan2(delta_y, delta_x) + Math.PI;
    }

    update() {
        this.active -= MS_PER_TICK;
    }

    draw() {
        ctx.translate(this.center_x + offset[0], this.center_y + offset[1]);
        ctx.rotate(this.angle);
        ctx.drawImage(this.active > 0 ? PLANK_IMAGES[1] : PLANK_IMAGES[0], -30, -10, 60, 20);
        ctx.rotate(-this.angle);
        ctx.translate(-this.center_x - offset[0], -this.center_y - offset[1]);
    }
}

class Marble {
    constructor(position) {
        this.position = position;
        this.trace = [];
        this.collision = -1;
    }

    synchronize(position, collision) {
        this.trace.push(position);
        if (this.trace.length > TRACE_SIZE) {
            this.trace.shift();
        }
        this.position = position;
        this.collision = collision;
    }

    draw() {
        ctx.globalAlpha = 0.05;
        ctx.shadowColor = '#fff7c8';
        ctx.shadowBlur = 15;
        let trace_len = this.trace.length;
        for (let j = 0; j < trace_len; j++) {
            G.drawCircleFilled(
                this.trace[trace_len - j - 1],
                MARBLE_RADIUS - j * RADIUS_DECAY - 1,
                '#fff7c8'
            );
        }
        ctx.shadowBlur = 0;
        ctx.globalAlpha = 1.0;

        ctx.drawImage(
            MARBLE_IMAGE,
            this.position[0] - MARBLE_IMAGE_SIZE / 2 + offset[0],
            this.position[1] - MARBLE_IMAGE_SIZE / 2 + offset[1],
            MARBLE_IMAGE_SIZE,
            MARBLE_IMAGE_SIZE
        );
    }
}

class Particle {
    constructor(position, velocity, time) {
        this.position = position;
        this.velocity = velocity;
        this.time_left = time;
        this.time_total = time;
    }

    update() {
        this.time_left -= MS_PER_TICK;
        // apply velocity
        this.position[0] += this.velocity[0];
        this.position[1] += this.velocity[1];
        // apply gravity
        this.velocity[1] += 0.2;
    }

    draw() {
        ctx.globalAlpha = Math.sqrt(this.time_left / this.time_total);
        ctx.drawImage(
            PARTICLE_IMAGE,
            this.position[0] - PARTICLE_IMAGE_SIZE / 2 + offset[0],
            this.position[1] - PARTICLE_IMAGE_SIZE / 2 + offset[1],
            PARTICLE_IMAGE_SIZE,
            PARTICLE_IMAGE_SIZE
        );
        ctx.globalAlpha = 1.0;
    }
}

let track = [];
let planks = [];
let marbles = [];
let particles = []; // additional effects
let ready = false; // True if world fully generated
let planks_loaded = false;
let main_loop_running = false;

function load_track(midi_json) {
    API.reset_tracks();

    let track_id = 0;

    midi_json['tracks'].forEach(track => {
        track['notes'].sort((a, b) => {
            return a.time - b.time;
        });
    });

    midi_json['tracks'].forEach((track, idx) => {
        if (track.notes.length > midi_json['tracks'][track_id].notes.length) {
            track_id = idx;
        }
    });

    let multiplier = midi_json['header']['multiplier'] || 1.0;
    let time_offset = 0.5 - midi_json['tracks'][track_id].notes[0].time * multiplier;
    let unmerged = midi_json['tracks'][track_id].notes.map(sound => {
        return {
            note_names: [sound.name],
            time: sound.time * multiplier + time_offset,
            duration: sound.duration * multiplier
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
    if (!planks_loaded) {
        planks = API.get_planks().map((p) => new Plank(p[0], p[1]));
        if (ready) planks_loaded = true;
    }

    let collisions = API.get_marble_collisions();

    API.get_marbles().forEach((m, i) => {
        if (marbles[i]) {
            marbles[i].synchronize(m, collisions[i]);
        } else {
            marbles[i] = new Marble(m);
        }
    });
}

function update() {
    // shift background and camera
    let sum_x = (marbles[0].position[1] > 2000 ? 0 : marbles[0].position[0]);
    let targetX = -sum_x + canvas.width / 2;
    offset[0] = (offset[0] * 0.98 + targetX * 0.02);
    canvas.style.backgroundPosition = `${offset[0] / 2}px -10px`;
    document.body.style.backgroundPosition = `${offset[0] / 4}px -10px`;

    planks.forEach(p => p.update());

    particles.forEach(p => p.update());

    particles = particles.filter(p => p.time_left >= 0);
}

function draw() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    marbles.forEach(m => m.draw());

    planks.forEach(p => p.draw());

    particles.forEach(p => p.draw());

}

function generate_particle(center, plank_index) {
    let position = [center[0] + (Math.random() - 0.5) * 5, center[1] + (Math.random() - 0.5) * 5];
    let velocity = [(Math.random() - 0.5) * 5, -Math.random() * 2 - 1];
    particles.push(new Particle(position, velocity, track[plank_index].duration * 1000 * 3));
}

function play_sounds() {
    for (let i = 0; i < marbles.length; i++) {
        let plank_index = marbles[i].collision;
        if (plank_index === -1)
            continue;
        let plank = planks[plank_index];
        let particle_position = [plank.center_x, plank.center_y];
        plank.active = track[plank_index].duration * 1200;
        PIANO.triggerAttackRelease(track[plank_index].note_names, track[plank_index].duration);
        for (let j = 0; j < track[plank_index].note_names.length * 3 + 4; j++) {
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
    planks_loaded = false;
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
            let ch = list.children;
            for(let j = 0; j < ch.length; j++) {
                ch[j].classList.remove('active')
            }
            element.classList.add('active');
            load_track(music[i]);
            run_all();
        };
        list.append(element);
    })
});