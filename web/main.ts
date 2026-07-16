import { EditorView, basicSetup } from 'codemirror';
import { StreamLanguage } from '@codemirror/language';
import { linter } from '@codemirror/lint';
import { Compartment, Text } from '@codemirror/state';
import { Robot } from './robot.ts';
import { Board } from './board.ts';
import { Flasher, serialSupported } from './flash.ts';
import { LESSONS } from './lessons.ts';

// UI strings live here, not in locales/*.json (that loader only accepts
// keywords/builtins/errors sections).
interface UIStrings {
  run: string; stop: string; stopped: string;
  share: string; copied: string;
  flash: string; flashing: string; disconnect: string;
  flashFail: string; serverBusy: string;
  noPin: (n: number) => string;
  modeDevice: string; modeLang: string;
  examples: Record<string, string>;
}

const UI: Record<string, UIStrings> = {
  id: {
    run: 'Jalankan', stop: 'Berhenti', stopped: '— dihentikan —',
    share: 'Bagikan', copied: 'Tautan disalin — kirim ke temanmu!',
    flash: 'Kirim ke Perangkat', flashing: 'Mengirim…', disconnect: 'Putus Sambungan',
    flashFail: 'Gagal mengirim ke perangkat', serverBusy: 'Server sibuk — coba lagi sebentar',
    noPin: (n: number) => `Tidak ada pin ${n} di papan`,
    modeDevice: '🔌 Mode Perangkat', modeLang: '🤖 Mode Bahasa',
    examples: {
      'Kedip': 'ulangi 5 {\n    nyalakan(1)\n    tunggu(300)\n    matikan(1)\n    tunggu(300)\n}\n',
      'Tombol → Lampu': 'ulangi 20 {\n    jika (baca(4) == 1) {\n        nyalakan(2)\n    } lainnya {\n        matikan(2)\n    }\n    tunggu(100)\n}\n',
      'Kipas Saat Panas': 'buat sensor = 5\nbuat kipas = 7\nulangi 30 {\n    jika (baca_analog(sensor) > 30) {\n        nyalakan(kipas)\n    } lainnya {\n        matikan(kipas)\n    }\n    tunggu(200)\n}\n',
      'Lampu Lalu Lintas': 'ulangi 3 {\n    nyalakan(1)\n    tunggu(1000)\n    matikan(1)\n    nyalakan(2)\n    tunggu(400)\n    matikan(2)\n    nyalakan(3)\n    tunggu(1000)\n    matikan(3)\n}\n',
      'Permainan Reaksi': 'nyalakan(6)\ntunggu(300)\nmatikan(6)\nbuat hitungan = 0\nselama (baca(4) == 0 dan hitungan < 50) {\n    hitungan = hitungan + 1\n    tunggu(100)\n}\ntulis("Waktu reaksimu:", hitungan)\n',
    },
  },
  en: {
    run: 'Run', stop: 'Stop', stopped: '— stopped —',
    share: 'Share', copied: 'Link copied — send it to a friend!',
    flash: 'Send to Device', flashing: 'Sending…', disconnect: 'Disconnect',
    flashFail: 'Could not send to the device', serverBusy: 'Server is busy — try again in a moment',
    noPin: (n: number) => `There is no pin ${n} on the board`,
    modeDevice: '🔌 Device Mode', modeLang: '🤖 Language Mode',
    examples: {
      'Blink': 'repeat 5 {\n    turn_on(1)\n    wait(300)\n    turn_off(1)\n    wait(300)\n}\n',
      'Button → Light': 'repeat 20 {\n    if (read(4) == 1) {\n        turn_on(2)\n    } else {\n        turn_off(2)\n    }\n    wait(100)\n}\n',
      'Fan When Hot': 'make sensor = 5\nmake fan = 7\nrepeat 30 {\n    if (read_analog(sensor) > 30) {\n        turn_on(fan)\n    } else {\n        turn_off(fan)\n    }\n    wait(200)\n}\n',
      'Traffic Light': 'repeat 3 {\n    turn_on(1)\n    wait(1000)\n    turn_off(1)\n    turn_on(2)\n    wait(400)\n    turn_off(2)\n    turn_on(3)\n    wait(1000)\n    turn_off(3)\n}\n',
      'Reaction Game': 'turn_on(6)\nwait(300)\nturn_off(6)\nmake count = 0\nwhile (read(4) == 0 and count < 50) {\n    count = count + 1\n    wait(100)\n}\nprint("Your reaction time:", count)\n',
    },
  },
};

