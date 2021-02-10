Build and Run

```
# Build (after enabled emscripten tools e.g. by emsdk_env.sh)
cd src
make build -j 4 wasm_simd_post_mvp=yes

# Run on NodeJS
# NOTE: emscripten's node (emsdk/node/12.18.1_64bit/bin/node) is old and doesn't seem to compatible with latest simd (cf. https://github.com/emscripten-core/emscripten/issues/11484)
/usr/bin/node --version
v15.8.0

# Example benchmark
/usr/bin/node --experimental-wasm-{simd,threads} --experimental-repl-await --wasm-simd-post-mvp
> const sf = await require("./stockfish")();
> sf.postMessage("bench 16 1 22 current depth classical");
Total time (ms) : 5056
Nodes searched  : 3478378
Nodes/second    : 687970

> sf.postMessage("bench 16 1 22 current depth NNUE");
Total time (ms) : 6424
Nodes searched  : 2670151
Nodes/second    : 415652
```

Comparing to other build type

```
# [ with `wasm_simd=yes` ]
> sf.postMessage("bench 16 1 22 current depth NNUE");
Total time (ms) : 9472
Nodes searched  : 2670151
Nodes/second    : 281899

# [ without `wasm_simd` ]
> sf.postMessage("bench 16 1 22 current depth NNUE");
Total time (ms) : 54008
Nodes searched  : 2670151
Nodes/second    : 49439
```

Run on browser

```
python misc/server.py # then open http://localhost:8000/misc/test.html
```

Benchmark `evaluate` (essentially this measures the performance of matrix(32x512)-vector(512) multiplication)

```
/usr/bin/node --experimental-wasm-{simd,threads} --experimental-repl-await --wasm-simd-post-mvp
> const sf = await require("./stockfish")();
> sf.postMessage("setoption name Use NNUE value true")
> sf.postMessage("bench_eval")
1.860234 usec (stddev: 0.050028, min: 1.810873, max: 1.951648, n: 80000, r: 10)
> sf.postMessage("setoption name Use NNUE value false") # Non-NNUE case is not so relevant because Stockfish caches a lot of classical evaluation
> sf.postMessage("bench_eval")
447.088706 nsec (stddev: 10.019180, min: 434.404873, max: 465.951900, n: 400000, r: 10)
```

Feature detection for `i32x4.dot_i16x8_s` (see `misc/feature_detection.wat` for how these bytes are generated)

```
/usr/bin/node --experimental-wasm-simd
> data = Uint8Array.from([0, 97, 115, 109, 1, 0, 0, 0, 1, 5, 1, 96, 0, 1, 123, 3, 2, 1, 0, 7, 8, 1, 4, 116, 101, 115, 116, 0, 0, 10, 15, 1, 13, 0, 65, 0, 253, 17, 65, 0, 253, 17, 253, 186, 1, 11])
> WebAssembly.validate(data)
false

/usr/bin/node --experimental-wasm-simd --wasm-simd-post-mvp
> data = Uint8Array.from([0, 97, 115, 109, 1, 0, 0, 0, 1, 5, 1, 96, 0, 1, 123, 3, 2, 1, 0, 7, 8, 1, 4, 116, 101, 115, 116, 0, 0, 10, 15, 1, 13, 0, 65, 0, 253, 17, 65, 0, 253, 17, 253, 186, 1, 11])
> WebAssembly.validate(data)
true
```

References

- WASM SIMD Specification (https://github.com/webassembly/simd/blob/master/proposals/simd/SIMD.md)
- WASM SIMD header (https://github.com/llvm/llvm-project/blob/main/clang/lib/Headers/wasm_simd128.h)
- WASM SIMD builtin to experiment with "post-mvp" features (https://github.com/llvm/llvm-project/blob/main/clang/include/clang/Basic/BuiltinsWebAssembly.def)
- Emscripten's x86 SIMD emulation by WASM SIMD (https://github.com/emscripten-core/emscripten/blob/master/system/include/compat/xmmintrin.h)
- V8's WASM opcode list (https://github.com/v8/v8/blob/master/src/wasm/wasm-opcodes.h)
- SpiderMonkey's WASM opcode list (https://github.com/mozilla/gecko-dev/blob/master/js/src/wasm/WasmConstants.h)
