// Send-to-device: compile the program on the server, flash the returned
// firmware over WebSerial, then stream the board's serial output into the
// console. Chromium-only (navigator.serial); the button stays hidden elsewhere.
import { ESPLoader, Transport } from "esptool-js";

export const serialSupported = () => "serial" in navigator;

// ?server=https://... overrides where programs are sent to be compiled
const COMPILE_URL =
  new URLSearchParams(location.search).get("server") ??
  "http://localhost:8765/compile";

const b64 = (s: string) => Uint8Array.from(atob(s), (c) => c.charCodeAt(0));

// esptool-js chip name → server board id. Others (S3, original ESP32) are
// rejected as unsupported.
const CHIP: Record<string, string> = { "ESP32-C3": "c3", "ESP32-C6": "c6" };

export class Flasher {
  port: SerialPort | null = null;
  reader?: ReadableStreamDefaultReader<Uint8Array>;

  constructor(public put: (text: string, cls?: string) => void) {}

  // Compile + flash. Throws with a short code ('busy', 'internal', …) on
  // failure; the caller owns the kid-facing wording.
  async deploy(source: string, locale: string) {
    // Connect first so esptool-js can identify the chip, then compile for that
    // exact target — one port session does detect → compile → flash.
    this.port = await navigator.serial.requestPort();
    const transport = new Transport(this.port);
    try {
      const put = this.put;
      const loader = new ESPLoader({
        transport,
        baudrate: 921600,
        terminal: {
          clean() {},
          write: (t: string) => put(t, "info"),
          writeLine: (t: string) => put(t + "\n", "info"),
        },
      });
      // Native USB-Serial-JTAG (C3/C6) re-enumerates whenever the chip is
      // reset, and WebSerial can't re-acquire a re-enumerated device without a
      // fresh user gesture — so esptool-js's auto-reset drops the port
      // ("device has been lost"). Instead the board is put in download mode by
      // hand (hold BOOT, tap RESET) and we connect with no_reset: no reset, no
      // re-enumeration mid-flash. the manual button press is the
      // ceiling; auto needs a WebSerial reconnect-after-enumerate dance
      // esptool-js doesn't do here.
      await loader.main("no_reset");
      const board = CHIP[loader.chip.CHIP_NAME];
      if (!board) throw new Error("unsupported_chip");

      const res = await fetch(COMPILE_URL, {
        method: "POST",
        body: JSON.stringify({ source, locale, board }),
      }).catch(() => null);
      if (!res) throw new Error("no_server");
      if (!res.ok)
        throw new Error(
          (await res.json().catch(() => ({}))).error ?? "internal",
        );
      const { parts } = await res.json();

      await loader.writeFlash({
        fileArray: parts.map((p: { data: string; addr: number }) => ({
          data: b64(p.data),
          address: p.addr,
        })),
        flashMode: "keep",
        flashFreq: "keep",
        flashSize: "keep",
        eraseAll: false,
        compress: true,
      });
      // Reboot into run mode. esptool-js's HardReset only *releases* RTS (it
      // assumes RTS was already asserted), which on native USB-JTAG doesn't
      // pulse EN — the board just keeps running the download stub, so nothing
      // happens until someone taps RESET. Pulse EN ourselves (true→false), the
      // same "reset via RTS pin" esptool.py does. The reboot re-enumerates USB,
      // so a swallowed device-lost mid-pulse is expected.
      try {
        await transport.setDTR(false); // boot pin HIGH — reset into run, not download
        await transport.setRTS(true); // EN low → reset (setRTS re-applies DTR=high)
        await new Promise((r) => setTimeout(r, 100));
        await transport.setRTS(false); // EN high → boot the flashed app
      } catch {
        /* re-enumerated mid-pulse — the board is already rebooting */
      }
    } finally {
      await transport.disconnect().catch(() => {});
    }
  }

  // Read serial output line by line until disconnect(). Runtime errors arrive
  // as "! E101 3:0 <already-localized message>" — shown in error styling.
  async monitor() {
    // The flash reset just re-enumerated the device; give the re-appearing
    // port a few tries to open, and give up quietly rather than surfacing a
    // monitor hiccup as a flash failure.
    for (let i = 0; ; i++) {
      try {
        await this.port!.open({ baudRate: 115200 });
        break;
      } catch {
        if (i >= 8) return;
        await new Promise((r) => setTimeout(r, 250));
      }
    }
    this.reader = this.port!.readable.getReader();
    const dec = new TextDecoder();
    let buf = "";
    try {
      for (;;) {
        const { value, done } = await this.reader!.read();
        if (done) break;
        buf += dec.decode(value, { stream: true });
        let nl;
        while ((nl = buf.indexOf("\n")) >= 0) {
          const line = buf.slice(0, nl + 1);
          buf = buf.slice(nl + 1);
          this.put(line, /^! E\d+ \d+:\d+ /.test(line) ? "err" : undefined);
        }
      }
    } catch {
      /* port yanked or cancelled — either way the monitor is over */
    }
  }

  async disconnect() {
    await this.reader?.cancel().catch(() => {});
    this.reader?.releaseLock();
    await this.port?.close().catch(() => {});
    this.port = null;
  }
}
