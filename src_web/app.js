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

const loadedTabs = new Set();

async function switchTab(hash) {
    const target = hash || "#home";
    const id = target.replace("#", "");

    tabs.forEach(t => t.classList.remove("tab-active"));
    panels.forEach(p => p.classList.add("hidden"));

    const activeTab   = document.querySelector(`.tab[href="${target}"]`);
    const activePanel = document.querySelector(target);

    if (activeTab)   activeTab.classList.add("tab-active");
    if (activePanel) activePanel.classList.remove("hidden");

    if (!loadedTabs.has(id)) {
        loadedTabs.add(id);

        try {
            const res = await fetch(`./tabs/${id}.html`);
            if (res.ok) activePanel.innerHTML = await res.text();
        } catch (e) {}

        try {
            const mod = await import(`./tabs/${id}.js`);
            if (mod.init) mod.init(activePanel);
        } catch (e) {}
    }

    updateIndicator();
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