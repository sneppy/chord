#include "core_types.h"

#if PLATFORM_ENABLE_SIMD

#include "math/math.h"
#include "templates/simd.h"

/// @ref Simd::Vector constants
/// @{
const __m128 Simd::Vector<float32, 4>::zero{0.f, 0.f, 0.f, 0.f};
const __m128 Simd::Vector<float32, 4>::unit{1.f, 1.f, 1.f, 1.f};
const __m128 Simd::Vector<float32, 4>::neg{-0.f, -0.f, -0.f, -0.f};
const __m128 Simd::Vector<float32, 4>::rowEven{1.f, -1.f, 1.f, -1.f};
const __m128 Simd::Vector<float32, 4>::rowOdd{-1.f, 1.f, -1.f, 1.f};

const __m256 Simd::Vector<float32, 8>::zero{0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
const __m256 Simd::Vector<float32, 8>::unit{1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f};
const __m256 Simd::Vector<float32, 8>::neg{-0.f, -0.f, -0.f, -0.f, -0.f, -0.f, -0.f, -0.f};
const __m256 Simd::Vector<float32, 8>::row{1.f, -1.f, 1.f, -1.f, -1.f, 1.f, -1.f, 1.f};
/// @}

/// @ref Vec3 constants
/// @{
template<> const Vec3<float32, true> Vec3<float32, true>::zero(0.f);
template<> const Vec3<float32, true> Vec3<float32, true>::unit(1.f);
template<> const Vec3<float32, true> Vec3<float32, true>::right(1.f, 0.f, 0.f);
template<> const Vec3<float32, true> Vec3<float32, true>::left(-1.f, 0.f, 0.f);
template<> const Vec3<float32, true> Vec3<float32, true>::up(0.f, 1.f, 0.f);
template<> const Vec3<float32, true> Vec3<float32, true>::down(0.f, -1.f, 0.f);
template<> const Vec3<float32, true> Vec3<float32, true>::forward(0.f, 0.f, 1.f);
template<> const Vec3<float32, true> Vec3<float32, true>::backward(0.f, 0.f, -1.f);
/// @}

/// @ref Mat4 constants
/// @{
template<> const Mat4<float32, true> Mat4<float32, true>::zero(0.f);
template<> const Mat4<float32, true> Mat4<float32, true>::unit = Mat4<float32, true>::eye(1.f);
/// @}

#endif