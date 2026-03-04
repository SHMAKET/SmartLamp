export function init(panel) {
    let count = 0;
    const counterElement = document.getElementById("counter3");

    setInterval(() => {
        count++;
        counterElement.textContent = count;
    }, 1000);
}