import { EditorView, basicSetup } from 'codemirror';
import { StreamLanguage } from '@codemirror/language';
import { linter } from '@codemirror/lint';
import { Compartment } from '@codemirror/state';
import { Turtle } from './turtle.js';
import { Board } from './board.js';
import { Flasher, serialSupported } from './flash.js';

// UI strings live here, not in locales/*.json (that loader only accepts
// keywords/builtins/errors sections).
const UI = {
  id: {
    run: 'Jalankan', stop: 'Berhenti', stopped: '— dihentikan —',
    flash: 'Kirim ke Perangkat', flashing: 'Mengirim…', disconnect: 'Putus Sambungan',
    flashFail: 'Gagal mengirim ke perangkat', serverBusy: 'Server sibuk — coba lagi sebentar',
    noPin: (n) => `Tidak ada pin ${n} di papan`,
    sample: 'nama = tanya("Siapa namamu?")\ntulis("Halo,", nama)\nulangi 3 {\n    tulis("Aksa!")\n}\n',
    sampleName: 'Halo',
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
    flash: 'Send to Device', flashing: 'Sending…', disconnect: 'Disconnect',
    flashFail: 'Could not send to the device', serverBusy: 'Server is busy — try again in a moment',
    noPin: (n) => `There is no pin ${n} on the board`,
    sample: 'name = ask("What is your name?")\nprint("Hello,", name)\nrepeat 3 {\n    print("Aksa!")\n}\n',
    sampleName: 'Hello',
    examples: {
      'Blink': 'repeat 5 {\n    turn_on(1)\n    wait(300)\n    turn_off(1)\n    wait(300)\n}\n',
      'Button → Light': 'repeat 20 {\n    if (read(4) == 1) {\n        turn_on(2)\n    } else {\n        turn_off(2)\n    }\n    wait(100)\n}\n',
      'Fan When Hot': 'make sensor = 5\nmake fan = 7\nrepeat 30 {\n    if (read_analog(sensor) > 30) {\n        turn_on(fan)\n    } else {\n        turn_off(fan)\n    }\n    wait(200)\n}\n',
      'Traffic Light': 'repeat 3 {\n    turn_on(1)\n    wait(1000)\n    turn_off(1)\n    turn_on(2)\n    wait(400)\n    turn_off(2)\n    turn_on(3)\n    wait(1000)\n    turn_off(3)\n}\n',
      'Reaction Game': 'turn_on(6)\nwait(300)\nturn_off(6)\nmake count = 0\nwhile (read(4) == 0 and count < 50) {\n    count = count + 1\n    wait(100)\n}\nprint("Your reaction time:", count)\n',
    },
  },
};

// every pristine doc a locale can show, in a stable order shared by locales
function uiDocs(u) { return [u.sample, ...Object.values(u.examples)]; }

const $ = (id) => document.getElementById(id);
const localeSel = $('locale'), runBtn = $('run'), stopBtn = $('stop');
const flashBtn = $('flash');
const consoleEl = $('console'), errorsEl = $('errors');
const speedEl = $('speed'), resetBtn = $('reset'), examplesSel = $('examples');

let running = false;
let flashState = 'idle'; // 'flashing' | 'monitoring'
let stopRequested = false;
let pendingInput = null; // resolver of the promise ask() is awaiting
const localeJson = {};   // locale id -> fetched JSON text

function ui() { return UI[localeSel.value]; }

function put(text, cls) {
  const span = document.createElement('span');
  if (cls) span.className = cls;
  span.textContent = text;
  consoleEl.appendChild(span);
  consoleEl.scrollTop = consoleEl.scrollHeight;
}

