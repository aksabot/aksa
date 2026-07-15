// Virtual board state lives entirely here; the VM just forwards builtin
// calls. Fixed pin map:
//   1  red LED        pin_on / pin_off
//   2  yellow LED     pin_on / pin_off
//   3  green LED      pin_on / pin_off
//   4  push button    pin_read -> 0/1 (held down = 1)
//   5  slider 0-100   pin_read_analog
//   6  buzzer         pin_on / pin_off (audible tone + glow)
//   7  fan            pin_on / pin_off (spins while on)

export class Board {
  // opts: { stopped: () => bool, notify: (text) => void, activate: () => void }
  constructor(root, opts) {
    this.opts = opts;
    this.leds = [root.querySelector('.led-1'), root.querySelector('.led-2'),
                 root.querySelector('.led-3')];
    this.btn = root.querySelector('#pin-btn');
    this.pot = root.querySelector('#pin-pot');
    this.buzzer = root.querySelector('#pin-buzzer');
    this.fan = root.querySelector('#pin-fan');
    this.pressed = false;
    this.osc = null;
    this.reset();
  }

  reset() {
    for (const led of this.leds) led.classList.remove('on');
    this.buzzer.classList.remove('on');
    this.fan.classList.remove('on');
    this.silence();
  }

  silence() {
    if (this.osc) { this.osc.stop(); this.osc = null; }
  }

  buzz(on) {
    this.buzzer.classList.toggle('on', on);
    if (!on) return this.silence();
    if (this.osc) return;
    this.audio = this.audio || new AudioContext();
    this.osc = this.audio.createOscillator();
    this.osc.type = 'square';
    this.osc.frequency.value = 440;
    this.osc.connect(this.audio.destination);
    this.osc.start();
  }

  set(pin, on) {
    if (pin >= 1 && pin <= 3) this.leds[pin - 1].classList.toggle('on', on);
    else if (pin === 6) this.buzz(on);
    else if (pin === 7) this.fan.classList.toggle('on', on);
    else this.opts.notify(pin);
  }

  read(pin, analog) {
    if (!analog && pin === 4) return this.pressed ? 1 : 0;
    if (analog && pin === 5) return +this.pot.value;
    this.opts.notify(pin);
    return 0;
  }

  // waits in short slices so the stop button interrupts long pauses
  async wait(ms) {
    const end = performance.now() + ms;
    while (performance.now() < end && !this.opts.stopped())
      await new Promise((r) => setTimeout(r, Math.min(50, end - performance.now())));
  }

  // returns false if `canon` is not a board builtin
  async op(canon, num) {
    if (this.opts.stopped()) return true; // swallow trailing ops after stop
    switch (canon) {
      case 'pin_on': this.opts.activate(); this.set(num, true); return true;
      case 'pin_off': this.opts.activate(); this.set(num, false); return true;
      case 'pin_read': this.opts.activate(); return this.read(num, false);
      case 'pin_read_analog': this.opts.activate(); return this.read(num, true);
      case 'wait': await this.wait(num); return true;
      default: return false;
    }
  }
}
