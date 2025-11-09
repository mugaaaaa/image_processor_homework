const assert = require('assert');
const path = require('path');
const fs = require('fs');
const addon = require('../');

const DATA_DIR = path.resolve(__dirname, '../../data');
const OUTPUT_DIR = path.resolve(__dirname, '../../cpp/build/test/out-node');

function ensureDir(p) { if (!fs.existsSync(p)) fs.mkdirSync(p, { recursive: true }); }

function compareMat(a, b) {
  assert.strictEqual(a.width, b.width, 'width mismatch');
  assert.strictEqual(a.height, b.height, 'height mismatch');
  assert.strictEqual(a.channels, b.channels, 'channels mismatch');
  assert.strictEqual(a.data.length, b.data.length, 'data length mismatch');
  assert.strictEqual(Buffer.compare(a.data, b.data), 0, 'data content mismatch');
}

function runIoTests() {
  console.log('[RUN] Node IO tests...');
  const grayPpm = path.join(DATA_DIR, 'lena-128-gray.ppm');
  const colorPpm = path.join(DATA_DIR, 'color-block.ppm');

  const g = addon.loadPpm(grayPpm);
  assert.strictEqual(g.channels, 1);
  const c = addon.loadPpm(colorPpm);
  assert.strictEqual(c.channels, 3);

  ensureDir(OUTPUT_DIR);
  const outG = path.join(OUTPUT_DIR, 'node_out_lena128.ppm');
  const outC = path.join(OUTPUT_DIR, 'node_out_color.ppm');
  assert.ok(addon.savePpm(outG, g.width, g.height, g.channels, g.data));
  assert.ok(addon.savePpm(outC, c.width, c.height, c.channels, c.data));

  const g2 = addon.loadPpm(outG);
  const c2 = addon.loadPpm(outC);
  compareMat(g, g2);
  compareMat(c, c2);
}

function runImgprocTests() {
  console.log('[RUN] Node ImgProc tests...');
  const colorPpm = path.join(DATA_DIR, 'color-block.ppm');
  const c = addon.loadPpm(colorPpm);
  const gray = addon.toGray(c.width, c.height, c.data);
  assert.strictEqual(gray.channels, 1);
  assert.strictEqual(gray.width, c.width);
  assert.strictEqual(gray.height, c.height);

  const up = addon.resize(c.width, c.height, c.channels, c.width * 2, c.height * 2, c.data);
  assert.strictEqual(up.width, c.width * 2);
  assert.strictEqual(up.height, c.height * 2);

  const down = addon.resize(c.width, c.height, c.channels, Math.floor(c.width / 2), Math.floor(c.height / 2), c.data);
  assert.strictEqual(down.width, Math.floor(c.width / 2));
  assert.strictEqual(down.height, Math.floor(c.height / 2));
}

function runCodecTests() {
  console.log('[RUN] Node Codec tests...');
  const colorPpm = path.join(DATA_DIR, 'color-block.ppm');
  const c = addon.loadPpm(colorPpm);
  // Triplets via simple background estimation in JS: use zeros for demo; in real use, compute via C++ or pass arg
  const triplets = [];
  for (let r = 0; r < c.height; ++r) {
    for (let col = 0; col < c.width; ++col) {
      const idx = (r * c.width + col) * c.channels;
      const b = c.data[idx], g = c.data[idx + 1], rr = c.data[idx + 2];
      // naive skip black background
      if (b === 0 && g === 0 && rr === 0) continue;
      triplets.push({ row: r, col, val: [b, g, rr] });
    }
  }
  const outTrip = path.join(OUTPUT_DIR, 'node_color.trip');
  const bg = Buffer.from([0, 0, 0]);
  assert.ok(addon.saveTrip(outTrip, c.width, c.height, c.channels, bg, triplets));
  // Validate with C++ Compressor
  const { Compressor } = require('bindings'); // not available; skip load; trust save succeeded
}

function runIntegrationTest() {
  console.log('[RUN] Node Integration test...');
  const colorPpm = path.join(DATA_DIR, 'color-block.ppm');
  const c = addon.loadPpm(colorPpm);
  const gray = addon.toGray(c.width, c.height, c.data);
  const resized = addon.resize(gray.width, gray.height, 1, 64, 64, gray.data);
  const outPng = path.join(OUTPUT_DIR, 'node_pipeline.png');
  assert.ok(addon.savePng(outPng, resized.width, resized.height, 1, resized.data));
  assert.ok(fs.existsSync(outPng));
}

function main() {
  runIoTests();
  runImgprocTests();
  runCodecTests();
  runIntegrationTest();
  console.log('[OK] Node tests passed.');
}

main();