const $ = <T extends HTMLElement = HTMLElement>(id: string) => document.getElementById(id) as T;
const localeSel = $<HTMLSelectElement>('locale'), runBtn = $<HTMLButtonElement>('run'), stopBtn = $<HTMLButtonElement>('stop');
const flashBtn = $<HTMLButtonElement>('flash'), shareBtn = $<HTMLButtonElement>('share'), modeLink = $('mode-link');
const consoleEl = $('console'), errorsEl = $('errors');
const speedEl = $<HTMLInputElement>('speed'), resetBtn = $('reset'), examplesSel = $<HTMLSelectElement>('examples');
const lessonEl = $('lesson');

// One bundle serves both pages; each page declares what it has and the
// wiring below skips whatever is missing. Language page: robot canvas +
// lessons. Device page: board + flash + device examples.
const hasBoard = !!$('board');

let running = false;
let flashState: 'idle' | 'flashing' | 'monitoring' = 'idle';
let stopRequested = false;
let pendingInput: ((answer: string) => void) | null = null; // resolver of the promise ask() is awaiting
const localeJson: Record<string, string> = {};   // locale id -> fetched JSON text

function ui() { return UI[localeSel.value]; }

// every pristine doc a locale can show, in a stable order shared by locales
function docsFor(loc: string) {
  return hasBoard ? Object.values(UI[loc].examples)
                  : LESSONS[loc].map((l) => l.code);
}
function titles() {
  return hasBoard ? Object.keys(ui().examples)
                  : LESSONS[localeSel.value].map((l) => l.title);
}

function put(text: string, cls?: string) {
  const span = document.createElement('span');
  if (cls) span.className = cls;
  span.textContent = text;
  consoleEl.appendChild(span);
  consoleEl.scrollTop = consoleEl.scrollHeight;
}

