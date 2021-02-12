# Stockfish, a UCI chess playing engine derived from Glaurung 2.1
# Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
# Copyright (C) 2008-2015 Marco Costalba, Joona Kiiski, Tord Romstad
# Copyright (C) 2015-2019 Marco Costalba, Joona Kiiski, Gary Linscott, Tord Romstad
#
# Stockfish is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Stockfish is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


### ==========================================================================
### Section 1. General Configuration
### ==========================================================================

### Executable name
EXE = stockfish.js

### Source and object files
SRCS = benchmark.cpp bitbase.cpp bitboard.cpp endgame.cpp evaluate.cpp main.cpp \
	material.cpp misc.cpp movegen.cpp movepick.cpp pawns.cpp position.cpp psqt.cpp \
	search.cpp thread.cpp timeman.cpp tt.cpp uci.cpp ucioption.cpp tune.cpp \
	nnue/evaluate_nnue.cpp nnue/features/half_kp.cpp nnue/math.cpp embedded_nnue_data.cpp

OBJS = $(notdir $(SRCS:.cpp=.o))

VPATH = nnue:nnue/features

### ==========================================================================
### Section 2. High-level Configuration
### ==========================================================================
#
# flag                --- Comp switch      --- Description
# ----------------------------------------------------------------------------
#
# debug = yes/no      --- -DNDEBUG         --- Enable/Disable debug mode
# optimize = yes/no   --- (-O3/-fast etc.) --- Enable/Disable optimizations
# wasm_simd = yes/no
# wasm_simd_post_mvp = yes/no
# minify_js = yes/no
#
# Note that Makefile is space sensitive, so when adding new architectures
# or modifying existing flags, you have to make sure there are no extra spaces
# at the end of the line for flag values.

### 2.1. General and architecture defaults

optimize = yes
debug = no
popcnt = yes
bits = 64
wasm_simd = no
wasm_simd_post_mvp = no
minify_js = yes

COMP = emscripten
EXE = stockfish.js

### ==========================================================================
### Section 3. Low-level Configuration
### ==========================================================================

### 3.1 Selecting compiler
CXXFLAGS += -Wall -Wcast-qual -fno-exceptions -std=c++17 $(EXTRACXXFLAGS)
DEPENDFLAGS += -std=c++17
LDFLAGS += $(EXTRALDFLAGS)

ifeq ($(COMP),emscripten)
	comp=clang
	CXX=em++
	EMFLAGS += -s MODULARIZE=1 -s EXPORT_NAME="Stockfish" -s ENVIRONMENT=web,worker,node -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=1
	EMFLAGS += -s EXIT_RUNTIME=0 -s "EXTRA_EXPORTED_RUNTIME_METHODS=['ccall']" -s "INCOMING_MODULE_JS_API=['print', 'postRun', 'onAbort', 'onRuntimeInitialized']" --pre-js pre.js
	EMFLAGS += -s ALLOW_MEMORY_GROWTH=1 -s INITIAL_MEMORY=67108864 -s MAXIMUM_MEMORY=2147483648
	EMFLAGS += -s FILESYSTEM=0 --closure 1
	EMFLAGS += -s STRICT=1 -s ASSERTIONS=0
	ifeq ($(minify_js),yes)
		EMFLAGS += --closure 1
	else
		EMFLAGS += --closure 0 -g1
	endif
	CXXFLAGS += $(EMFLAGS)
endif

### 3.2.1 Debugging
ifeq ($(debug),no)
	CXXFLAGS += -DNDEBUG
else
	CXXFLAGS += -g
endif

### 3.3 Optimization
ifeq ($(optimize),yes)
	CXXFLAGS += -O3
endif

### 3.4 Bits
ifeq ($(bits),64)
	CXXFLAGS += -DIS_64BIT
endif

### 3.5 prefetch
CXXFLAGS += -DNO_PREFETCH

### 3.6 popcnt
ifeq ($(popcnt),yes)
	CXXFLAGS += -DUSE_POPCNT
endif

### 3.7 SIMD
ifeq ($(wasm_simd_post_mvp),yes)
	CXXFLAGS += -msimd128 -DUSE_WASM_SIMD -DUSE_WASM_SIMD_POST_MVP
else ifeq ($(wasm_simd),yes)
	CXXFLAGS += -msimd128 -DUSE_WASM_SIMD
endif

### 3.8 Link Time Optimization
### This is a mix of compile and link time options because the lto link phase
### needs access to the optimization flags.
ifeq ($(optimize),yes)
ifeq ($(debug), no)
	CXXFLAGS += -flto=thin
	LDFLAGS += $(CXXFLAGS)
else
	LDFLAGS += $(EMFLAGS)
endif
endif

### ==========================================================================
### Section 4. Public Targets
### ==========================================================================

.PHONY: build clean objclean

build:
	$(MAKE) COMP=$(COMP) all

# clean all
clean: objclean
	@rm -f .depend *~ core

# clean binaries and objects
objclean:
	@rm -f $(EXE) *.o

# evaluation network (nnue)
nnuenet=nn-62ef826d1a6d.nnue

net:
	@echo "Default net: $(nnuenet)"
	$(eval nnuedownloadurl := https://tests.stockfishchess.org/api/nn/$(nnuenet))
	$(eval curl_or_wget := $(shell if hash curl 2>/dev/null; then echo "curl -skL"; elif hash wget 2>/dev/null; then echo "wget -qO-"; fi))
	@if test -f "$(nnuenet)"; then \
            echo "Already available."; \
         else \
            if [ "x$(curl_or_wget)" = "x" ]; then \
               echo "Automatic download failed: neither curl nor wget is installed. Install one of these tools or download the net manually"; exit 1; \
            else \
               echo "Downloading $(nnuedownloadurl)"; $(curl_or_wget) $(nnuedownloadurl) > $(nnuenet);\
            fi; \
        fi;
	$(eval shasum_command := $(shell if hash shasum 2>/dev/null; then echo "shasum -a 256 "; elif hash sha256sum 2>/dev/null; then echo "sha256sum "; fi))
	@if [ "x$(shasum_command)" != "x" ]; then \
	    if [ "$(nnuenet)" != "nn-"`$(shasum_command) $(nnuenet) | cut -c1-12`".nnue" ]; then \
                echo "Failed download or $(nnuenet) corrupted, please delete!"; exit 1; \
            fi \
         else \
            echo "shasum / sha256sum not found, skipping net validation"; \
        fi

embedded_nnue_data.cpp: net
	@echo "-- Generate embedded_nnue_data.cpp --"
	python ../misc/embedded_nnue_data.py $(nnuenet) > embedded_nnue_data.cpp


### ==========================================================================
### Section 5. Private Targets
### ==========================================================================

all: $(EXE) .depend

$(EXE): $(OBJS) pre.js
	+$(CXX) -o $@ $(OBJS) $(LDFLAGS)

.depend:
	-@$(CXX) $(DEPENDFLAGS) -MM $(SRCS) > $@ 2> /dev/null

-include .depend
