/** @type {import('tailwindcss').Config} */
module.exports = {
  darkMode: 'class',
  content: [
    "./src_web/**/*.html",
    "./src_web/**/*.js"
  ],
  theme: {
    extend: {
      colors: {
        'text': 'rgb(var(--text))',
        'background': 'rgb(var(--background))',
        'primary': 'rgb(var(--primary))',
        'secondary': 'rgb(var(--secondary))',
        'accent': 'rgb(var(--accent))',
      },
    },
  },
  plugins: [],
}