const path = require('path');
const assert = require('assert');
const addon = require('..');

const DATA_DIR = path.resolve(__dirname, '../../data');

function run() {
  console.log('[RUN] ImgProc tests...');
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

module.exports = { run };