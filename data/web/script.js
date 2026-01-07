const html = document.documentElement;
const btn = document.getElementById("theme-toggle");

const savedTheme = localStorage.getItem("theme");
if (savedTheme) {
    html.setAttribute("data-bs-theme", savedTheme);
} else {
    const prefersDark = window.matchMedia("(prefers-color-scheme: dark)").matches;
    html.setAttribute("data-bs-theme", prefersDark ? "dark" : "light");
}

btn.addEventListener("click", () => {
    const currentTheme = html.getAttribute("data-bs-theme");
    const newTheme = currentTheme === "dark" ? "light" : "dark";
    html.setAttribute("data-bs-theme", newTheme);
    localStorage.setItem("theme", newTheme);
});