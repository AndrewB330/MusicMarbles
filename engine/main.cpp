/*
 * ATTENTION!
 * THIS IS NOT THE FINAL VERSION OF THE CODE
 * THIS CODE IS REALLY UGLY AND UNREADABLE
 * I WILL CLEAN THIS UP AND WILL POST CLEAN CODE IN A FEW DAYS OR WEEKS
*/

#include <cmath>
#include <vector>
#include <algorithm>
#include <random>
#include "math.hpp"


const int TICK_MS = 15;
const int MAX_TRACKS = 4;
const int MICRO_TICKS = 4;

const double RADIUS = 16.0;
const double plank_THICK = 4.0;

const double DELTA_TICK = TICK_MS / 1000.0;
const double DT = DELTA_TICK / MICRO_TICKS;

const Vec2d GRAVITY = Vec2d(0, 9.8 * 10 * 3);

std::mt19937 rng(0);

std::pair<double, Vec2d> point_to_segment(Vec2d o, Vec2d a, Vec2d b) {
    double proj = (o - a) * (b - a);
    double dist;
    Vec2d normal;
    if (proj < 0) {
        dist = (o - a).len();
        normal = (o - a).normal();
    } else if (proj > (b - a).len_sqr()) {
        dist = (o - b).len();
        normal = (o - b).normal();
    } else {
        dist = (o - a) ^ (b - a).normal();
        normal = -(b - a).rot90().normal();
        if (dist < 0) {
            normal = -normal;
            dist = -dist;
        }
    }
    return {dist, normal};
}

struct Sound {
    int note = 0;
    int octave = 0;
    int duration = 0;

    bool operator==(const Sound &o) {
        return note == o.note && octave == o.octave && duration == o.duration;
    }

    bool operator!=(const Sound &o) {
        return !(*this == o);
    }

    bool operator<(const Sound &o) const {
        return std::tie(note, octave, duration) < std::tie(o.note, o.octave, o.duration);
    }
};

struct Marble {
    Vec2d pos;
    Vec2d velocity;
    double bounce_factor = 1.0;
};

struct Plank {
    Vec2d a;
    Vec2d b;
    Sound sound;
    double bounce_factor = 1.0; // [0; 1] range
};

bool collide(Marble &a, Marble &b) {
    double dist_sqr = (a.pos - b.pos).len_sqr();
    if (dist_sqr >= (RADIUS * 2) * (RADIUS * 2)) {
        return false;
    }

    Vec2d ab = (b.pos - a.pos).normal();
    Vec2d abv = (b.velocity - a.velocity);

    a.velocity += (ab * abv) * ab;
    b.velocity -= (ab * abv) * ab;

    double depth = (2 * RADIUS - ab.len());

    a.pos -= ab * depth * 0.3;
    b.pos += ab * depth * 0.3;
    return true;
}

bool collide(Marble &a, Plank &b) {
    auto res = point_to_segment(a.pos, b.a, b.b);
    double dist = res.first;
    Vec2d normal = res.second;

    if (dist >= RADIUS + plank_THICK) {
        return false;
    }

    a.velocity -= (a.velocity * normal) * normal * (1 + (a.bounce_factor + b.bounce_factor) * 0.5);

    double depth = RADIUS + plank_THICK - dist;
    a.pos += normal * depth;
    return true;
}

struct WorldState {
    std::vector<Marble> marbles;
    std::vector<Plank> planks;

    std::vector<Sound> sounds; // sounds that were produced during the last tick

    std::vector<std::vector<Marble>> trace;
    std::vector<std::vector<Sound>> sound_trace;

    int time_elapsed = 0;

    void start() {
        time_elapsed = 0;
        trace.push_back(marbles);
        sounds.assign(marbles.size(), Sound{});
        sound_trace.push_back(sounds);
    }

    void update(bool track = true) {
        sounds.assign(marbles.size(), Sound{});
        for (int m_tick = 0; m_tick < MICRO_TICKS; m_tick++) {
            for (int i = 0; i < marbles.size(); i++) {
                for (int j = i + 1; j < marbles.size(); j++) {
                    if (collide(marbles[i], marbles[j])) {
                        // do nothing
                        sounds[i] = Sound{0, 0, -1};
                    }
                }
                for (auto &p : planks) {
                    if (collide(marbles[i], p)) {
                        sounds[i] = p.sound;
                    }
                }
            }
            for (auto &marble : marbles) {
                marble.pos += marble.velocity * DT + GRAVITY * (DT * DT * 0.5);
                marble.velocity += GRAVITY * DT;
            }
            if (track)
                trace.push_back(marbles);
        }
        if (track)
            sound_trace.push_back(sounds);
        time_elapsed += TICK_MS;
    }

