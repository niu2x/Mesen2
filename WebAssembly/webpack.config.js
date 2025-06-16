import path from 'path'
import { fileURLToPath } from 'url';
import { dirname } from 'path';
import fs from 'fs';

import HtmlWebpackPlugin from 'html-webpack-plugin'
import CopyPlugin from 'copy-webpack-plugin';
import {WebpackAssetsManifest} from 'webpack-assets-manifest'

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

export default {
  entry: './src/main.js',
  output: {
    filename: '[name].[contenthash:8].js',
    path: path.resolve(__dirname, 'dist'),
  },

  plugins: [
    new CopyPlugin({
      patterns: [
        { 
          from: 'src/WebNative.worker.js',
          to: 'WebNative.worker.[contenthash:8].js', // 保持哈希一致
        },
        { 
          from: 'src/WebNative.js',
          to: 'WebNative.[contenthash:8].js', // 保持哈希一致
        },
        { 
          from: 'src/WebNative.wasm',
          to: 'WebNative.[contenthash:8].wasm', // 保持哈希一致
        }
      ]
    }),

    new WebpackAssetsManifest({
      output: 'manifest.json', // 生成的映射表文件名
      publicPath: '/',        // 资源公共路径
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
        const manifest = fs.existsSync(manifestPath) 
          ? JSON.parse(fs.readFileSync(manifestPath, 'utf8')) 
          : {};
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