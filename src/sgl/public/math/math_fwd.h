#pragma once

#include "core_types.h"
#include "templates/is_void.h"
#include "templates/simd.h"

//////////////////////////////////////////////////
// Forward declarations
//////////////////////////////////////////////////

template<typename>			struct Vec2;
template<typename, bool>	struct Vec3;
template<typename, bool>	struct Vec4;
template<typename, bool>	struct Quat;
template<typename, bool>	struct Mat3;
template<typename, bool>	struct Mat4;
template<typename>			struct Box;

//////////////////////////////////////////////////
// Typedefs for common types
//////////////////////////////////////////////////

using vec2 = Vec2<float32>;
using vec3 = Vec3<float32, hasVectorIntrinsics(float32, 4)>;
using vec4 = Vec4<float32, hasVectorIntrinsics(float32, 4)>;
using mat3 = Mat3<float32, hasVectorIntrinsics(float32, 4)>;
using quat = Quat<float32, hasVectorIntrinsics(float32, 4)>;
using mat4 = Mat4<float32, hasVectorIntrinsics(float32, 4) & hasVectorIntrinsics(float32, 8)>;

using ivec2 = Vec2<int32>;
using ivec3 = Vec3<int32, hasVectorIntrinsics(int32, 4)>;
using ivec4 = Vec4<int32, hasVectorIntrinsics(int32, 4)>;
using imat3 = Mat3<int32, hasVectorIntrinsics(int32, 4)>;
using imat4 = Mat4<int32, hasVectorIntrinsics(int32, 4) & hasVectorIntrinsics(int32, 8)>;

using uvec2 = Vec2<uint32>;
using uvec3 = Vec3<uint32, hasVectorIntrinsics(uint32, 4)>;
using uvec4 = Vec4<uint32, hasVectorIntrinsics(uint32, 4)>;
using umat3 = Mat3<uint32, hasVectorIntrinsics(uint32, 4)>;
using umat4 = Mat4<uint32, hasVectorIntrinsics(uint32, 4) & hasVectorIntrinsics(uint32, 8)>;

using point2 = ivec2;
using point3 = ivec3;
using point4 = ivec4;

using dim2 = uvec2;
using dim3 = uvec3;
using dim4 = uvec4;