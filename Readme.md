stockfish-mv.wasm
=================

WebAssembly port of the strong chess engine
[Stockfish](https://github.com/official-stockfish/Stockfish)
with [multi-variant support](https://github.com/ddugovic/Stockfish).
See it in action on https://lichess.org/analysis.

[![npm version](https://badge.fury.io/js/stockfish-mv.wasm.svg)](https://badge.fury.io/js/stockfish-mv.wasm)

Requirements
------------

Uses the latest WebAssembly threading proposal.

* Available as an [Origin Trial in Chrome 70](https://developers.google.com/web/updates/2018/10/wasm-threads).
* Behind a flag since Chrome 70: chrome://flags/#enable-webassembly-threads
* Behind a flag in Firefox Nightly: `javascript.options.shared_memory` in [about:config](about:config).

See [stockfish.js](https://github.com/niklasf/stockfish.js) for a more
portable but single-threaded version.

Feature detection:

```javascript
typeof WebAssembly === 'object' &&
WebAssembly.validate(Uint8Array.of(0x0, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00)) &&
typeof SharedArrayBuffer === 'function' &&
new WebAssembly.Memory({shared: true, initial: 8, maximum: 8}).buffer instanceof SharedArrayBuffer
```

Limitations
-----------

* Maximum number of threads determined at compile time (currently 8).
* Maximum size of hashtable determined at compile time (currently 16 MB).
* No Syzygy tablebase support.
* Exposed as a global variable in JavaScript (`window.Module`), only one
  instance per site.

Building
--------

Assuming [em++](https://github.com/kripken/emscripten) is available:

```
npm run-script prepare
```

Usage
-----

Requires `stockfish.js`, `stockfish.wasm`, `stockfish.js.mem` and
`pthread-main.js` (total size ~600K) to be served from the same directory.

```html
<script src="stockfish.js"></script>
<script>
Module.addMessageListener(function (line) {
  console.log(line);
});

Module.postMessage('uci');
</script>
```

License
-------

Thanks to the Stockfish team for sharing the engine under the GPL3.
Thanks to Daniel Dugovic et al. for the
[multi-variant support](https://github.com/ddugovic/Stockfish).
