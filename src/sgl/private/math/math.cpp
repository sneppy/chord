#include "math/math.h"

/// @ref Vec3 constants
/// @{
template<> const Vec3<float32, false> Vec3<float32, false>::zero(0.f);
template<> const Vec3<float32, false> Vec3<float32, false>::unit(1.f);
template<> const Vec3<float32, false> Vec3<float32, false>::right(1.f, 0.f, 0.f);
template<> const Vec3<float32, false> Vec3<float32, false>::left(-1.f, 0.f, 0.f);
template<> const Vec3<float32, false> Vec3<float32, false>::up(0.f, 1.f, 0.f);
template<> const Vec3<float32, false> Vec3<float32, false>::down(0.f, -1.f, 0.f);
template<> const Vec3<float32, false> Vec3<float32, false>::forward(0.f, 0.f, 1.f);
template<> const Vec3<float32, false> Vec3<float32, false>::backward(0.f, 0.f, -1.f);
/// @}