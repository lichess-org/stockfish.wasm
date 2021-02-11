#!/bin/bash

#
# Example
#   NODE=/usr/bin/node TC=60+0.6 ROUNDS=100 OPENING="-openings file=noob_3moves.epd format=epd policy=round -repeat -games 2" bash misc/match.sh
#

CUTECHESS_CLI=${CLI:-"cutechess-cli"}
NODE=${NODE:-"node"}
UCI_JS=${UCI_JS:-"$PWD/src/uci.js"}
ENGINE_ARGS=${NODE_ARGS:-"arg=--experimental-wasm-simd arg=--experimental-wasm-threads arg=--wasm-simd-post-mvp arg=$UCI_JS"}

CONCURRENCY=${CONCURRENCY:-1}
ROUNDS=${ROUNDS:-10}
TC=${TC:-"10+0.1"}
DRAW=${DRAW:-"-draw movenumber=40 movecount=4 score=10"}
RESIGN=${RESIGN:-"-resign movecount=4 score=1000"}
RESULT=${RESULT:-"-pgnout result.pgn"}
OPENING=${OPENING:-""}

set -x
$CUTECHESS_CLI \
  -tournament round-robin \
  -concurrency $CONCURRENCY \
  -each proto=uci \
  -engine tc=$TC name=nnue      "option.Use NNUE=true"  cmd=$NODE $ENGINE_ARGS \
  -engine tc=$TC name=classical "option.Use NNUE=false" cmd=$NODE $ENGINE_ARGS \
  -rounds $ROUNDS \
  $OPENING $DRAW $RESIGN $RESULT
