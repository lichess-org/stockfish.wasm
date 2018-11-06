#!/bin/sh -e

mkdir -p dist
cd src
make ARCH=wasm build -j
cd ..
cat preamble.js src/stockfish.js > dist/stockfish.js
cp src/stockfish.wasm dist/
cp src/stockfish.js.mem dist/
cp src/pthread-main.js dist/