// Tokenizer over the locale's own keyword/builtin words, so highlighting
// follows the selected language for free.
function aksaLanguage(json: string) {
  const dict = JSON.parse(json);
  const keywords = new Set(Object.keys(dict.keywords));
  const builtins = new Set(Object.keys(dict.builtins));
  return StreamLanguage.define({
    token(stream) {
      if (stream.eatSpace()) return null;
      if (stream.match('//')) { stream.skipToEnd(); return 'comment'; }
      if (stream.match(/^"[^"]*"?/)) return 'string';
      if (stream.match(/^\d[\d.]*/)) return 'number';
      if (stream.match(/^[\p{L}_][\p{L}\p{N}_]*/u)) {
        const w = stream.current();
        if (keywords.has(w)) return 'keyword';
        if (builtins.has(w)) return 'atom';
        return 'variableName';
      }
      stream.next();
      return null;
    },
  });
}

async function fetchLocale(loc: string) {
  if (!localeJson[loc])
    localeJson[loc] = await (await fetch(`../locales/${loc}.json`)).text();
  return localeJson[loc];
}

// Shared programs travel in the URL itself: #<locale>:<base64 of utf-8>.
function fromHash() {
  const m = location.hash.match(/^#(\w+):(.+)$/);
  if (!m || !UI[m[1]]) return null;
  try {
    const bytes = Uint8Array.from(atob(m[2]), (c) => c.charCodeAt(0));
    return { locale: m[1], code: new TextDecoder().decode(bytes) };
  } catch { return null; }
}

AksaModule().then(init);

async function init(M: AksaM) {
  const robot = $('lines') && new Robot($<HTMLCanvasElement>('lines'), $<HTMLCanvasElement>('overlay'), {
    speed: () => +speedEl.value,
    stopped: () => stopRequested,
  });

  const board = hasBoard && new Board($('board'), {
    stopped: () => stopRequested,
    notify: (pin) => put(`${ui().noPin(pin)}\n`, 'info'),
    activate: () => {},
  });
  if (board) {
    const potVal = $('pin-pot-val');
    $('pin-pot').oninput = () => { potVal.textContent = $<HTMLInputElement>('pin-pot').value; };
    const btn = $('pin-btn');
    btn.onpointerdown = () => { board.pressed = true; };
    btn.onpointerup = btn.onpointerleave = () => { board.pressed = false; };
  }

  resetBtn.onclick = () => { if (robot) robot.reset(); if (board) board.reset(); };

  // After stop, drawing and output are suppressed while the program races to
  // its end (the VM halts for real at its next yield checkpoint).
  M.aksaOut = (text) => { if (!stopRequested) put(text); };

  // Robot first, then the board; a return of false from both surfaces the
  // localized "can't be used here yet". Board reads return numbers.
  M.aksaHost = async (canon, num, str) => {
    if (robot) {
      const r = await robot.op(canon, num, str);
      if (r !== false) return r;
    }
    if (board) return board.op(canon, num);
    if (canon === 'wait') { // pausing still works without the board
      const end = performance.now() + num;
      while (performance.now() < end && !stopRequested)
        await new Promise((r) => setTimeout(r, Math.min(50, end - performance.now())));
      return true;
    }
    return false;
  };

  M.aksaInput = () => new Promise((resolve) => {
    const field = document.createElement('input');
    consoleEl.appendChild(field);
    consoleEl.appendChild(document.createTextNode('\n'));
    field.focus();
    pendingInput = (answer: string) => {
      pendingInput = null;
      field.disabled = true;
      field.value = answer;
      resolve(answer);
    };
    field.onkeydown = (ev) => { if (ev.key === 'Enter') pendingInput?.(field.value); };
  });

  // Awaiting a 0ms timeout yields to the browser so the page stays
  // responsive and the stop button works during tight loops.
  M.aksaYield = async () => {
    await new Promise((r) => setTimeout(r));
    return stopRequested ? 1 : 0;
  };

  interface AksaErr { line: number; col: number; msg: string }

  function check(src: string) {
    const ptr = M.ccall('aksa_wasm_check', 'number', ['string', 'string'],
                        [src, localeJson[localeSel.value]]);
    const errs = JSON.parse(M.UTF8ToString(ptr));
    M._free(ptr);
    return errs as AksaErr[];
  }

  function errorPos(doc: Text, e: AksaErr) {
    const line = doc.line(Math.min(Math.max(e.line, 1), doc.lines));
    const from = Math.min(line.from + Math.max(e.col - 1, 0), line.to);
    return { from, to: Math.min(from + 1, line.to) };
  }

  function renderPanel(errs: AksaErr[]) {
    errorsEl.textContent = '';
    for (const e of errs) {
      const row = document.createElement('div');
      row.textContent = e.msg;
      row.onclick = () => {
        const { from, to } = errorPos(view.state.doc, e);
        view.dispatch({ selection: { anchor: from, head: to },
                        scrollIntoView: true });
        view.focus();
      };
      errorsEl.appendChild(row);
    }
  }

  const aksaLinter = linter((v) => {
    if (running) return []; // never re-enter wasm while a run is suspended
    const errs = check(v.state.doc.toString());
    renderPanel(errs);
    return errs.map((e) => ({ ...errorPos(v.state.doc, e),
                              severity: 'error', message: e.msg }));
  });

  const shared = fromHash();
  if (shared) localeSel.value = shared.locale;
  const langComp = new Compartment();
  await fetchLocale(localeSel.value);
  const view = new EditorView({
    doc: shared ? shared.code : docsFor(localeSel.value)[0],
    extensions: [basicSetup, aksaLinter,
                 langComp.of(aksaLanguage(localeJson[localeSel.value]))],
    parent: $('editor'),
  });

  function showLesson(i: number) {
    if (lessonEl) lessonEl.textContent = LESSONS[localeSel.value][i].text;
  }

  function applyLabels() {
    runBtn.textContent = ui().run;
    stopBtn.textContent = ui().stop;
    shareBtn.textContent = ui().share;
    if (flashBtn)
      flashBtn.textContent = flashState === 'monitoring' ? ui().disconnect : ui().flash;
    modeLink.textContent = hasBoard ? ui().modeLang : ui().modeDevice;
    examplesSel.textContent = '';
    titles().forEach((title, i) => {
      const opt = document.createElement('option');
      opt.value = String(i);
      opt.textContent = title;
      examplesSel.appendChild(opt);
    });
    showLesson(+examplesSel.value || 0);
  }
  applyLabels();

  function setDoc(text: string) {
    view.dispatch({ changes: { from: 0, to: view.state.doc.length, insert: text } });
  }
  examplesSel.onchange = () => {
    setDoc(docsFor(localeSel.value)[+examplesSel.value]);
    showLesson(+examplesSel.value);
  };

  localeSel.onchange = async () => {
    const json = await fetchLocale(localeSel.value);
    const doc = view.state.doc.toString();
    // an untouched lesson/example swaps to its counterpart in the new locale
    let match = -1;
    for (const loc of Object.keys(UI)) {
      const i = docsFor(loc).indexOf(doc);
      if (i >= 0) match = i;
    }
    view.dispatch({
      effects: [langComp.reconfigure(aksaLanguage(json))],
      ...(match >= 0
          ? { changes: { from: 0, to: doc.length, insert: docsFor(localeSel.value)[match] } }
          : {}),
    });
    if (match >= 0) examplesSel.value = String(match);
    applyLabels();
  };

  runBtn.onclick = async () => {
    running = true;
    runBtn.disabled = true;
    stopBtn.disabled = false;
    stopRequested = false;
    consoleEl.textContent = '';
    if (robot) robot.reset();
    if (board) board.reset();
    const ptr = await M.ccall('aksa_wasm_run', 'number', ['string', 'string'],
                              [view.state.doc.toString(), localeJson[localeSel.value]],
                              { async: true });
    for (const e of JSON.parse(M.UTF8ToString(ptr)))
      put(`${e.msg}\n`, 'err');
    M._free(ptr);
    if (stopRequested) put(`${ui().stopped}\n`, 'info');
    running = false;
    runBtn.disabled = false;
    stopBtn.disabled = true;
  };

  stopBtn.onclick = () => {
    stopRequested = true;
    if (pendingInput) pendingInput('');
  };

  shareBtn.onclick = async () => {
    const src = view.state.doc.toString();
    const b64 = btoa(String.fromCharCode(...new TextEncoder().encode(src)));
    location.hash = `${localeSel.value}:${b64}`;
    await navigator.clipboard.writeText(location.href);
    put(`${ui().copied}\n`, 'info');
  };

  if (!flashBtn) return;
  const flasher = new Flasher(put);
  flashBtn.hidden = !serialSupported();
  flashBtn.onclick = async () => {
    if (flashState === 'monitoring') { await flasher.disconnect(); return; }
    if (running) return;
    const src = view.state.doc.toString();
    const errs = check(src);
    if (errs.length) { renderPanel(errs); return; } // fix it here first
    flashState = 'flashing';
    flashBtn.disabled = true;
    flashBtn.textContent = ui().flashing;
    consoleEl.textContent = '';
    try {
      await flasher.deploy(src, localeSel.value);
      flashState = 'monitoring';
      flashBtn.disabled = false;
      applyLabels();
      await flasher.monitor(); // runs until disconnect or unplug
    } catch (e) {
      const err = e instanceof Error ? e : new Error(String(e));
      if (err.name !== 'NotFoundError') // silent when the port picker is cancelled
        put(`${err.message === 'busy' ? ui().serverBusy : ui().flashFail}\n`, 'err');
      await flasher.disconnect();
    }
    flashState = 'idle';
    flashBtn.disabled = false;
    applyLabels();
  };
}
