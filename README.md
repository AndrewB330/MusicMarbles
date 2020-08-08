# New video (https://youtu.be/rcDFb2OkA4Q)
# Music Marbles 

![Logo](/images/logo_small_.png?raw=true)

Creating music using falling marbles on automatically
generated 2D map with obstacles.

- Recursive map generator
- Simplified physics engine
- C++ for high performance
- Vanilla HTML5 Canvas for visualization
- Tone.js for sounds

# Demo

![UI](/images/ui_demo_v2.png?raw=true)

Live demo is available by this link: https://andrewb330.github.io/MusicMarbles/index.html

YouTube video: https://youtu.be/rcDFb2OkA4Q


# Physics engine
In this project, I'm using my own simplified physics engine. It is written on C++ and
compiled with emscripten to work in your browser.
Right now engine supports only two types of objects: **Marbles** and **Planks**.
Information about the world at some point in time is represented by the WorldState class.
There is two very useful functions in this class: `simulate()` and `rewind_back()`.
- `simulate()` - simulates one tick (15ms by default) of physics
- `rewind_back()` - rewinds WorldState to its previous state, one tick before

Here is a small example how you can use it:
```cpp
WorldState state;
state.marbles.push_back(Marble{/*position:*/Vec2d(100, 200),/*velocity:*/Vec2d(10, 0)});
state.marbles.push_back(Marble{Vec2d(150, 200),Vec2d(-9, 0)});
state.marbles.push_back(Marble{Vec2d(125, 220),Vec2d(0, -2)});
state.planks.push_back(Plank{/*first_point*/Vec2d(90, 0), /*second_point*/Vec2d(160, 0)});
// simulate 2 seconds of physics
while (state.elapsed_time < 2000)
    state.simulate();

/*do something*/

// rewind everything to initial state
while (state.elapsed_time > 0)
    state.rewind_back();
```

### Algorithm

The algorithm for generating a map is pretty straightforward. 
We know moments of time, when we want a note to be played, 
so we can add a plank to  the world just before sound should be played. 
Basically, the algorithm is just recursive brute-force.

TODO: add pseudo code

### Stack

- JS
- C++
- WASM