    void undo() {
        time_elapsed -= TICK_MS;
        for (int i = 0; i < MICRO_TICKS; i++) {
            trace.pop_back();
        }
        sound_trace.pop_back();

        sounds = sound_trace.back();
        marbles = trace.back();
    }

    void reset() {
        time_elapsed = 0;
        marbles.clear();
        planks.clear();
        sounds.clear();
        trace.clear();
    }
};

std::vector<std::vector<std::pair<int, Sound>>> tracks(MAX_TRACKS);
WorldState main_world;

extern "C" {

void update_main_world() {
    if (main_world.time_elapsed == 0) {
        main_world.marbles[0].pos.x = -1000;
        main_world.marbles.push_back(Marble{Vec2d{700, +30}, Vec2d{}, 0.8});
    }
    if (main_world.time_elapsed == TICK_MS * 90) {
        main_world.marbles.push_back(Marble{Vec2d{300, -40}, Vec2d{}, 0.8});
    }
    if (main_world.time_elapsed == TICK_MS * 400) {
        main_world.marbles[0].velocity = Vec2d{};
        main_world.marbles[0].pos = Vec2d{0, -340};
    }

    main_world.update(false);
}

std::vector<int> pointers(MAX_TRACKS);
std::vector<Sound> required_sounds(MAX_TRACKS);
std::vector<std::vector<Sound>> required_sounds_stack = {required_sounds};

std::vector<std::vector<Marble>> TRACE;

int recurs(WorldState &state, int track, bool all_done) {
    if (track >= state.marbles.size()) {
        state.update();
        for (int i = 0; i < state.marbles.size(); i++) {
            if (state.sounds[i] != required_sounds[i]) {
                state.undo();
                return -1;
            }
        }
        if (all_done) {
            bool ok = true;
            for (int i = 0; i < 200; i++) {
                state.update();
                for (auto &s: state.sounds) {
                    if (s.note || s.octave || s.duration)
                        ok = false;
                }
            }
            for (int i = 0; i < 200; i++) {
                state.undo();
            }
            state.undo();
            if (!ok)
                return -4;
            TRACE = state.trace;
            main_world.planks = state.planks;
            return +1;
        }
        required_sounds_stack.push_back(required_sounds);
        required_sounds.assign(MAX_TRACKS, Sound{});
        int res = recurs(state, 0, true);
        required_sounds = required_sounds_stack.back();
        required_sounds_stack.pop_back();
        state.undo();
        return res;
    }

    required_sounds[track] = Sound{};

    if (pointers[track] >= tracks[track].size() ||
        tracks[track][pointers[track]].first >= state.time_elapsed + TICK_MS) {
        int updates = 0;
        int sound_stack_updates = 0;
        while (pointers[track] >= tracks[track].size() ||
               tracks[track][pointers[track]].first >= state.time_elapsed + TICK_MS) {
            if (pointers[track] < tracks[track].size())
                all_done = false;
            track++;
            if (track >= state.marbles.size()) {
                state.update();
                updates++;
                for (int i = 0; i < state.marbles.size(); i++) {
                    if (state.sounds[i] != required_sounds[i]) {
                        while(updates--)
                            state.undo();
                        while(sound_stack_updates--) {
                            required_sounds = required_sounds_stack.back();
                            required_sounds_stack.pop_back();
                        }
                        return -1;
                    }
                }
                if (all_done) {
                    int res = recurs(state, track, all_done);
                    if (res > 0) {
                        while(updates--)
                            state.undo();
                        while(sound_stack_updates--) {
                            required_sounds = required_sounds_stack.back();
                            required_sounds_stack.pop_back();
                        }
                        return res;
                    }
                }

                track = 0;
                all_done = true;
                required_sounds_stack.push_back(required_sounds);
                required_sounds.assign(MAX_TRACKS, Sound{});
                sound_stack_updates++;
            }
        }

        auto res = recurs(state, track, all_done);
        while(updates--)
            state.undo();
        while(sound_stack_updates--) {
            required_sounds = required_sounds_stack.back();
            required_sounds_stack.pop_back();
        }
        return res;
    }

    if (pointers[track] >= tracks[track].size())
        return recurs(state, track + 1, all_done);

    if (tracks[track][pointers[track]].first >= state.time_elapsed + TICK_MS) {
        return recurs(state, track + 1, false);
    }

    required_sounds[track] = tracks[track][pointers[track]].second;

    auto pos = state.marbles[track].pos;
    auto dir = state.marbles[track].velocity.normal();

    if (abs(pos.y) > 350)
        return false;
    /*if (pos.x - state.trace[0][track].pos.x > (30 * state.time_elapsed) / 1000 + 420)
        return false;*/

    std::vector<Vec2d> dirs = {
            dir,
            dir.rot(0.15),
            dir.rot(-0.15),
            dir.rot(0.6),
            dir.rot(-0.6),
            dir.rot(0.3),
            dir.rot(-0.3),
            dir.rot(0.9),
            dir.rot(-0.9),
            dir.rot(1.3),
            dir.rot(-1.3),
    };

    std::shuffle(dirs.begin(), dirs.end(), rng);

    pointers[track]++;
    for (auto d : dirs) {
        auto p = pos + d * (RADIUS + plank_THICK + 1e-3);
        auto a = p + d.rot90() * 18;
        auto b = p - d.rot90() * 18;

        auto ok = true;
        for (auto &t : state.trace) {
            for (auto &m: t) {
                if (point_to_segment(m.pos, a, b).first <= RADIUS + plank_THICK) {
                    ok = false;
                    break;
                }
            }
        }
        if (!ok) continue;

        state.planks.push_back(Plank{a, b, required_sounds[track]});

        int res = recurs(state, track + 1, false);

        if (res > 0) return res;

        state.planks.pop_back();
    }
    pointers[track]--;
    return -3;
}

int regenerate_world() {
    for(auto & track : tracks) {
        std::sort(track.begin(), track.end());
    }
    pointers.assign(0, MAX_TRACKS);
    WorldState state;
    state.reset();
    state.marbles.push_back(Marble{Vec2d(0, -340), Vec2d()});
    state.start();

    main_world = state;
    return recurs(state, 0, false);
}

void init_world() {
    main_world.reset();
    main_world.marbles.push_back(Marble{Vec2d(0, -120), Vec2d()});
    main_world.marbles.push_back(Marble{Vec2d(0 + 145, -120), Vec2d()});
    main_world.planks.push_back(Plank{Vec2d(-30, -30), Vec2d(30, 0)});
    main_world.planks.push_back(Plank{Vec2d(-30 + 150, 0), Vec2d(30 + 150, -30)});
    main_world.planks.push_back(Plank{Vec2d(-30 - 100, -30 + 50), Vec2d(30 - 80, 0 + 50)});
    main_world.planks.push_back(Plank{Vec2d(-30 + 250, 0 + 45), Vec2d(30 + 250, -30 + 55)});
    main_world.planks.push_back(Plank{Vec2d(-20, 70), Vec2d(160, 75)});
}

// INTERFACE

int num_marbles() {
    return main_world.marbles.size();
}

int num_planks() {
    return main_world.planks.size();
}

int get_marble_x(size_t i) {
    return main_world.marbles[i].pos.x;
}

int get_marble_y(size_t i) {
    return main_world.marbles[i].pos.y;
}

int get_plank_a_x(size_t i) {
    return main_world.planks[i].a.x;
}

int get_plank_a_y(size_t i) {
    return main_world.planks[i].a.y;
}

int get_plank_b_x(size_t i) {
    return main_world.planks[i].b.x;
}

int get_plank_b_y(size_t i) {
    return main_world.planks[i].b.y;
}

int num_sounds() {
    return main_world.sounds.size();
}

int get_note(int i) {
    return main_world.sounds[i].note;
}

int get_octave(int i) {
    return main_world.sounds[i].octave;
}

int get_duration(int i) {
    return main_world.sounds[i].duration;
}

void add_sound(int track, int time, int note, int octave, int duration) {
    tracks[track].emplace_back(time, Sound{note, octave, duration});
}

}

int main() {
    return 0;
}