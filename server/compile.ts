/* Aksa compile server: POST /compile {source, locale, board} → firmware parts.
   Receives kid source (never raw C), emits C with the native aksa binary, and
   compiles it as an Arduino sketch. Toolchain output stays server-side; the
   browser only ever sees stable error codes. Run: bun server/compile.ts */

import { mkdtemp, rm, mkdir, writeFile, readFile } from "node:fs/promises";
import { existsSync } from "node:fs";
import { join } from "node:path";
import { tmpdir } from "node:os";

const ROOT = join(import.meta.dir, "..");
const PORT = Number(process.env.PORT || 8765);
// Chip → Arduino FQBN. Detection (from esptool-js on connect) picks the key;
// add S3/etc. here when a board joins. ponytail: per-board GPIO pin map is the
// follow-up — `board` is the hook for it, but wiring isn't finalized yet.
const FQBN: Record<string, string> = {
    c3: "esp32:esp32:esp32c3",
    c6: "esp32:esp32:esp32c6",
};
const AKSA = join(ROOT, "aksa");
const COMPILE_TIMEOUT_MS = 180_000;
const MAX_BODY = 64 * 1024;
const MAX_RUNNING = 2;
const MAX_WAITING = 30;

/* Runtime files copied into every sketch (rt.c's include set is closed). */
const RUNTIME_FILES = [
    "core/rt.c", "core/rt.h", "core/error.c", "core/error.h",
    "core/locale.c", "core/locale.h", "core/lexer.h",
    "runtime-device/hal.h", "runtime-device/hal_esp32.cpp",
];
const INO = 'extern "C" int aksa_main(void);\nvoid setup() { aksa_main(); }\nvoid loop() {}\n';

async function run(cmd: string[], opts: { timeout?: number; cwd?: string } = {}) {
    const proc = Bun.spawn(cmd, { stdout: "pipe", stderr: "pipe", ...opts });
    let timedOut = false;
    const timer = opts.timeout && setTimeout(() => { timedOut = true; proc.kill(); }, opts.timeout);
    const [stdout, stderr] = await Promise.all([
        new Response(proc.stdout).text(), new Response(proc.stderr).text(),
    ]);
    const code = await proc.exited;
    if (timer) clearTimeout(timer);
    return { code, stdout, stderr, timedOut };
}

/* Startup checks: fail fast if the toolchain isn't there. */
if (!existsSync(AKSA)) { console.error("missing ./aksa binary — run `make aksa`"); process.exit(1); }
if ((await run(["arduino-cli", "version"])).code !== 0) { console.error("arduino-cli not found"); process.exit(1); }

/* boot_app0.bin lives in the esp32 core's tools; resolve once. */
const dataDir = (await run(["arduino-cli", "config", "get", "directories.data"])).stdout.trim();
const bootApp0 = (await Array.fromAsync(
    new Bun.Glob("packages/esp32/hardware/esp32/*/tools/partitions/boot_app0.bin").scan({ cwd: dataDir })
)).map((p) => join(dataDir, p))[0];
if (!bootApp0) { console.error("esp32 core not installed — run `arduino-cli core install esp32:esp32`"); process.exit(1); }

/* Tiny queue: a classroom is a burst of ~30; beyond that, tell them to retry. */
let running = 0;
const waiting: ((ok: boolean) => void)[] = [];
function acquire() {
    if (running < MAX_RUNNING) { running++; return Promise.resolve(true); }
    if (waiting.length >= MAX_WAITING) return Promise.resolve(false);
    return new Promise<boolean>((resolve) => waiting.push(resolve));
}
function release() {
    const next = waiting.shift();
    if (next) next(true); else running--;
}

const CORS = {
    "Access-Control-Allow-Origin": "*",
    "Access-Control-Allow-Headers": "content-type",
};
const fail = (status: number, error: string) => Response.json({ error }, { status, headers: CORS });

async function part(path: string, addr: number) {
    return { addr, data: Buffer.from(await readFile(path)).toString("base64") };
}

async function compile(source: string, locale: string, board: string) {
    const tmp = await mkdtemp(join(tmpdir(), "aksa-"));
    try {
        const src = join(tmp, "program.aksa");
        await writeFile(src, source);
        const emit = await run([AKSA, "emit", src, "--locale", locale], { cwd: ROOT });
        if (emit.code !== 0) return fail(422, "invalid_source");

        const sketch = join(tmp, "aksa");
        await mkdir(sketch);
        // "locale.h" would shadow the C standard header once the sketch dir is
        // on the include path (it breaks the toolchain's own C++ headers), so
        // it's renamed to ak_locale.h during assembly.
        for (const f of RUNTIME_FILES) {
            const name = f.split("/").pop()!;
            const text = (await readFile(join(ROOT, f), "utf8"))
                .replaceAll('#include "locale.h"', '#include "ak_locale.h"');
            await writeFile(join(sketch, name.startsWith("locale.") ? "ak_" + name : name), text);
        }
        await writeFile(join(sketch, "aksa.ino"), INO);
        await writeFile(join(sketch, "program.c"), "#define main aksa_main\n" + emit.stdout);
        const localeJson = await readFile(join(ROOT, "locales", `${locale}.json`), "utf8");
        await writeFile(join(sketch, "aksa_locale.h"),
            `static const char aksa_locale_json[] = ${JSON.stringify(localeJson)};\n`);

        const out = join(tmp, "out");
        const cc = await run(
            ["arduino-cli", "compile", "--fqbn", FQBN[board], "--output-dir", out, sketch],
            { timeout: COMPILE_TIMEOUT_MS },
        );
        if (cc.code !== 0) {
            console.error(`compile failed (timeout=${cc.timedOut}):\n${cc.stderr || cc.stdout}`);
            return fail(500, "internal");
        }

        // Separate parts, not the merged image: merged.bin is the full 4MB
        // flash (mostly padding); these four total a few hundred KB.
        const parts = [
            await part(join(out, "aksa.ino.bootloader.bin"), 0x0),
            await part(join(out, "aksa.ino.partitions.bin"), 0x8000),
            await part(bootApp0, 0xe000),
            await part(join(out, "aksa.ino.bin"), 0x10000),
        ];
        return Response.json({ parts }, { headers: CORS });
    } finally {
        await rm(tmp, { recursive: true, force: true });
    }
}

Bun.serve({
    port: PORT,
    idleTimeout: 240,
    async fetch(req) {
        const url = new URL(req.url);
        if (req.method === "OPTIONS") return new Response(null, { status: 204, headers: CORS });
        if (req.method !== "POST" || url.pathname !== "/compile") return fail(404, "bad_request");
        if (Number(req.headers.get("content-length")) > MAX_BODY) return fail(413, "bad_request");

        let body: any;
        try { body = await req.json(); } catch { return fail(400, "bad_request"); }
        const { source, locale = "id", board = "c3" } = body;
        if (typeof source !== "string" || source.length > MAX_BODY) return fail(400, "bad_request");
        if (!/^[a-z]{2,8}$/.test(locale) || !existsSync(join(ROOT, "locales", `${locale}.json`))) return fail(400, "bad_request");
        if (!FQBN[board]) return fail(400, "bad_request");

        if (!(await acquire())) return fail(429, "busy");
        try {
            return await compile(source, locale, board);
        } catch (e) {
            console.error(e);
            return fail(500, "internal");
        } finally {
            release();
        }
    },
});
console.log(`aksa compile server on :${PORT} (boards ${Object.keys(FQBN).join(", ")})`);
