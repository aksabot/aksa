// Turtle state lives entirely here; the VM just forwards builtin calls.
// Two stacked canvases: `lines` keeps drawn pixels, `overlay` shows the
// turtle cursor so moving it never damages the drawing.

// Indonesian color words -> canvas colors (English words pass through).
const COLORS = {
  merah: 'red', biru: 'blue', hijau: 'green', kuning: 'yellow',
  hitam: 'black', putih: 'white', ungu: 'purple', jingga: 'orange',
  coklat: 'brown', 'merah muda': 'pink', 'abu-abu': 'gray',
};

export class Turtle {
  // opts: { speed: () => 1..10 (10 = instant), stopped: () => bool }
  constructor(lines, overlay, opts) {
    this.lines = lines;
    this.overlay = overlay;
    this.opts = opts;
    this.reset();
  }

  reset() {
    this.x = this.lines.width / 2;
    this.y = this.lines.height / 2;
    this.heading = -90; // up
    this.penDown = true;
    this.color = '#222';
    this.lines.getContext('2d').clearRect(0, 0, this.lines.width, this.lines.height);
    this.drawCursor();
  }

  drawCursor() {
    const ctx = this.overlay.getContext('2d');
    ctx.clearRect(0, 0, this.overlay.width, this.overlay.height);
    ctx.save();
    ctx.translate(this.x, this.y);
    ctx.rotate((this.heading + 90) * Math.PI / 180);
    ctx.beginPath();
    ctx.moveTo(0, -8);
    ctx.lineTo(6, 8);
    ctx.lineTo(-6, 8);
    ctx.closePath();
    ctx.fillStyle = this.color;
    ctx.fill();
    ctx.restore();
  }

  moveTo(nx, ny) {
    if (this.penDown) {
      const ctx = this.lines.getContext('2d');
      ctx.strokeStyle = this.color;
      ctx.lineWidth = 2;
      ctx.lineCap = 'round';
      ctx.beginPath();
      ctx.moveTo(this.x, this.y);
      ctx.lineTo(nx, ny);
      ctx.stroke();
    }
    this.x = nx;
    this.y = ny;
    this.drawCursor();
  }

  async forward(dist) {
    const rad = this.heading * Math.PI / 180;
    const sx = this.x, sy = this.y;
    const tx = sx + Math.cos(rad) * dist;
    const ty = sy + Math.sin(rad) * dist;
    const total = Math.abs(dist);
    let travelled = 0;
    while (travelled < total) {
      const speed = this.opts.speed(); // read live: mid-run changes apply
      if (speed >= 10) break;          // max slider = instant
      if (this.opts.stopped()) return; // halt mid-segment
      travelled = Math.min(travelled + speed * speed, total);
      const t = travelled / total;
      this.moveTo(sx + (tx - sx) * t, sy + (ty - sy) * t);
      await new Promise((r) => requestAnimationFrame(r));
    }
    this.moveTo(tx, ty);
  }

  // returns true if `canon` is a turtle builtin (and performs it)
  async op(canon, num, str) {
    switch (canon) {
      case 'forward': await this.forward(num); return true;
      case 'backward': await this.forward(-num); return true;
      case 'turn_right': this.heading += num; this.drawCursor(); return true;
      case 'turn_left': this.heading -= num; this.drawCursor(); return true;
      case 'color':
        if (str) this.color = COLORS[str.toLowerCase()] || str;
        this.drawCursor();
        return true;
      case 'pen_up': this.penDown = false; return true;
      case 'pen_down': this.penDown = true; return true;
      default: return false;
    }
  }
}
