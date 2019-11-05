stockfish.wasm
==============

WebAssembly port of the strong chess engine
[Stockfish](https://github.com/official-stockfish/Stockfish). See it in action
on https://lichess.org/analysis.

[![npm version](https://badge.fury.io/js/stockfish.wasm.svg)](https://badge.fury.io/js/stockfish.wasm)

Requirements
------------

Uses the latest WebAssembly threading proposal.

* Enabled by default in Chrome 74 (desktop only).
* Available as an [Origin Trial since Chrome 70](https://developers.google.com/web/updates/2018/10/wasm-threads).
* Behind a flag since Chrome 70: `chrome://flags/#enable-webassembly-threads`
* Behind a flag in Firefox 68: `javascript.options.shared_memory` in [about:config](about:config).

See [stockfish.js](https://github.com/niklasf/stockfish.js) for a more
portable but single-threaded version.

Feature detection:

```javascript
function wasmThreadsSupported() {
  // WebAssembly 1.0
  var source = Uint8Array.of(0x0, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00);
  if (typeof WebAssembly !== 'object' || !WebAssembly.validate(source)) return false;

  // SharedArrayBuffer
  if (typeof SharedArrayBuffer !== 'function') return false;

  // Atomics
  if (typeof Atomics !== 'object') return false;

  // Shared memory
  var mem = new WebAssembly.Memory({shared: true, initial: 8, maximum: 16});
  if (!(mem.buffer instanceof SharedArrayBuffer)) return false;

  // Growable shared memory
  /* try {
    mem.grow(8);
  } catch (e) {
    return false;
  } */

  // Structured cloning
  try {
    // You have to make sure nobody cares about this message!
    window.postMessage(new WebAssembly.Module(source), '*');
  } catch (e) {
    return false;
  }

  return true;
}
```

Current limitations
-------------------

* Maximum size of hashtable determined at compile time (currently 16 MB).
  [(#3)](https://github.com/niklasf/stockfish.wasm/issues/3)
* Maximum number of threads determined at compile time (currently 4). Blocked
  on reserving more memory at runtime.
  [(#4)](https://github.com/niklasf/stockfish.wasm/issues/4)
* Can hang when UCI protocol is misused. (Do not send invalid commands or
  positions. While the engine is searching, do not change options or start
  additional searches).
* No Syzygy tablebase support.

Building
--------

Assuming [em++](https://github.com/kripken/emscripten) (>= 1.39.0) is available:

```
npm run-script prepare
```

Usage
-----

Requires `stockfish.js`, `stockfish.wasm` and `stockfish.worker.js`
(total size ~1.5M, ~200K gzipped) to be served from the same directory.

```html
<script src="stockfish.js"></script>
<script>
var sf = Stockfish();
sf.addMessageListener(function (line) {
  console.log(line);
});

sf.postMessage('uci');
</script>
```

License
-------

Thanks to the Stockfish team for sharing the engine under the GPL3.
