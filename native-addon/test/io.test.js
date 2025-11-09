const path = require('path');
const fs = require('fs');
const assert = require('assert');
const addon = require('..');
const { checkSame } = require('./helpers');

const DATA_DIR = path.resolve(__dirname, '../../data');
const OUTPUT_DIR = path.resolve(__dirname, '../../cpp/build/test/out-node');

function ensureDir(p) { if (!fs.existsSync(p)) fs.mkdirSync(p, { recursive: true }); }

function run() {
  console.log('[RUN] IO tests...');
  const grayPpm = path.join(DATA_DIR, 'lena-128-gray.ppm');
  const colorPpm = path.join(DATA_DIR, 'color-block.ppm');
  const g = addon.loadPpm(grayPpm);
  const c = addon.loadPpm(colorPpm);
  ensureDir(OUTPUT_DIR);
  const outG = path.join(OUTPUT_DIR, 'node_out_lena128.ppm');
  const outC = path.join(OUTPUT_DIR, 'node_out_color.ppm');
  assert.ok(addon.savePpm(outG, g.width, g.height, g.channels, g.data));
  assert.ok(addon.savePpm(outC, c.width, c.height, c.channels, c.data));
  const g2 = addon.loadPpm(outG);
  const c2 = addon.loadPpm(outC);
  checkSame(g, g2);
  checkSame(c, c2);
}

module.exports = { run };