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
/*
 * TODO: LATER, THIS FILE SHOULD BECOME A SEPARATE LIBRARY
 */
let canvasId = "main-canvas";

let canvas = document.getElementById(canvasId);
let ctx = canvas.getContext('2d');
ctx.scale(1, -1);

let offset = [0, 0];

class G {
    static setupLineStyle(color, lineWidth, dashed = []) {
        ctx.setLineDash(dashed);
        ctx.strokeStyle = color;
        ctx.lineWidth = lineWidth;
        ctx.fillStyle = color;
    }

    static drawSegment(a, b, color, lineWidth, dashed = []) {
        this.setupLineStyle(color, lineWidth, dashed);

        ctx.beginPath();
        ctx.moveTo(a[0] + offset[0], a[1] + offset[1]);
        ctx.lineTo(b[0] + offset[0], b[1] + offset[1]);
        ctx.stroke();
    }

    static drawLine(points, color, lineWidth, dashed = []) {
        this.setupLineStyle(color, lineWidth, dashed);

        ctx.beginPath();
        for(let i = 0; i < points.length; i++) {
            let x = points[i][0] + offset[0];
            let y = points[i][1] + offset[1];
            if (i === 0) {
                ctx.moveTo(x, y);
            } else {
                ctx.lineTo(x, y);
            }
        }
        ctx.stroke();
    }

    static drawCircle(center, radius, color, lineWidth, dashed = []) {
        this.setupLineStyle(color, lineWidth, dashed);

        ctx.beginPath();
        ctx.arc(center[0] + offset[0], center[1] + offset[1], radius, 0, Math.PI * 2);
        ctx.stroke();
    }

    static drawCircleFilled(center, radius, color) {
        ctx.fillStyle = color;

        ctx.beginPath();
        ctx.arc(center[0] + offset[0], center[1] + offset[1], radius, 0, Math.PI * 2);
        ctx.fill();
    }

    static drawOriginCross(size = 6, color = '#202020', lineWidth = 2) {
        this.drawSegment([-size, 0], [size, 0], color, lineWidth);
        this.drawSegment([0, -size], [0, size], color, lineWidth);
    }
}

window.addEventListener('load', () => {
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;
    offset = [Math.floor(canvas.width / 2), Math.floor(canvas.height / 2)];
});

