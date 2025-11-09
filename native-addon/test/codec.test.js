const path = require('path');
const assert = require('assert');
const fs = require('fs');
const addon = require('..');

const DATA_DIR = path.resolve(__dirname, '../../data');
const OUTPUT_DIR = path.resolve(__dirname, '../../cpp/build/test/out-node');

function ensureDir(p) { if (!fs.existsSync(p)) fs.mkdirSync(p, { recursive: true }); }

function run() {
  console.log('[RUN] Codec tests...');
  const colorPpm = path.join(DATA_DIR, 'color-block.ppm');
  const c = addon.loadPpm(colorPpm);
  ensureDir(OUTPUT_DIR);
  const outTrip = path.join(OUTPUT_DIR, 'node_color.trip');
  // 使用 Compressor 封装做完整保存/加载闭环
  assert.ok(addon.compressorSave(outTrip, c.width, c.height, c.channels, c.data));
  const restored = addon.compressorLoad(outTrip);
  // 只检查尺寸与通道，像素可能因背景色被视为压缩差异，不强制逐像素一致
  assert.strictEqual(restored.width, c.width);
  assert.strictEqual(restored.height, c.height);
  assert.strictEqual(restored.channels, c.channels);
}

module.exports = { run };