// Tokenizer over the locale's own keyword/builtin words, so highlighting
// follows the selected language for free.
function aksaLanguage(json) {
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

async function fetchLocale(loc) {
  if (!localeJson[loc])
    localeJson[loc] = await (await fetch(`../locales/${loc}.json`)).text();
  return localeJson[loc];
}

AksaModule().then(init);

async function init(M) {
  const turtle = new Turtle($('lines'), $('overlay'), {
    speed: () => +speedEl.value,
    stopped: () => stopRequested,
  });

  // The right pane shows either the turtle canvas or the board; whichever
  // one a running program touches pulls its tab to the front.
  const showTab = (which) => {
    $('canvas').hidden = which !== 'turtle';
    $('board').hidden = which !== 'board';
    $('tab-turtle').classList.toggle('active', which === 'turtle');
    $('tab-board').classList.toggle('active', which === 'board');
  };
  $('tab-turtle').onclick = () => showTab('turtle');
  $('tab-board').onclick = () => showTab('board');

  const board = new Board($('board'), {
    stopped: () => stopRequested,
    notify: (pin) => put(`${ui().noPin(pin)}\n`, 'info'),
    activate: () => showTab('board'),
  });
  const potVal = $('pin-pot-val');
  $('pin-pot').oninput = () => { potVal.textContent = $('pin-pot').value; };
  const btn = $('pin-btn');
  btn.onpointerdown = () => { board.pressed = true; };
  btn.onpointerup = btn.onpointerleave = () => { board.pressed = false; };

  resetBtn.onclick = () => { turtle.reset(); board.reset(); };

  // After stop, drawing and output are suppressed while the program races to
  // its end (the VM halts for real at its next yield checkpoint).
  M.aksaOut = (text) => { if (!stopRequested) put(text); };

  // Turtle first, then the board; a return of false from both surfaces the
  // localized "can't be used here yet". Board reads return numbers.
  M.aksaHost = async (canon, num, str) => {
    const r = await turtle.op(canon, num, str);
    if (r !== false) { showTab('turtle'); return r; }
    return board.op(canon, num);
  };

  M.aksaInput = () => new Promise((resolve) => {
    const field = document.createElement('input');
    consoleEl.appendChild(field);
    consoleEl.appendChild(document.createTextNode('\n'));
    field.focus();
    pendingInput = (answer) => {
      pendingInput = null;
      field.disabled = true;
      field.value = answer;
      resolve(answer);
    };
    field.onkeydown = (ev) => { if (ev.key === 'Enter') pendingInput(field.value); };
  });

  // Awaiting a 0ms timeout yields to the browser so the page stays
  // responsive and the stop button works during tight loops.
  M.aksaYield = async () => {
    await new Promise((r) => setTimeout(r));
    return stopRequested ? 1 : 0;
  };

  function check(src) {
    const ptr = M.ccall('aksa_wasm_check', 'number', ['string', 'string'],
                        [src, localeJson[localeSel.value]]);
    const errs = JSON.parse(M.UTF8ToString(ptr));
    M._free(ptr);
    return errs;
  }

  function errorPos(doc, e) {
    const line = doc.line(Math.min(Math.max(e.line, 1), doc.lines));
    const from = Math.min(line.from + Math.max(e.col - 1, 0), line.to);
    return { from, to: Math.min(from + 1, line.to) };
  }

  function renderPanel(errs) {
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

  const langComp = new Compartment();
  await fetchLocale(localeSel.value);
  const view = new EditorView({
    doc: ui().sample,
    extensions: [basicSetup, aksaLinter,
                 langComp.of(aksaLanguage(localeJson[localeSel.value]))],
    parent: $('editor'),
  });

  function applyLabels() {
    runBtn.textContent = ui().run;
    stopBtn.textContent = ui().stop;
    flashBtn.textContent = flashState === 'monitoring' ? ui().disconnect : ui().flash;
    examplesSel.textContent = '';
    uiDocs(ui()).forEach((doc, i) => {
      const opt = document.createElement('option');
      opt.value = i;
      opt.textContent = i === 0 ? ui().sampleName : Object.keys(ui().examples)[i - 1];
      examplesSel.appendChild(opt);
    });
  }
  applyLabels();

  function setDoc(text) {
    view.dispatch({ changes: { from: 0, to: view.state.doc.length, insert: text } });
  }
  examplesSel.onchange = () => setDoc(uiDocs(ui())[+examplesSel.value]);

  localeSel.onchange = async () => {
    const json = await fetchLocale(localeSel.value);
    const doc = view.state.doc.toString();
    // an untouched sample/example swaps to its counterpart in the new locale
    let match = -1;
    for (const u of Object.values(UI)) {
      const i = uiDocs(u).indexOf(doc);
      if (i >= 0) match = i;
    }
    view.dispatch({
      effects: [langComp.reconfigure(aksaLanguage(json))],
      ...(match >= 0
          ? { changes: { from: 0, to: doc.length, insert: uiDocs(ui())[match] } }
          : {}),
    });
    applyLabels();
    if (match >= 0) examplesSel.value = match;
  };

  runBtn.onclick = async () => {
    running = true;
    runBtn.disabled = true;
    stopBtn.disabled = false;
    stopRequested = false;
    consoleEl.textContent = '';
    turtle.reset();
    board.reset();
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
      if (e.name !== 'NotFoundError') // silent when the port picker is cancelled
        put(`${e.message === 'busy' ? ui().serverBusy : ui().flashFail}\n`, 'err');
      await flasher.disconnect();
    }
    flashState = 'idle';
    flashBtn.disabled = false;
    applyLabels();
  };
}
