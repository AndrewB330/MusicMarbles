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

const RED = '#aa0836';
const BLUE = '#035db4';

const MS_PER_TICK = 15;
// Actual radius +1 because sometimes it looks like it didn't hit the plank, because hit happened in micro-tick
const MARBLE_RADIUS = 16 + 1;
// The same as in engine, this is half-width in fact
const PLANK_WIDTH = 4;

const PIANO = new Tone.PolySynth(4, Tone.Synth, {}).toMaster();

const NOTE_IMAGES = [new Image(), new Image()]
NOTE_IMAGES[0].src = './images/note_1.png';
NOTE_IMAGES[1].src = './images/note_2.png';

const PLANK_IMAGES = [new Image(), new Image()]
PLANK_IMAGES[0].src = './images/plank.png';
PLANK_IMAGES[1].src = './images/plank_active.png';

const PARTICLE_IMAGE = new Image();
PARTICLE_IMAGE.src = './images/particle.png';

const MARBLE_IMAGE = new Image();
MARBLE_IMAGE.src = './images/marble.png';

const TRACE_SIZE = 60;
const RADIUS_DECAY = 0.25;

const PARTICLE_IMAGE_SIZE = 16;
const MARBLE_IMAGE_SIZE = 60;