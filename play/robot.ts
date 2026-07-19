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

  // Same robot as the site icon: navy body, white eyes, amber antenna and
  // mouth. Antenna points toward the heading; antenna ball shows pen color.
  drawCursor() {
    const ctx = this.overlay.getContext('2d')!;
    ctx.clearRect(0, 0, this.overlay.width, this.overlay.height);
    ctx.save();
    ctx.translate(this.x, this.y);
    ctx.rotate((this.heading + 90) * Math.PI / 180); // front (-Y) faces heading
    ctx.scale(0.9, 0.9);
    ctx.translate(-12, -12);         // icon is drawn in a 24x24 box
    ctx.strokeStyle = '#0f172a';     // antenna
    ctx.lineWidth = 1.6;
    ctx.beginPath(); ctx.moveTo(12, 6); ctx.lineTo(12, 3.5); ctx.stroke();
    ctx.fillStyle = this.color;
    ctx.beginPath(); ctx.arc(12, 2.6, 1.8, 0, Math.PI * 2); ctx.fill();
    ctx.fillStyle = '#0f172a';       // body
    ctx.beginPath(); ctx.roundRect(3, 6, 18, 15, 5); ctx.fill();
    ctx.fillStyle = 'white';         // eyes (separate paths so they don't join)
    ctx.beginPath(); ctx.arc(8.6, 12.2, 2, 0, Math.PI * 2); ctx.fill();
    ctx.beginPath(); ctx.arc(15.4, 12.2, 2, 0, Math.PI * 2); ctx.fill();
    ctx.fillStyle = '#f59e0b';       // mouth
    ctx.beginPath(); ctx.roundRect(9.5, 16.4, 5, 1.8, 0.9); ctx.fill();
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
