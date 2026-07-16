// Robot state lives entirely here; the VM just forwards builtin calls.
// Two stacked canvases: `lines` keeps drawn pixels, `overlay` shows the
// robot cursor so moving it never damages the drawing.

// Indonesian color words -> canvas colors (English words pass through).
const COLORS: Record<string, string> = {
  merah: 'red', biru: 'blue', hijau: 'green', kuning: 'yellow',
  hitam: 'black', putih: 'white', ungu: 'purple', jingga: 'orange',
  coklat: 'brown', 'merah muda': 'pink', 'abu-abu': 'gray',
};

export class Robot {
  x = 0;
  y = 0;
  heading = 0;
  penDown = true;
  color = '#222';

  // opts.speed: () => 1..10 (10 = instant)
  constructor(
    public lines: HTMLCanvasElement,
    public overlay: HTMLCanvasElement,
    public opts: { speed: () => number; stopped: () => boolean },
  ) {
    this.reset();
  }

  reset() {
    this.x = this.lines.width / 2;
    this.y = this.lines.height / 2;
    this.heading = -90; // up
    this.penDown = true;
    this.color = '#222';
    this.lines.getContext('2d')!.clearRect(0, 0, this.lines.width, this.lines.height);
    this.drawCursor();
  }

  drawCursor() {
    const ctx = this.overlay.getContext('2d')!;
    ctx.clearRect(0, 0, this.overlay.width, this.overlay.height);
    ctx.save();
    ctx.translate(this.x, this.y);
    ctx.rotate((this.heading + 90) * Math.PI / 180); // front (-Y) faces heading
    ctx.fillStyle = this.color;
    ctx.beginPath();                 // nose
    ctx.moveTo(0, -9); ctx.lineTo(4, -4); ctx.lineTo(-4, -4); ctx.closePath();
    ctx.fill();
    ctx.beginPath();                 // body
    ctx.roundRect(-7, -4, 14, 12, 3);
    ctx.fill();
    ctx.fillStyle = 'white';         // eyes (separate paths so they don't join)
    ctx.beginPath(); ctx.arc(-3, 1, 1.5, 0, Math.PI * 2); ctx.fill();
    ctx.beginPath(); ctx.arc(3, 1, 1.5, 0, Math.PI * 2); ctx.fill();
    ctx.restore();
  }

  moveTo(nx: number, ny: number) {
    if (this.penDown) {
      const ctx = this.lines.getContext('2d')!;
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

  async forward(dist: number) {
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

  // returns true if `canon` is a drawing builtin (and performs it)
  async op(canon: string, num: number, str: string) {
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
