import { defineConfig } from 'vite'
import { resolve } from 'path'
import tailwindcss from '@tailwindcss/vite'
//import { viteStaticCopy } from 'vite-plugin-static-copy'

export default defineConfig({
    root: 'src_web',
    base: '/',

    build: {
        outDir: resolve(__dirname, 'data/web'),
        emptyOutDir: true,
        minify: 'terser',

        rollupOptions: {
            input: resolve(__dirname, 'src_web/index.html'),
        }
    },
    plugins: [
        tailwindcss(),
        /*viteStaticCopy({
            targets: [
                { src: 'icons/*', dest: 'icons' }
            ]
        })*/
    ]
})