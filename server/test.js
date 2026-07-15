/* End-to-end test for the compile server. Needs arduino-cli + the esp32 core,
   so it skips (exit 0) when they're missing — same spirit as tests/diff.sh.
   Run: make test-server */

import { join } from "node:path";

const ROOT = join(import.meta.dir, "..");

async function have(cmd) {
    try { return (await Bun.spawn(cmd, { stdout: "pipe", stderr: "pipe" }).exited) === 0; }
    catch { return false; }
}
if (!(await have(["arduino-cli", "version"]))) {
    console.log("server/test: skipped (arduino-cli not installed)");
    process.exit(0);
}
const cores = await new Response(Bun.spawn(["arduino-cli", "core", "list"], { stdout: "pipe" }).stdout).text();
if (!cores.includes("esp32:esp32")) {
    console.log("server/test: skipped (esp32 core not installed)");
    process.exit(0);
}

const port = 18000 + Math.floor(Math.random() * 1000);
const server = Bun.spawn(["bun", join(import.meta.dir, "compile.js")], {
    env: { ...process.env, PORT: String(port) },
    stdout: "pipe",
    stderr: "inherit",
});
const url = `http://localhost:${port}/compile`;
for (let i = 0; ; i++) {
    try { await fetch(url, { method: "OPTIONS" }); break; }
    catch { if (i > 50) throw new Error("server never came up"); await Bun.sleep(100); }
}

let failed = 0;
function check(name, ok) {
    if (!ok) failed++;
    console.log(`${ok ? "ok" : "FAIL"} - ${name}`);
}

try {
    const blink = await Bun.file(join(ROOT, "tests/golden/ok/kedip.id.aksa")).text();
    const res = await fetch(url, {
        method: "POST",
        body: JSON.stringify({ source: blink, locale: "id", board: "esp32" }),
    });
    check("blink compiles (200)", res.status === 200);
    const { parts } = await res.json();
    check("returns flash parts", Array.isArray(parts) && parts.length === 4);
    const app = parts.find((p) => p.addr === 0x10000);
    check("app binary is real (>100KB)", app && atob(app.data).length > 100_000);

    const bad = await fetch(url, {
        method: "POST",
        body: JSON.stringify({ source: "ini bukan program {{{", locale: "id" }),
    });
    check("invalid source rejected (422)", bad.status === 422);

    const wrongBoard = await fetch(url, {
        method: "POST",
        body: JSON.stringify({ source: blink, board: "uno" }),
    });
    check("unknown board rejected (400)", wrongBoard.status === 400);
} finally {
    server.kill();
}

console.log(failed ? `server/test: ${failed} failed` : "server/test: all passed");
process.exit(failed ? 1 : 0);
