#pragma once

#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no, viewport-fit=cover">
<title>WiFi HID Controller</title>
<style>
:root {
  --bg: #0f111a;
  --surface: #1a1d2e;
  --surface-alt: #242942;
  --border: #323859;
  --primary: #00e5ff;
  --primary-glow: rgba(0, 229, 255, 0.25);
  --accent: #7c4dff;
  --success: #00e676;
  --danger: #ff5252;
  --warning: #ffd600;
  --text: #f0f3ff;
  --text-muted: #8c96be;
}
* { box-sizing: border-box; margin: 0; padding: 0; -webkit-tap-highlight-color: transparent; }
body {
  font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
  background-color: var(--bg);
  color: var(--text);
  display: flex;
  flex-direction: column;
  min-height: 100vh;
  user-select: none;
}
header {
  background: var(--surface);
  border-bottom: 1px solid var(--border);
  padding: 12px 16px;
  display: flex;
  align-items: center;
  justify-content: space-between;
  position: sticky;
  top: 0;
  z-index: 100;
}
.brand {
  display: flex;
  align-items: center;
  gap: 10px;
}
.brand-icon {
  width: 32px;
  height: 32px;
  border-radius: 8px;
  background: linear-gradient(135deg, var(--primary), var(--accent));
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 16px;
  font-weight: 900;
  color: #000;
}
.brand-title {
  font-size: 16px;
  font-weight: 700;
  letter-spacing: 0.5px;
}
.status-pill {
  font-size: 11px;
  font-weight: 600;
  padding: 4px 10px;
  border-radius: 20px;
  background: rgba(0, 230, 118, 0.15);
  color: var(--success);
  border: 1px solid rgba(0, 230, 118, 0.3);
  display: flex;
  align-items: center;
  gap: 6px;
}
.status-dot {
  width: 7px;
  height: 7px;
  border-radius: 50%;
  background-color: var(--success);
  box-shadow: 0 0 8px var(--success);
}
nav {
  display: flex;
  background: var(--surface);
  border-bottom: 1px solid var(--border);
}
.nav-btn {
  flex: 1;
  padding: 12px 6px;
  background: none;
  border: none;
  color: var(--text-muted);
  font-size: 13px;
  font-weight: 600;
  cursor: pointer;
  position: relative;
  transition: all 0.2s;
}
.nav-btn.active {
  color: var(--primary);
}
.nav-btn.active::after {
  content: "";
  position: absolute;
  bottom: 0;
  left: 15%;
  width: 70%;
  height: 3px;
  background: var(--primary);
  border-radius: 3px 3px 0 0;
  box-shadow: 0 0 10px var(--primary);
}
main {
  flex: 1;
  padding: 16px;
  display: flex;
  flex-direction: column;
  gap: 16px;
  max-width: 600px;
  margin: 0 auto;
  width: 100%;
}
.tab-content { display: none; flex-direction: column; gap: 14px; }
.tab-content.active { display: flex; }
.card {
  background: var(--surface);
  border: 1px solid var(--border);
  border-radius: 12px;
  padding: 14px;
  display: flex;
  flex-direction: column;
  gap: 12px;
}
.card-title {
  font-size: 12px;
  text-transform: uppercase;
  letter-spacing: 1px;
  color: var(--text-muted);
  font-weight: 700;
}
.input-row {
  display: flex;
  gap: 8px;
}
input[type="text"], textarea {
  flex: 1;
  background: var(--bg);
  border: 1px solid var(--border);
  border-radius: 8px;
  color: var(--text);
  padding: 12px 14px;
  font-size: 15px;
  outline: none;
  font-family: inherit;
  transition: border-color 0.2s;
}
input[type="text"]:focus, textarea:focus {
  border-color: var(--primary);
  box-shadow: 0 0 8px var(--primary-glow);
}
textarea {
  min-height: 100px;
  resize: vertical;
  font-family: monospace;
  font-size: 13px;
}
.btn {
  background: var(--surface-alt);
  color: var(--text);
  border: 1px solid var(--border);
  border-radius: 8px;
  padding: 10px 14px;
  font-size: 13px;
  font-weight: 600;
  cursor: pointer;
  display: inline-flex;
  align-items: center;
  justify-content: center;
  gap: 6px;
  transition: all 0.15s ease-out;
  touch-action: manipulation;
}
.btn:active {
  transform: scale(0.96);
  opacity: 0.9;
}
.btn-primary {
  background: linear-gradient(135deg, var(--primary), #00b0ff);
  color: #000;
  font-weight: 700;
  border: none;
  box-shadow: 0 4px 12px var(--primary-glow);
}
.btn-accent {
  background: linear-gradient(135deg, var(--accent), #6200ea);
  color: #fff;
  border: none;
}
.btn-danger {
  background: rgba(255, 82, 82, 0.15);
  color: var(--danger);
  border-color: rgba(255, 82, 82, 0.3);
}
.btn-toggle.active {
  background: var(--primary);
  color: #000;
  border-color: var(--primary);
  box-shadow: 0 0 10px var(--primary-glow);
}
.toggles-row {
  display: flex;
  align-items: center;
  justify-content: space-between;
  font-size: 13px;
  color: var(--text-muted);
}
.switch-label {
  display: flex;
  align-items: center;
  gap: 8px;
  cursor: pointer;
}
.switch-label input[type="checkbox"] {
  accent-color: var(--primary);
  width: 17px;
  height: 17px;
}
.key-grid {
  display: grid;
  grid-template-columns: repeat(4, 1fr);
  gap: 8px;
}
.key-grid-3 {
  grid-template-columns: repeat(3, 1fr);
}
.key-grid-6 {
  grid-template-columns: repeat(6, 1fr);
}
.key-grid-arrow {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 8px;
  max-width: 180px;
  margin: 0 auto;
}
.chip-row {
  display: flex;
  flex-wrap: wrap;
  gap: 6px;
}
.chip {
  padding: 6px 10px;
  border-radius: 6px;
  font-size: 11px;
  background: var(--surface-alt);
  border: 1px solid var(--border);
  color: var(--text);
  cursor: pointer;
  font-family: monospace;
}
.chip:active {
  background: var(--primary);
  color: #000;
}
#trackpad {
  height: 220px;
  background: var(--bg);
  border: 2px dashed var(--border);
  border-radius: 12px;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  color: var(--text-muted);
  font-size: 13px;
  touch-action: none;
  cursor: crosshair;
  position: relative;
  overflow: hidden;
}
#trackpad:active {
  border-color: var(--primary);
  background: rgba(0, 229, 255, 0.03);
}
.trackpad-indicator {
  width: 24px;
  height: 24px;
  border-radius: 50%;
  border: 2px solid var(--primary);
  position: absolute;
  pointer-events: none;
  display: none;
  transform: translate(-50%, -50%);
}
.footer-bar {
  text-align: center;
  font-size: 11px;
  color: var(--text-muted);
  padding: 12px;
  border-top: 1px solid var(--border);
}
</style>
</head>
<body>
<header>
  <div class="brand">
    <div class="brand-icon">HID</div>
    <div>
      <div class="brand-title">ESP32 Dongle</div>
    </div>
  </div>
  <div class="status-pill" id="statusPill">
    <div class="status-dot"></div>
    <span id="statusText">Ready</span>
  </div>
