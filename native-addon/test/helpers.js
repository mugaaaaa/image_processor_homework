const assert = require('assert');

function checkSame(a, b) {
  assert.strictEqual(a.width, b.width);
  assert.strictEqual(a.height, b.height);
  assert.strictEqual(a.channels, b.channels);
  assert.strictEqual(a.data.length, b.data.length);
  assert.strictEqual(Buffer.compare(a.data, b.data), 0);
}

module.exports = { checkSame };