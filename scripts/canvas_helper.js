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