</header>

<nav>
  <button class="nav-btn active" onclick="switchTab('tabType')">Type</button>
  <button class="nav-btn" onclick="switchTab('tabKeys')">Keys</button>
  <button class="nav-btn" onclick="switchTab('tabShortcuts')">Shortcuts</button>
  <button class="nav-btn" onclick="switchTab('tabMouse')">Mouse</button>
</nav>

<main>
  <!-- TAB 1: TYPE -->
  <div id="tabType" class="tab-content active">
    <div class="card">
      <div class="card-title">Send Text to PC</div>
      <div class="input-row">
        <input type="text" id="textInput" placeholder="Type text to send directly..." autocomplete="off" autocorrect="off" autocapitalize="off" spellcheck="false">
        <button class="btn btn-primary" onclick="sendTextInput()">Send</button>
      </div>
      <div class="toggles-row">
        <label class="switch-label">
          <input type="checkbox" id="enterAfterCheck" checked> Press Enter after send
        </label>
        <label class="switch-label">
          <input type="checkbox" id="clearAfterCheck" checked> Clear on send
        </label>
      </div>
    </div>

    <div class="card">
      <div class="card-title">Quick Commands</div>
      <div class="chip-row">
        <button class="chip" onclick="quickSend('cmd')">cmd</button>
        <button class="chip" onclick="quickSend('powershell')">powershell</button>
        <button class="chip" onclick="quickSend('notepad')">notepad</button>
        <button class="chip" onclick="quickSend('calc')">calc</button>
        <button class="chip" onclick="quickSend('taskmgr')">taskmgr</button>
        <button class="chip" onclick="quickSend('exit')">exit</button>
        <button class="chip" onclick="quickSend('ls')">ls</button>
        <button class="chip" onclick="quickSend('dir')">dir</button>
        <button class="chip" onclick="quickSend('ipconfig')">ipconfig</button>
        <button class="chip" onclick="quickSend('clear')">clear</button>
      </div>
    </div>

    <div class="card">
      <div class="card-title">Essential Keys</div>
      <div class="key-grid">
        <button class="btn btn-accent" onclick="sendKey('enter')">Enter</button>
        <button class="btn" onclick="sendKey('backspace')">Bksp</button>
        <button class="btn" onclick="sendKey('tab')">Tab</button>
        <button class="btn" onclick="sendKey('space')">Space</button>
        <button class="btn" onclick="sendKey('esc')">Esc</button>
        <button class="btn" onclick="sendKey('delete')">Del</button>
        <button class="btn" onclick="sendKey('up')">Up</button>
        <button class="btn" onclick="sendKey('down')">Down</button>
      </div>
    </div>
  </div>

  <!-- TAB 2: KEYS -->
  <div id="tabKeys" class="tab-content">
    <div class="card">
      <div class="card-title">Sticky Modifiers</div>
      <div class="key-grid">
        <button class="btn btn-toggle" id="btnCtrl" onclick="toggleMod('ctrl')">Ctrl</button>
        <button class="btn btn-toggle" id="btnShift" onclick="toggleMod('shift')">Shift</button>
        <button class="btn btn-toggle" id="btnAlt" onclick="toggleMod('alt')">Alt</button>
        <button class="btn btn-toggle" id="btnGui" onclick="toggleMod('gui')">Win/Cmd</button>
      </div>
    </div>

    <div class="card">
      <div class="card-title">Navigation Arrows</div>
      <div class="key-grid-arrow">
        <div></div>
        <button class="btn" onclick="sendKey('up')">▲</button>
        <div></div>
        <button class="btn" onclick="sendKey('left')">◄</button>
        <button class="btn" onclick="sendKey('down')">▼</button>
        <button class="btn" onclick="sendKey('right')">►</button>
      </div>
    </div>

    <div class="card">
      <div class="card-title">Navigation Keys</div>
      <div class="key-grid">
        <button class="btn" onclick="sendKey('home')">Home</button>
        <button class="btn" onclick="sendKey('end')">End</button>
        <button class="btn" onclick="sendKey('pageup')">PgUp</button>
        <button class="btn" onclick="sendKey('pagedown')">PgDn</button>
      </div>
    </div>

    <div class="card">
      <div class="card-title">Function Keys</div>
      <div class="key-grid-6">
        <button class="btn" onclick="sendKey('f1')">F1</button>
        <button class="btn" onclick="sendKey('f2')">F2</button>
        <button class="btn" onclick="sendKey('f3')">F3</button>
        <button class="btn" onclick="sendKey('f4')">F4</button>
        <button class="btn" onclick="sendKey('f5')">F5</button>
        <button class="btn" onclick="sendKey('f6')">F6</button>
        <button class="btn" onclick="sendKey('f7')">F7</button>
        <button class="btn" onclick="sendKey('f8')">F8</button>
        <button class="btn" onclick="sendKey('f9')">F9</button>
        <button class="btn" onclick="sendKey('f10')">F10</button>
        <button class="btn" onclick="sendKey('f11')">F11</button>
        <button class="btn" onclick="sendKey('f12')">F12</button>
      </div>
    </div>
  </div>

  <!-- TAB 3: SHORTCUTS -->
  <div id="tabShortcuts" class="tab-content">
    <div class="card">
      <div class="card-title">Windows & System</div>
      <div class="key-grid">
        <button class="btn" onclick="sendShortcut('win_r')">Win + R</button>
        <button class="btn" onclick="sendShortcut('win_l')">Lock (Win+L)</button>
        <button class="btn" onclick="sendShortcut('win_d')">Desktop</button>
        <button class="btn" onclick="sendShortcut('win_e')">Explorer</button>
        <button class="btn" onclick="sendShortcut('alt_tab')">Alt + Tab</button>
        <button class="btn" onclick="sendShortcut('alt_f4')">Alt + F4</button>
        <button class="btn" onclick="sendShortcut('taskmgr')">Task Mgr</button>
        <button class="btn btn-danger" onclick="sendShortcut('ctrl_alt_del')">Ctrl+Alt+Del</button>
      </div>
    </div>

    <div class="card">
      <div class="card-title">Clipboard & Edit</div>
      <div class="key-grid">
        <button class="btn" onclick="sendShortcut('copy')">Copy (Ctrl+C)</button>
        <button class="btn" onclick="sendShortcut('paste')">Paste (Ctrl+V)</button>
        <button class="btn" onclick="sendShortcut('cut')">Cut (Ctrl+X)</button>
        <button class="btn" onclick="sendShortcut('undo')">Undo (Ctrl+Z)</button>
        <button class="btn" onclick="sendShortcut('select_all')">Select All</button>
        <button class="btn" onclick="sendShortcut('save')">Save (Ctrl+S)</button>
        <button class="btn" onclick="sendShortcut('find')">Find (Ctrl+F)</button>
        <button class="btn" onclick="sendShortcut('printscreen')">PrtScn</button>
      </div>
    </div>

    <div class="card">
      <div class="card-title">Script Runner (DuckyScript style)</div>
      <textarea id="scriptText" placeholder="GUI r
