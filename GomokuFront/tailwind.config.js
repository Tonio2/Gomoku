/** @type {import('tailwindcss').Config} */
module.exports = {
  content: [
    "./src/**/*.{js,jsx,ts,tsx}",
  ],
  theme: {
    extend: {
      boxShadow: {
        'blur': '0 0 10px rgba(0, 255, 255, 0.5);',
        'blurLine': '0 0 5px rgba(0, 255, 255, 0.8)',
        'strongBlur': '0 0 10px #0FF',
        'buttonBlur': '0 0 15px rgba(0, 255, 255, 0.8);',
        'buttonBlurHover': '0 0 20px rgba(0, 255, 255, 1);',
      }
    }
  },
  plugins: [],
}

