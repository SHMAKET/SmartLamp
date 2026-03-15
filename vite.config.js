import { defineConfig } from 'vite'
import { resolve } from 'path'
import tailwindcss from '@tailwindcss/vite'
import viteCompression from 'vite-plugin-compression'

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
        viteCompression({
            algorithm: 'gzip',
            ext: '.gz'
        })
    ],
    server: {
        proxy: {
            '/api': {
                target: 'http://192.168.3.1',
                changeOrigin: true
            },
            '/ws': {
                target: 'http://192.168.3.1',
                ws: true,
                changeOrigin: true
            }
        }
    }
})