DELAY 500
STRING notepad
ENTER
DELAY 500
STRING Hello from my phone!
ENTER"></textarea>
      <div class="input-row">
        <button class="btn btn-primary" style="flex:1;" onclick="runScript()">Run Script</button>
        <button class="btn" onclick="loadScriptPreset('notepad')">Notepad</button>
        <button class="btn" onclick="loadScriptPreset('cmd')">CMD</button>
      </div>
    </div>
  </div>

  <!-- TAB 4: MOUSE & MEDIA -->
  <div id="tabMouse" class="tab-content">
    <div class="card">
      <div class="card-title">Virtual Touchpad</div>
      <div id="trackpad">
        <div class="trackpad-indicator" id="padDot"></div>
        <span>Drag finger to move mouse<br>Tap to Left Click</span>
      </div>
      <div class="key-grid-3">
        <button class="btn btn-accent" onclick="sendMouseClick('left')">Left Click</button>
        <button class="btn" onclick="sendMouseClick('middle')">Middle</button>
        <button class="btn" onclick="sendMouseClick('right')">Right Click</button>
      </div>
      <div class="key-grid">
        <button class="btn" onclick="sendMouseScroll(2)">Scroll Up</button>
        <button class="btn" onclick="sendMouseScroll(-2)">Scroll Down</button>
        <button class="btn" onclick="flipScreen()">Flip LCD</button>
        <button class="btn btn-danger" onclick="releaseAllKeys()">Release All</button>
      </div>
    </div>

    <div class="card">
      <div class="card-title">Media Controls</div>
      <div class="key-grid-3">
        <button class="btn" onclick="sendMedia('vol_down')">Vol -</button>
        <button class="btn" onclick="sendMedia('mute')">Mute</button>
        <button class="btn" onclick="sendMedia('vol_up')">Vol +</button>
        <button class="btn" onclick="sendMedia('prev')">|&#9664; Prev</button>
        <button class="btn btn-primary" onclick="sendMedia('play_pause')">&#9654;|| Play</button>
        <button class="btn" onclick="sendMedia('next')">Next &#9654;|</button>
      </div>
    </div>
  </div>
