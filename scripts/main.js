/*
 * ATTENTION!
 * THIS IS NOT THE FINAL VERSION OF THE CODE
 * THIS CODE IS REALLY UGLY AND UNREADABLE
 * I WILL CLEAN THIS UP AND WILL POST CLEAN CODE IN A FEW DAYS OR WEEKS
*/


let dt = 15;
let MARBLE_RADIUS = 16;
let PLANK_WIDTH = 4;
const psynth = new Tone.PolySynth(4, Tone.Synth, {}).toMaster();
let running = false;

let note1 = new Image();
note1.src = './resources/note_1.png';
let note2 = new Image();
note2.src = './resources/note_2.png';

let particles = [];

let red = '#aa0836';
let blue = '#035db4';

function note_to_num(note) {
    if (note[1] === '#')
        return -note_to_num(note[0]);
    return note.charCodeAt(0) - 'A'.charCodeAt(0) + 1;
}

let trace = {};
let trace_small = {};
let SOUND_ARRAY = [];
let pointer = 0;

let T = 0;
let PLANKS = [];
let lastTime = Date.now();
function update() {
    T++;
    let time = Date.now();
    let delta = time - lastTime;
    lastTime = time;
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    if (T >= 100)
        API.update_main_world(dt);

    let sumX = 0;
    for (let i = 0; i < API.num_marbles(); i++) {
        if (!(i in trace)) {
            trace[i] = [];
            trace_small[i] = [];
        }
        let pos = API.get_marble(i);
        sumX += pos[0];
        //trace[i].push(pos);
        trace_small[i].push(pos);
        if (trace_small[i].length > 15) {
            trace_small[i].shift();
        }
        let color = (i === 0 ? red : '#404040');
        G.drawCircleFilled(pos, MARBLE_RADIUS + 1, color);
        ctx.globalAlpha = 0.4;
        ctx.globalAlpha = 0.2;
        for(let j = 0; j < trace_small[i].length; j++) {
            G.drawCircleFilled(trace_small[i][trace_small[i].length - j - 1], MARBLE_RADIUS - j - 1, color);
        }
        ctx.globalAlpha = 1.0;
    }
    let targetX = Math.min(-API.get_marble(0)[0] + canvas.width / 2, canvas.width/2 - 600);
    offset[0] = (offset[0] * 0.98 + targetX * 0.02);
    for (let i = 0; i < PLANKS.length; i++) {
        let color = (i >= pointer ? blue: red);
        G.drawLine(PLANKS[i], color, PLANK_WIDTH * 2);
        G.drawCircleFilled(PLANKS[i][0], PLANK_WIDTH, color);
        G.drawCircleFilled(PLANKS[i][1], PLANK_WIDTH, color);
    }


    API.get_sounds().forEach(sound => {
        if (sound['note']) {
            let plank = API.get_plank(sound['note'] - 1);
            let v = [(plank[0][0] + plank[1][0])*0.5,(plank[0][1] + plank[1][1])*0.5];
            v[0] += (Math.random() - 0.5) * 5;
            v[1] += (Math.random() - 0.5) * 5;
            let velocity = [(Math.random() - 0.5) * 5, -Math.random() * 2 - 3];

            let duration = sound['duration'] / 1000.0;
            psynth.triggerAttackRelease(SOUND_ARRAY[sound['note'] - 1], duration);
            let big = SOUND_ARRAY[sound['note'] - 1].length > 1;
            particles.push({
                image: (big ? 2 : 1),
                time_total: duration * 1000 * (big ? 3 : 2),
                time_left: duration * 1000 * ( big ? 3 : 2),
                velocity: velocity,
                position: v
            });
            if (API.get_marble(0)[0] >= -100) {
                pointer++;
                console.log(API.get_marble(0));
            }
        }
    });

    particles.forEach(p => {
        ctx.globalAlpha = Math.sqrt(p.time_left / p.time_total);
        ctx.drawImage((p.image === 1 ? note1: note2), p.position[0] - 15 + offset[0], p.position[1] - 15 + offset[1], 30, 30);
        p.time_left -= dt;
        p.position[0] += p.velocity[0];
        p.position[1] += p.velocity[1];
        p.velocity[1] += 0.5;
    })
    ctx.globalAlpha = 1.0;
    particles = particles.filter(p => p.time_left >= 0);

    setTimeout(update, Math.max(5, Math.min(2 * dt - delta, dt)));
}

function run_all() {
    offset[0] = canvas.width/2 - 600;
    let time_offset = -1000;
    let tracks_sounds = [];
    midi.tracks.forEach((track, track_id) => {
        tracks_sounds[track_id] = [];
        track.notes.forEach(note => {
            let duration = Math.round(note.duration * 1000);
            let time = Math.round(note.time * 1000) + time_offset;
            let name = note.name;
            let octave = parseInt(name.slice(-1));
            let num = note_to_num(name.slice(0, -1));
            tracks_sounds[track_id].push({time: time, note: num, octave: octave, duration: duration, name: name});
        });
    })

    let track_ids = [0];
    tracks_sounds.forEach((sounds, track_id) => {
        if (!(track_id in track_ids)) return;
        for (let i = 0; i < Math.min(sounds.length, 100); i++) {
            if (i > 0 && sounds[i - 1].time === sounds[i].time) {
                SOUND_ARRAY[SOUND_ARRAY.length-1].push(sounds[i].name);
                continue;
            }
            sounds[i].note = SOUND_ARRAY.length + 1;
            API.add_sound(track_id, sounds[i].time, sounds[i]);
            SOUND_ARRAY.push([sounds[i].name]);
        }
    });

    if (!running) {
        API.regenerate_world();

        for (let i = 0; i < API.num_planks(); i++) {
            PLANKS.push(API.get_plank(i));
        }

        update();
        running = true;
    }
}