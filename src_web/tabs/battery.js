let intervalId = null;

export function init(panel) {
    resume(panel);
}

export function pause() {
    clearInterval(intervalId);
    intervalId = null;
}

export function resume(panel) {
    const update = async () => {
        try {
            const data = await fetch('/api/batinfo').then(r => r.json());
            panel.querySelectorAll('[data-key]').forEach(el => {
                const val = data[el.dataset.key];
                el.textContent = val !== undefined ? val.toFixed(2) : '—';
            });
        } catch {
            console.log("NOT CONECTED");
        }
    };

    update();
    intervalId = setInterval(update, 1000);
}