</main>

<div class="footer-bar">
  ESP32-S3 Composite HID &bull; 0.96" LCD &bull; AP Mode
</div>

<script>
const mods = { ctrl: false, shift: false, alt: false, gui: false };

function haptic() {
  if (navigator.vibrate) navigator.vibrate(15);
}

function switchTab(id) {
  haptic();
  document.querySelectorAll('.tab-content').forEach(t => t.classList.remove('active'));
  document.querySelectorAll('.nav-btn').forEach(b => b.classList.remove('active'));
  document.getElementById(id).classList.add('active');
  event.target.classList.add('active');
}

function updateStatus(text, ok = true) {
  const pill = document.getElementById('statusPill');
  const span = document.getElementById('statusText');
  span.innerText = text;
  pill.style.color = ok ? 'var(--success)' : 'var(--danger)';
  pill.style.borderColor = ok ? 'rgba(0, 230, 118, 0.3)' : 'rgba(255, 82, 82, 0.3)';
}

async function apiPost(url, data) {
  try {
    const res = await fetch(url, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(data)
    });
    if (res.ok) {
      updateStatus('Sent OK', true);
      return true;
    } else {
      updateStatus('Error', false);
      return false;
    }
  } catch (e) {
    updateStatus('Offline', false);
    return false;
  }
}

