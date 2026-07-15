import { EditorView, basicSetup } from 'codemirror';
import { StreamLanguage } from '@codemirror/language';
import { linter } from '@codemirror/lint';
import { Compartment } from '@codemirror/state';
import { Turtle } from './turtle.js';

// UI strings live here, not in locales/*.json (that loader only accepts
// keywords/builtins/errors sections).
const UI = {
  id: {
    run: 'Jalankan', stop: 'Berhenti', stopped: '— dihentikan —',
    sample: 'nama = tanya("Siapa namamu?")\ntulis("Halo,", nama)\nulangi 3 {\n    tulis("Aksa!")\n}\n',
  },
  en: {
    run: 'Run', stop: 'Stop', stopped: '— stopped —',
    sample: 'name = ask("What is your name?")\nprint("Hello,", name)\nrepeat 3 {\n    print("Aksa!")\n}\n',
  },
};

const $ = (id) => document.getElementById(id);
const localeSel = $('locale'), runBtn = $('run'), stopBtn = $('stop');
const consoleEl = $('console'), errorsEl = $('errors');
const speedEl = $('speed'), resetBtn = $('reset');

let running = false;
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
  resetBtn.onclick = () => turtle.reset();

  // After stop, drawing and output are suppressed while the program races to
  // its end (the VM halts for real at its next yield checkpoint).
  M.aksaOut = (text) => { if (!stopRequested) put(text); };

  // Turtle builtins draw; unknown names (hardware) return false -> localized
  // "can't be used here yet".
  M.aksaHost = (canon, num, str) => turtle.op(canon, num, str);

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
  }
  applyLabels();

  localeSel.onchange = async () => {
    const json = await fetchLocale(localeSel.value);
    const doc = view.state.doc.toString();
    const effects = [langComp.reconfigure(aksaLanguage(json))];
    const prev = Object.values(UI).find((u) => u.sample === doc);
    view.dispatch({
      effects,
      ...(prev ? { changes: { from: 0, to: doc.length, insert: ui().sample } } : {}),
    });
    applyLabels();
  };

  runBtn.onclick = async () => {
    running = true;
    runBtn.disabled = true;
    stopBtn.disabled = false;
    stopRequested = false;
    consoleEl.textContent = '';
    turtle.reset();
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
}
