
[**My YouTube channel (!!)**](https://www.youtube.com/channel/UCd5z_a5j7mKFe-ynUEgnr_Q)
# Music Marbles 

![Logo](/resources/logo_small_.png?raw=true)

Creating music using falling marbles on automatically
generated 2D map with obstacles.

- Recursive map generator
- Simplified physics engine
- C++ for high performance
- Vanilla HTML5 Canvas for visualization


**[[Alert]] This was written in a few days so right now the code in this repository is not clean, 
I'm refactoring and cleaning it right now and will post it later this or next week.**

# Demo
Live demo is available by this link: https://andrewb330.github.io/MusicMarbles/index.html

YouTube video: https://youtu.be/yB-my6OsfvA


# Physics engine
In this project, we are using our own simplified physics engine. It is written on C++ and
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

### Stack

- JS
- C++
- WASM