function toggleMod(mod) {
  haptic();
  mods[mod] = !mods[mod];
  const btn = document.getElementById('btn' + mod.charAt(0).toUpperCase() + mod.slice(1));
  if (mods[mod]) btn.classList.add('active');
  else btn.classList.remove('active');
}

function clearMods() {
  for (let k in mods) {
    mods[k] = false;
    const btn = document.getElementById('btn' + k.charAt(0).toUpperCase() + k.slice(1));
    if (btn) btn.classList.remove('active');
  }
}

function getActiveMods() {
  const active = [];
  if (mods.ctrl) active.push('ctrl');
  if (mods.shift) active.push('shift');
  if (mods.alt) active.push('alt');
  if (mods.gui) active.push('gui');
  return active;
}

function sendKey(key) {
  haptic();
  const activeMods = getActiveMods();
  apiPost('/api/key', { key: key, modifiers: activeMods });
  clearMods();
}

function sendTextInput() {
  haptic();
  const input = document.getElementById('textInput');
  const val = input.value;
  if (!val) return;
  const enter = document.getElementById('enterAfterCheck').checked;
  const clear = document.getElementById('clearAfterCheck').checked;
  apiPost('/api/type', { text: val, enter: enter });
  if (clear) input.value = '';
}

document.getElementById('textInput').addEventListener('keydown', function(e) {
  if (e.key === 'Enter') {
    sendTextInput();
  }
});

function quickSend(txt) {
  haptic();
  apiPost('/api/type', { text: txt, enter: true });
}

function sendShortcut(sc) {
  haptic();
  apiPost('/api/shortcut', { shortcut: sc });
}

function sendMedia(act) {
  haptic();
  apiPost('/api/media', { action: act });
}

function sendMouseClick(btn) {
  haptic();
  apiPost('/api/mouse', { click: btn });
}

function sendMouseScroll(val) {
  haptic();
  apiPost('/api/mouse', { wheel: val });
}

function flipScreen() {
  haptic();
  apiPost('/api/flip_screen', {});
}

function releaseAllKeys() {
  haptic();
  clearMods();
  apiPost('/api/key', { key: 'release_all' });
}

function loadScriptPreset(type) {
  const box = document.getElementById('scriptText');
  if (type === 'notepad') {
    box.value = "GUI r\nDELAY 500\nSTRING notepad\nENTER\nDELAY 600\nSTRING Hello from ESP32 WiFi Dongle!\nENTER";
  } else if (type === 'cmd') {
    box.value = "GUI r\nDELAY 500\nSTRING cmd\nENTER\nDELAY 600\nSTRING echo Running from ESP32 USB Dongle\nENTER";
  }
}

function runScript() {
  haptic();
  const txt = document.getElementById('scriptText').value;
  if (!txt.trim()) return;
  apiPost('/api/script', { script: txt });
}

// Touchpad Implementation
const pad = document.getElementById('trackpad');
let lastTouch = null;
let tapStartTime = 0;
let moved = false;

pad.addEventListener('touchstart', (e) => {
  e.preventDefault();
  if (e.touches.length === 1) {
    lastTouch = { x: e.touches[0].clientX, y: e.touches[0].clientY };
    tapStartTime = Date.now();
    moved = false;
  }
}, { passive: false });

pad.addEventListener('touchmove', (e) => {
  e.preventDefault();
  if (!lastTouch || e.touches.length !== 1) return;
  const touch = e.touches[0];
  const dx = (touch.clientX - lastTouch.x) * 1.5;
  const dy = (touch.clientY - lastTouch.y) * 1.5;
  
  if (Math.abs(dx) > 1 || Math.abs(dy) > 1) {
    moved = true;
    apiPost('/api/mouse', { dx: Math.round(dx), dy: Math.round(dy) });
    lastTouch = { x: touch.clientX, y: touch.clientY };
  }
}, { passive: false });

pad.addEventListener('touchend', (e) => {
  e.preventDefault();
  const elapsed = Date.now() - tapStartTime;
  if (!moved && elapsed < 250) {
    sendMouseClick('left');
  }
  lastTouch = null;
}, { passive: false });
</script>
</body>
</html>
)rawliteral";
