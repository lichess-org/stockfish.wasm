# stockfish-mv.wasm

WebAssembly port of the strong chess engine
[Stockfish](https://github.com/official-stockfish/Stockfish)
with [multi-variant support](https://github.com/ddugovic/Stockfish).
See it in action on https://lichess.org/analysis.

[![npm version](https://badge.fury.io/js/stockfish-mv.wasm.svg)](https://badge.fury.io/js/stockfish-mv.wasm)
[![CI](https://github.com/niklasf/stockfish.wasm/workflows/CI/badge.svg?branch=multi-variant)](https://github.com/niklasf/stockfish.wasm/actions?query=workflow%3ACI+branch%3Amulti-variant)
[![Passively maintained](https://img.shields.io/badge/passively%20maintained-x-yellow.svg)](#status)

## Status

This is forked from the point equivalent to `SF_classical` in multi-variant
Stockfish. Upstream is also only passively maintained at this point, so for
the time being this is the strongest multi-variant Stockfish available
for browsers.

New development is happening in https://github.com/ianfab/Fairy-Stockfish.

## Requirements

Uses the latest WebAssembly threading proposal. Requires these HTTP headers
on the top level response:

```
Cross-Origin-Embedder-Policy: require-corp
Cross-Origin-Opener-Policy: same-origin
```

And the following header on the included files:

```
Cross-Origin-Embedder-Policy: require-corp
```

### Chromium based (desktop only)

- Since Chromium 79: Full support.
- Chromium 74: Supports treading, but cannot allocate additional memory.
  The default allocation only suffices for 1 thread and 16 MB hash.
- Chromium 70 to 73: Needs flag `chrome://flags/#enable-webassembly-threads` or
  [Origin Trial](https://developers.chrome.com/origintrials/#/view_trial/-5026017184145473535).

### Firefox

- Since Firefox 79: Full support.
- Firefox 72 to 78: Structured cloning can no longer be enabled with flags, except on nightlies.
- Firefox 71: Requires `javascript.options.shared_memory` and `dom.postMessage.sharedArrayBuffer.withCOOP_COEP` to be enabled in `about:flags`.
- Firefox 68 to 70: Requires `javascript.options.shared_memory` to be enabled in `about:flags`

### Other browsers

No support.

See [stockfish.js](https://github.com/niklasf/stockfish.js) for a more
portable but single-threaded version.

### Feature detection

```javascript
function wasmThreadsSupported() {
  // WebAssembly 1.0
  const source = Uint8Array.of(0x0, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00);
  if (
    typeof WebAssembly !== "object" ||
    typeof WebAssembly.validate !== "function"
  )
    return false;
  if (!WebAssembly.validate(source)) return false;

  // SharedArrayBuffer
  if (typeof SharedArrayBuffer !== "function") return false;

  // Atomics
  if (typeof Atomics !== "object") return false;

  // Shared memory
  const mem = new WebAssembly.Memory({ shared: true, initial: 8, maximum: 16 });
  if (!(mem.buffer instanceof SharedArrayBuffer)) return false;

  // Structured cloning
  try {
    // You have to make sure nobody cares about these messages!
    window.postMessage(mem, "*");
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

## Current limitations

- Hashtable: 1024 MB. You may want to check
  [`navigator.deviceMemory`](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/deviceMemory)
  before allocating.
- Threads: 32. You may want to check
  [`navigator.hardwareConcurrency`](https://developer.mozilla.org/en-US/docs/Web/API/NavigatorConcurrentHardware/hardwareConcurrency).
  May be capped lower (e.g., `dom.workers.maxPerDomain` in Firefox).
- Can hang when UCI protocol is misused. (Do not send invalid commands or
  positions. While the engine is searching, do not change options or start
  additional searches).
- No NNUE support.
- No Syzygy tablebase support.

## Building

Assuming [em++](https://github.com/kripken/emscripten) (`^2.0.13`) is available:

```
npm run-script prepare
```

## Usage

Requires `stockfish.js`, `stockfish.wasm` and `stockfish.worker.js`
(total size ~600K, ~200K gzipped) to be served from the same directory.

```html
<script src="stockfish.js"></script>
<script>
  StockfishMv().then((sf) => {
    sf.addMessageListener((line) => {
      console.log(line);
    });

    sf.postMessage("uci");
  });
</script>
```

Or from recent node (v14.4.0 tested) with flags
`--experimental-wasm-threads --experimental-wasm-bulk-memory`:

```javascript
const StockfishMv = require("stockfish-mv.wasm"); // the module, not the file

StockfishMv().then((sf) => {
  // ...
});
```

## License

Thanks to the Stockfish team for sharing the engine under the GPL3.
Thanks to Daniel Dugovic et al. for the multi-variant support.
