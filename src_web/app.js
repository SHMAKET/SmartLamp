const toggleTheme = document.getElementById('toggleTheme');
const burgerBtn   = document.getElementById('burgerBtn');
const nav         = document.getElementById('nav');
const overlay     = document.getElementById('navOverlay');

toggleTheme.addEventListener('click', () => {
    document.documentElement.classList.toggle('dark');
});

function openMenu() {
    nav.classList.remove('-translate-x-56');
    burgerBtn.classList.add('active');
    overlay.classList.remove('opacity-0', 'pointer-events-none');
    overlay.classList.add('opacity-100', 'pointer-events-auto');
}

function closeMenu() {
    nav.classList.add('-translate-x-56');
    burgerBtn.classList.remove('active');
    overlay.classList.add('opacity-0', 'pointer-events-none');
    overlay.classList.remove('opacity-100', 'pointer-events-auto');
}

burgerBtn.addEventListener('click', () => {
    nav.classList.contains('-translate-x-56') ? openMenu() : closeMenu();
});

overlay.addEventListener('click', () => { closeMenu(); });

// =========TABS=========
const indicator = document.getElementById("indicator");
const tabs      = document.querySelectorAll(".tab");
const panels    = document.querySelectorAll(".panel");

function updateIndicator() {
    const active = document.querySelector(".tab-active");
    if (!active) return;

    const r = active.getBoundingClientRect();
    const p = indicator.offsetParent.getBoundingClientRect();

    indicator.style.top    = (r.top - p.top) + "px";
    indicator.style.height = r.height + "px";
}

const TAB_MODULES = {
    '#home':     () => import('./tabs/home.js'),
    '#battery':  () => import('./tabs/battery.js'),
    '#settings': () => import('./tabs/settings.js'),
}
const loadedTabs = new Set();
let currentModule = null;
let currentPanel = null;

async function switchTab(hash) {
    const target = hash || '#home';

    // --- pause current ---
    if (currentModule?.pause) currentModule.pause(currentPanel);

    // --- swithc DOM ---
    document.querySelectorAll('.tab').forEach(t => t.classList.remove('tab-active'));
    document.querySelectorAll('.panel').forEach(p => p.classList.replace('flex', 'hidden'));

    const activeTab   = document.querySelector(`.tab[href="${target}"]`);
    const activePanel = document.querySelector(target);
    if (activeTab)   activeTab.classList.add('tab-active');
    if (activePanel) activePanel.classList.replace('hidden', 'flex');
    updateIndicator();

    if (!TAB_MODULES[target]) return;

    const mod = await TAB_MODULES[target]();
    currentModule = mod;
    currentPanel  = activePanel;

    if (!loadedTabs.has(target)) {
        mod.init?.(activePanel);
        loadedTabs.add(target);
    } else {
        mod.resume?.(activePanel);
    }
}

tabs.forEach(tab => {
    tab.addEventListener("click", e => {
        e.preventDefault();
        const hash = tab.getAttribute("href");
        history.pushState(null, "", hash);
        switchTab(hash);
    });
});

window.addEventListener("load", () => switchTab(location.hash));
window.addEventListener("popstate", () => switchTab(location.hash));

const ro = new ResizeObserver(() => updateIndicator());
ro.observe(document.body);
// =========TABS=========
const blobs = document.querySelectorAll(".blob");

const isMobile = window.innerWidth < 768;

function getPosition(blob, index) {

    const vw = window.innerWidth;
    const vh = window.innerHeight;

    let zoneStart, zoneSize;

    if (isMobile) {
        zoneSize = vh / blobs.length;
        zoneStart = zoneSize * index;
    } else {
        zoneSize = vw / blobs.length;
        zoneStart = zoneSize * index;
    }

    const rect = blob.getBoundingClientRect();

    let x, y;

    if (isMobile) {
        x = Math.random() * (vw - rect.width);
        y = zoneStart + Math.random() * (zoneSize - rect.height);
    } else {
        x = zoneStart + Math.random() * (zoneSize - rect.width);
        y = Math.random() * (vh - rect.height);
    }
    return { x, y };
}

function animate(blob, index) {

    const { x, y } = getPosition(blob, index);

    const scale = 0.8 + Math.random() * 0.3;
    const duration = 1500 + Math.random() * 1000;

    blob.animate(
        {
            transform: `translate(${x}px, ${y}px) scale(${scale})`
        },
        {
            duration: duration,
            easing: "ease-in-out",
            fill: "forwards"
        }
    ).onfinish = () => animate(blob, index);
}

blobs.forEach((blob, i) => {
    const { x, y } = getPosition(blob, i);
    blob.style.transform = `translate(${x}px, ${y}px)`;
    animate(blob, i);
});