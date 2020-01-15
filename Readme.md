stockfish.wasm
==============

WebAssembly port of the strong chess engine
[Stockfish](https://github.com/official-stockfish/Stockfish). See it in action
on https://lichess.org/analysis.

[![npm version](https://badge.fury.io/js/stockfish.wasm.svg)](https://badge.fury.io/js/stockfish.wasm)

Requirements
------------

Uses the latest WebAssembly threading proposal.

### Chrome

* Since 79: Can allocate additional memory. The default allocation suffices
  for up to 2 threads and 16 MB hash.
* Enabled by default since 74 (desktop only)
* [Origin Trial from 70 to 75](https://developers.chrome.com/origintrials/#/view_trial/-5026017184145473535)
* Flag `chrome://flags/#enable-webassembly-threads` since 70

### Firefox

* Since Firefox 72: Structured cloning can no longer be enabled with flags.
* Since Firefox 71: Requires `javascript.options.shared_memory` and `dom.postMessage.sharedArrayBuffer.withCOOP_COEP` to be enabled in `about:flags` and these HTTP headers to be set on the top level response:

  ```
  Cross-Origin-Embedder-Policy: require-corp
  Cross-Origin-Opener-Policy: same-origin
  ```

* Firefox 68 to 70: Requires `javascript.options.shared_memory` to be enabled in `about:flags`

### Other browsers

No support.

See [stockfish.js](https://github.com/niklasf/stockfish.js) for a more
portable but single-threaded version.

### Feature detection

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

  // Structured cloning
  try {
    // You have to make sure nobody cares about these messages!
    window.postMessage(mem, '*');
  } catch (e) {
    return false;
  }

  // Growable shared memory (optional)
  try {
    mem.grow(8);
  } catch (e) {
    return false;
  }

  return true;
}
```

Current limitations
-------------------

* Hashtable: 1024 MB.
* Threads: 16.
* Can hang when UCI protocol is misused. (Do not send invalid commands or
  positions. While the engine is searching, do not change options or start
  additional searches).
* No Syzygy tablebase support.

Building
--------

Assuming [em++](https://github.com/kripken/emscripten) (>= 1.39.6) is available:

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

Optional: Callbacks assigned to `sf['onRuntimeInitialized']` and
`sf['onAbort']` will be called for successful/failed initialization.

License
-------

Thanks to the Stockfish team for sharing the engine under the GPL3.
