let intervalId = null;
let count = 0;

export function init(panel) {
    resume(panel);
}

export function pause() {
    clearInterval(intervalId);
    intervalId = null;
}

export function resume(panel) {
    intervalId = setInterval(() => {
        count++;
        panel.querySelector('#counter1').textContent = count;
    }, 1000);
}