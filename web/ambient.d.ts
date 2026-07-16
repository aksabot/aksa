// Globals that exist at runtime but not in the standard DOM lib: the Web
// Serial API (Chromium-only) and the wasm module loaded by a <script> tag.
// Only the members this app actually touches are declared.

interface SerialPort {
  open(options: { baudRate: number }): Promise<void>;
  close(): Promise<void>;
  readable: ReadableStream<Uint8Array>;
}

interface Navigator {
  serial: {
    requestPort(): Promise<SerialPort>;
  };
}

interface AksaM {
  ccall(fn: string, ret: string, argTypes: string[], args: unknown[]): number;
  ccall(fn: string, ret: string, argTypes: string[], args: unknown[],
        opts: { async: true }): Promise<number>;
  UTF8ToString(ptr: number): string;
  _free(ptr: number): void;
  aksaOut: (text: string) => void;
  aksaHost: (canon: string, num: number, str: string) => Promise<number | boolean>;
  aksaInput: () => Promise<string>;
  aksaYield: () => Promise<number>;
}

declare function AksaModule(): Promise<AksaM>;
