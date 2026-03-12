// tabs/home.js
const ZERO_PCT = (128 / 255) * 100;
const toHex2 = n => Math.abs(n).toString(16).padStart(2, '0').toUpperCase();
const pct    = (v, mn, mx) => ((v - mn) / (mx - mn)) * 100;
// ── State ──────────────────────────────────────────────────────────────────
let panel    = null;
let effects  = [];
let selected = null;
let ws = new WebSocket(`ws://${location.host}/ws`);

// ── Refs ───────────────────────────────────────────────────────────────────
let elTrigger, elDropdown, elChevron, elSearch, elClearBtn, elBtnLabel, elList, elCount;

// ── Lifecycle ──────────────────────────────────────────────────────────────
export async function init(rootPanel) {
    panel = rootPanel;

    elTrigger  = panel.querySelector('#cb > button');
    elDropdown = panel.querySelector('#dropdown');
    elChevron  = panel.querySelector('#chevron');
    elSearch   = panel.querySelector('#search');
    elClearBtn = panel.querySelector('#clear-btn');
    elBtnLabel = panel.querySelector('#btn-label');
    elList     = panel.querySelector('#list');
    elCount    = panel.querySelector('#count');

    elTrigger.addEventListener('click', toggle);
    elSearch.addEventListener('input', onSearch);
    elClearBtn.addEventListener('click', clearSearch);

    initBrightness(panel);
    initSpeed(panel);
    initKFactor(panel);


    try {
        const res = await fetch('/api/effectsList');
        effects   = await res.json(); // [{ id: 1, name: "fire" }, ...]
    } catch (err) {
        console.error('Failed to load effects:', err);
        effects = [];
    }

    renderList(effects);
    updateCount(effects.length);

    selectItem(3);

    ws.onmessage = (e) => {
        let d = JSON.parse(e.data);

        if(d.b) panel.querySelector('#br').value = d.b
        if(d.s) panel.querySelector('#sp').value = d.s
        if(d.k) panel.querySelector('#kf').value = d.k
    }

    panel.querySelector('#br').dispatchEvent(new Event('input'));
    panel.querySelector('#sp').dispatchEvent(new Event('input'));
    panel.querySelector('#kf').dispatchEvent(new Event('input'));
}

export function resume(_panel) {
    document.addEventListener('click', onOutsideClick);
}

export function pause(_panel) {
    close();
    document.removeEventListener('click', onOutsideClick);
}

// ── Toggle ─────────────────────────────────────────────────────────────────
function toggle() {
    elDropdown.classList.contains('open') ? close() : open();
}

function open() {
    elDropdown.classList.add('open');
    elChevron.classList.add('open');
    elSearch.value = '';
    elClearBtn.classList.add('hidden');
    renderList(effects);
    updateCount(effects.length);
    document.addEventListener('click', onOutsideClick);
}

function close() {
    elDropdown.classList.remove('open');
    elChevron.classList.remove('open');
    document.removeEventListener('click', onOutsideClick);
}

function onOutsideClick(e) {
    if (!panel.querySelector('#cb').contains(e.target)) close();
}

// ── Search ─────────────────────────────────────────────────────────────────
function onSearch() {
    const q = elSearch.value.trim().toLowerCase();
    elClearBtn.classList.toggle('hidden', !q);
    const filtered = q
        ? effects.filter(e =>
            e.name.toLowerCase().includes(q) || String(e.id).includes(q)
        ): effects;
    renderList(filtered);
    updateCount(filtered.length);
}

function clearSearch() {
    elSearch.value = '';
    elClearBtn.classList.add('hidden');
    renderList(effects);
    updateCount(effects.length);
}

// ── Render ─────────────────────────────────────────────────────────────────
function renderList(items) {
    if (!items.length) {
        elList.innerHTML = `
        <li class="px-3 py-3 text-sm text-center text-text/30">
            Ничего не найдено
        </li>`;
        return;
    }

    elList.innerHTML = items.map(e => {
    const active = selected?.id === e.id;
    return `
        <li data-id="${e.id}"
            class="flex items-center gap-3 px-3 py-2.5 rounded-lg cursor-pointer text-sm transition-all
                    ${active
                        ? 'bg-primary/15 text-primary'
                        : 'text-text/50 hover:bg-primary/5 hover:text-text'}">
        <span class="w-7 text-right text-xs font-mono shrink-0 text-text/20">${e.id}.</span>
        <span class="flex-1">${e.name}</span>
        ${active ? `<span class="w-1.5 h-1.5 rounded-full shrink-0 bg-accent"></span>` : ''}
        </li>`;
    }).join('');

    elList.querySelectorAll('li[data-id]').forEach(li => {
        li.addEventListener('click', () => selectItem(Number(li.dataset.id)));
    });
}

function selectItem(id) {
    selected = effects.find(e => e.id === id);
    elBtnLabel.textContent = `${selected.id}. ${selected.name}`;
    elBtnLabel.className   = 'text-text transition-colors';
    close();
}

function updateCount(shown) {
    elCount.textContent = `${shown} / ${effects.length}`;
}

function updateHex(panel, br, sp, kf) {
    const kv = +kf.value;
    panel.querySelector('#hex-row').textContent =
        `0x${toHex2(+br.value)} · 0x${toHex2(+sp.value)} · ${kv >= 0 ? '+' : '-'}0x${toHex2(kv)}`;
}


function initBrightness(panel) {
    const input = panel.querySelector('#br');
    input.addEventListener('input', () => {
        const v = +input.value;
        panel.querySelector('#br-chip').textContent = v;
        panel.querySelector('#br-fill').style.width = pct(v, 0, 255) + '%';
        
        ws.send(JSON.stringify({b:Number(input.value)}));
        
        updateHex(panel,
            panel.querySelector('#br'),
            panel.querySelector('#sp'),
            panel.querySelector('#kf'));
    });
}

function initSpeed(panel) {
    const input = panel.querySelector('#sp');
    input.addEventListener('input', () => {
        const v = +input.value;
        panel.querySelector('#sp-chip').textContent = v;
        panel.querySelector('#sp-fill').style.width = pct(v, 0, 255) + '%';
        
        ws.send(JSON.stringify({s:Number(input.value)}));

        updateHex(panel,
            panel.querySelector('#br'),
            panel.querySelector('#sp'),
            panel.querySelector('#kf'));
    });
}

function initKFactor(panel) {
    const input = panel.querySelector('#kf');
    const neg   = panel.querySelector('#kf-neg');
    const pos   = panel.querySelector('#kf-pos');

    input.addEventListener('input', () => {
        const v = +input.value;
        panel.querySelector('#kf-chip').textContent = v >= 0 ? '+' + v : String(v);
        neg.classList.add('hidden');
        pos.classList.add('hidden');

        if (v > 0) {
            pos.classList.remove('hidden');
            pos.style.left  = ZERO_PCT + '%';
            pos.style.width = (v / 255 * 100) + '%';
        } else if (v < 0) {
            const w = Math.abs(v) / 255 * 100;
            neg.classList.remove('hidden');
            neg.style.left  = (ZERO_PCT - w) + '%';
            neg.style.width = w + '%';
        }

        ws.send(JSON.stringify({k:Number(input.value)}));

        updateHex(panel,
            panel.querySelector('#br'),
            panel.querySelector('#sp'),
            panel.querySelector('#kf'));
    });
}