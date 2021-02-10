#include <stdint.h>

namespace Eval::NNUE::math {

template<int n, int m>
void affine(const int8_t A[m][n], const uint8_t x[n], const int32_t b[m], int32_t y[m]);

} // namespace Eval::NNUE::math
