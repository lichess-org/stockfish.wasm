#include "math.h"

#if defined(USE_WASM_SIMD)
#include <wasm_simd128.h>

namespace wasm_simd {

template<int n, int m>
void affine(const int8_t A[m][n], const uint8_t x[n], const int32_t b[m], int32_t y[m]) {
  static_assert(n % 16 == 0);

  //
  // Dot product of two SIMD vectors
  //
  [[maybe_unused]] auto dot_i8x16 = [](__i8x16 a, __i8x16 b) -> __i32x4 {
    __i16x8 a_lo = wasm_i16x8_widen_low_i8x16(a);
    __i16x8 a_hi = wasm_i16x8_widen_high_i8x16(a);
    __i16x8 b_lo = wasm_i16x8_widen_low_i8x16(b);
    __i16x8 b_hi = wasm_i16x8_widen_high_i8x16(b);
    #if defined(USE_WASM_SIMD_POST_MVP)
      // [ With `i32x4.dot_i16x8_s` ]
      // Not officially available but we can directly access as clang builtin
      return wasm_i32x4_add(__builtin_wasm_dot_s_i32x4_i16x8(a_lo, b_lo), __builtin_wasm_dot_s_i32x4_i16x8(a_hi, b_hi));
    #else
      // [ Without `i32x4.dot_i16x8_s` ]
      // NOTE: This version is slower than running "dot_i16x8" twice. So not used when USE_WASM_SIMD_POST_MVP is off
      __i16x8 w_lo = wasm_i16x8_mul(a_lo, b_lo);
      __i16x8 w_hi = wasm_i16x8_mul(a_hi, b_hi);
      __i32x4 u_lo = wasm_i32x4_add(wasm_i32x4_widen_low_i16x8(w_lo), wasm_i32x4_widen_high_i16x8(w_lo));
      __i32x4 u_hi = wasm_i32x4_add(wasm_i32x4_widen_low_i16x8(w_hi), wasm_i32x4_widen_high_i16x8(w_hi));
      return wasm_i32x4_add(u_lo, u_hi);
    #endif
  };

  [[maybe_unused]] auto dot_i16x8 = [](__i16x8 a, __i16x8 b) -> __i32x4 {
    __i16x8 c = wasm_i16x8_mul(a, b);
    return wasm_i32x4_add(wasm_i32x4_widen_low_i16x8(c), wasm_i32x4_widen_high_i16x8(c));
  };

  //
  // Horizontal sum
  //
  [[maybe_unused]] auto hadd = [&](__i32x4 x0, __i32x4 x1) -> __i32x4 {
    return wasm_i32x4_add(wasm_v32x4_shuffle(x0, x1, 0, 2, 4, 6), wasm_v32x4_shuffle(x0, x1, 1, 3, 5, 7));
  };

  [[maybe_unused]] auto haddx4 = [&](__i32x4 x0, __i32x4 x1, __i32x4 x2, __i32x4 x3) -> __i32x4 {
    return hadd(hadd(x0, x1), hadd(x2, x3));
  };

  //
  // Dot product of two vectors
  //
  [[maybe_unused]] auto dot = [&](const int8_t* a, const uint8_t* x, const int32_t* b, int32_t* out) {
    __i32x4 z = wasm_i32x4_splat(0);
    #if defined(USE_WASM_SIMD_POST_MVP)
      for (int j = 0; j < n; j += 16) {
        z = wasm_i32x4_add(z, dot_i8x16(wasm_v128_load(&a[j]), wasm_v128_load(&x[j])));
      }
    #else
      for (int j = 0; j < n; j += 8) {
        z = wasm_i32x4_add(z, dot_i16x8(wasm_i16x8_load_8x8(&a[j]), wasm_i16x8_load_8x8(&x[j])));
      }
    #endif
    out[0] = b[0] + z[0] + z[1] + z[2] + z[3];
  };

  //
  // Four dot products (exploting horizontal sum)
  //
  [[maybe_unused]] auto dot4 = [&](const int8_t* a, const uint8_t* x, const int32_t* b, int32_t* out) {
    __i32x4 z0 = wasm_i32x4_splat(0);
    __i32x4 z1 = wasm_i32x4_splat(0);
    __i32x4 z2 = wasm_i32x4_splat(0);
    __i32x4 z3 = wasm_i32x4_splat(0);
    #if defined(USE_WASM_SIMD_POST_MVP)
      for (int j = 0; j < n; j += 16) {
        __i8x16 xv = wasm_v128_load(&x[j]);
        z0 = wasm_i32x4_add(z0, dot_i8x16(wasm_v128_load(&a[j + 0 * n]), xv));
        z1 = wasm_i32x4_add(z1, dot_i8x16(wasm_v128_load(&a[j + 1 * n]), xv));
        z2 = wasm_i32x4_add(z2, dot_i8x16(wasm_v128_load(&a[j + 2 * n]), xv));
        z3 = wasm_i32x4_add(z3, dot_i8x16(wasm_v128_load(&a[j + 3 * n]), xv));
      }
    #else
      for (int j = 0; j < n; j += 8) {
        __i16x8 xv = wasm_i16x8_load_8x8(&x[j]);
        z0 = wasm_i32x4_add(z0, dot_i16x8(wasm_i16x8_load_8x8(&a[j + 0 * n]), xv));
        z1 = wasm_i32x4_add(z1, dot_i16x8(wasm_i16x8_load_8x8(&a[j + 1 * n]), xv));
        z2 = wasm_i32x4_add(z2, dot_i16x8(wasm_i16x8_load_8x8(&a[j + 2 * n]), xv));
        z3 = wasm_i32x4_add(z3, dot_i16x8(wasm_i16x8_load_8x8(&a[j + 3 * n]), xv));
      }
    #endif
    __i32x4 z = wasm_i32x4_add(wasm_v128_load(&b[0]), haddx4(z0, z1, z2, z3));
    wasm_v128_store(&out[0], z);
  };

  //
  // Affine
  //
  if constexpr (m % 4 == 0) {
    for (int i = 0; i < m; i += 4) {
      dot4(&A[i][0], &x[0], &b[i], &y[i]);
    }

  } else {
    for (int i = 0; i < m; i++) {
      dot(&A[i][0], &x[0], &b[i], &y[i]);
    }
  }
}

}; // namespace wasm_simd
#endif


namespace no_simd {

template<int n, int m>
void affine(const int8_t A[m][n], const uint8_t x[n], const int32_t b[m], int32_t y[m]) {
  for (int i = 0; i < m; i++) {
    int32_t sum = b[i];
    for (int j = 0; j < n; j++) { sum += A[i][j] * x[j]; }
    y[i] = sum;
  }
}

}; // namespace no_simd


namespace Eval::NNUE::math {

template<int n, int m>
void affine(const int8_t A[m][n], const uint8_t x[n], const int32_t b[m], int32_t y[m]) {
  #if defined(USE_WASM_SIMD)
    wasm_simd::affine<n, m>(A, x, b, y);
  #else
    no_simd::affine<n, m>(A, x, b, y);
  #endif
}

// Explicit instantiation
template void affine<512, 32>(const int8_t A[32][512], const uint8_t x[512], const int32_t b[32], int32_t y[32]);
template void affine< 32, 32>(const int8_t A[32][ 32], const uint8_t x[ 32], const int32_t b[32], int32_t y[32]);
template void affine< 32,  1>(const int8_t A[ 1][ 32], const uint8_t x[ 32], const int32_t b[ 1], int32_t y[ 1]);

} // namespace Eval::NNUE::math
