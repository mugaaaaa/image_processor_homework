const path = require('path');
const fs = require('fs');
const assert = require('assert');
const addon = require('..');

const DATA_DIR = path.resolve(__dirname, '../../data');
const OUTPUT_DIR = path.resolve(__dirname, '../../cpp/build/test/out-node');

function ensureDir(p) { if (!fs.existsSync(p)) fs.mkdirSync(p, { recursive: true }); }

function run() {
  console.log('[RUN] Integration test...');
  const colorPpm = path.join(DATA_DIR, 'color-block.ppm');
  const img = addon.loadPpm(colorPpm);
  const gray = addon.toGray(img.width, img.height, img.data);
  const resized = addon.resize(gray.width, gray.height, 1, 128, 128, gray.data);
  ensureDir(OUTPUT_DIR);
  const outPng = path.join(OUTPUT_DIR, 'node_integration.png');
  assert.ok(addon.savePng(outPng, resized.width, resized.height, 1, resized.data));
  assert.ok(fs.existsSync(outPng));
}

module.exports = { run };