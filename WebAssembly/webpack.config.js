import path from 'path'
import {
  fileURLToPath
} from 'url';
import {
  dirname
} from 'path';
import fs from 'fs';
import autoprefixer from 'autoprefixer'

import HtmlWebpackPlugin from 'html-webpack-plugin'
import CopyPlugin from 'copy-webpack-plugin';
import {
  WebpackAssetsManifest
} from 'webpack-assets-manifest'

const __filename = fileURLToPath(
  import.meta.url);
const __dirname = dirname(__filename);

export default {
  entry: './src/main.js',
  output: {
    filename: '[name].[contenthash:8].js',
    path: path.resolve(__dirname, 'dist'),
  },

  module: {
    rules: [{
      test: /\.(scss)$/,
      use: [{
        // Adds CSS to the DOM by injecting a `<style>` tag
        loader: 'style-loader'
      }, {
        // Interprets `@import` and `url()` like `import/require()` and will resolve them
        loader: 'css-loader'
      }, {
        // Loader for webpack to process CSS with PostCSS
        loader: 'postcss-loader',
        options: {
          postcssOptions: {
            plugins: [
              autoprefixer
            ]
          }
        }
      }, {
        // Loads a SASS/SCSS file and compiles it to CSS
        loader: 'sass-loader',
        options: {
          sassOptions: {
            // Optional: Silence Sass deprecation warnings. See note below.
            silenceDeprecations: [
              'mixed-decls',
              'color-functions',
              'global-builtin',
              'import'
            ]
          }
        }
      }]
    }]
  },

  plugins: [
    new CopyPlugin({
      patterns: [{
        from: 'src/WebNative.worker.js',
        to: 'WebNative.worker.[contenthash:8].js', // 保持哈希一致
      }, {
        from: 'src/WebNative.js',
        to: 'WebNative.[contenthash:8].js', // 保持哈希一致
      }, {
        from: 'src/WebNative.wasm',
        to: 'WebNative.[contenthash:8].wasm', // 保持哈希一致
      }]
    }),

    new WebpackAssetsManifest({
      output: 'manifest.json', // 生成的映射表文件名
      publicPath: '/', // 资源公共路径
      customize(key, entry) { // 自定义条目格式（可选）
        return {
          key: entry.key,
          value: entry.value,
          size: entry.value.length
        };
      }
    }),
    new HtmlWebpackPlugin({
      template: 'src/index.html',
      filename: 'index.html',
      templateParameters: (compilation, assets, assetTags) => {
        const manifestPath = path.resolve(__dirname, 'dist/manifest.json');
        const manifest = fs.existsSync(manifestPath) ?
          JSON.parse(fs.readFileSync(manifestPath, 'utf8')) :
          {};
        return {
          compilation,
          webpackConfig: compilation.options,
          htmlWebpackPlugin: {
            tags: assetTags,
            files: assets,
            manifest: manifest,
          }
        };
      }
    })
  ],
  mode: "production",
}