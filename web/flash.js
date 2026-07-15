// Send-to-device: compile the program on the server, flash the returned
// firmware over WebSerial, then stream the board's serial output into the
// console. Chromium-only (navigator.serial); the button stays hidden elsewhere.
import { ESPLoader, Transport } from 'esptool-js';

export const serialSupported = () => 'serial' in navigator;

// ?server=https://... overrides where programs are sent to be compiled
const COMPILE_URL = new URLSearchParams(location.search).get('server')
  ?? 'http://localhost:8765/compile';

const b64 = (s) => Uint8Array.from(atob(s), (c) => c.charCodeAt(0));

export class Flasher {
  constructor(put) {
    this.put = put;
    this.port = null;
  }

  // Compile + flash. Throws with a short code ('busy', 'internal', …) on
  // failure; the caller owns the kid-facing wording.
  async deploy(source, locale) {
    const res = await fetch(COMPILE_URL, {
      method: 'POST',
      body: JSON.stringify({ source, locale, board: 'esp32' }),
    }).catch(() => null);
    if (!res) throw new Error('no_server');
    if (!res.ok) throw new Error((await res.json().catch(() => ({}))).error ?? 'internal');
    const { parts } = await res.json();

    this.port = await navigator.serial.requestPort();
    const transport = new Transport(this.port);
    try {
      const put = this.put;
      const loader = new ESPLoader({
        transport,
        baudrate: 921600,
        terminal: { clean() {}, write: (t) => put(t, 'info'), writeLine: (t) => put(t + '\n', 'info') },
      });
      await loader.main();
      await loader.writeFlash({
        fileArray: parts.map((p) => ({ data: b64(p.data), address: p.addr })),
        flashMode: 'keep',
        flashFreq: 'keep',
        flashSize: 'keep',
        eraseAll: false,
        compress: true,
      });
      await loader.after('hard_reset');
    } finally {
      await transport.disconnect().catch(() => {});
    }
  }

  // Read serial output line by line until disconnect(). Runtime errors arrive
  // as "! E101 3:0 <already-localized message>" — shown in error styling.
  async monitor() {
    await this.port.open({ baudRate: 115200 });
    this.reader = this.port.readable.getReader();
    const dec = new TextDecoder();
    let buf = '';
    try {
      for (;;) {
        const { value, done } = await this.reader.read();
        if (done) break;
        buf += dec.decode(value, { stream: true });
        let nl;
        while ((nl = buf.indexOf('\n')) >= 0) {
          const line = buf.slice(0, nl + 1);
          buf = buf.slice(nl + 1);
          this.put(line, /^! E\d+ \d+:\d+ /.test(line) ? 'err' : undefined);
        }
      }
    } catch { /* port yanked or cancelled — either way the monitor is over */ }
  }

  async disconnect() {
    await this.reader?.cancel().catch(() => {});
    this.reader?.releaseLock();
    await this.port?.close().catch(() => {});
    this.port = null;
  }
}
