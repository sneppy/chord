#pragma once

#include "mat4.h"

/**
 * Vector intrinsics specialization
 */
template<typename T>
struct GCC_ALIGN(32) Mat4<T, true>
{
public:
	/// Constants
	/// @{
	static const Mat4<T, true> zero;
	static const Mat4<T, true> unit;
	/// @}

public:
	/// Vector intrinsics operation class
	using VecOps = Simd::Vector<T, 4>;
	using DVecOps = Simd::Vector<T, 8>;

	/// Vector intrinsics data type
	using VecT = typename VecOps::Type;
	using DVecT = typename DVecOps::Type;

	union
	{
		/// Intrinsics data
		/// @{
		VecT	vec[4];
		DVecT	dvec[2];
		/// @}

		/// Matrix buffer
		T matrix[4][4];

		/// Linear buffer
		T array[16];

		struct
		{
			/// Single components
			/// @todo Maybe unnecessary
			/// @{
			T _a, _b, _c, _d, _e, _f, _g, _h, _i, _l, _m, _n, _o;
			/// @}
		};
	};

public:
	/// Default constructor
	FORCE_INLINE Mat4() {}

	/// Vectors constructor
	FORCE_INLINE Mat4(VecT v0, VecT v1, VecT v2, VecT v3) : vec{v0, v1, v2, v3} {};

	/// Double vectors constructor
	FORCE_INLINE Mat4(DVecT upper, DVecT lower) : dvec{upper, lower} {};

	/// Elements constructor
	FORCE_INLINE Mat4(
		T __a, T __b, T __c, T __d,
		T __e, T __f, T __g, T __h,
		T __i, T __j, T __k, T __l,
		T __m, T __n, T __o, T __p
	) : array{
		__a, __b, __c, __d,
		__e, __f, __g, __h,
		__i, __j, __k, __l,
		__m, __n, __o, __p
	} {}

	/// Scalar constructor, fill matrix
	FORCE_INLINE Mat4(T s) : array{
		s, s, s, s,
		s, s, s, s,
		s, s, s, s,
		s, s, s, s
	} {}

	/// Array random-access operator
	/// @{
	FORCE_INLINE const T & operator[](uint8 i) const { return array[i]; }
	FORCE_INLINE T & operator[](uint8 i) { return array[i]; }
	/// @}

	/// Matrix random-access operator
	/// @{
	FORCE_INLINE const T & operator()(uint8 i, uint8 j) const { return matrix[i][j]; }
	FORCE_INLINE T & operator()(uint8 i, uint8 j) { return matrix[i][j]; }
	/// @}

	/**
	 * Comparison operations
	 * 
	 * @param [in] m matrix operand
	 * @return comparison result
	 * @{
	 */
	FORCE_INLINE bool operator==(const Mat4<T> & m) const
	{
		return
			DVecOps::template cmp<Simd::CMP_EQ>(dvec[0], m.dvec[0]) == 0xff &
			DVecOps::template cmp<Simd::CMP_EQ>(dvec[1], m.dvec[1]) == 0xff;
	}
	FORCE_INLINE bool operator!=(const Mat4<T> & m) const
	{
		return
			DVecOps::template cmp<Simd::CMP_NE>(dvec[0], m.dvec[0]) +
			DVecOps::template cmp<Simd::CMP_NE>(dvec[1], m.dvec[1]) != 0x0;
	}
	/// @}

	/**
	 * Matrix-scalar vector operations
	 * 
	 * @param [in] s scalar value
	 * @return new matrix
	 * @{
	 */
	FORCE_INLINE Mat4<T> operator+(T s) const
	{
		DVecT ss = DVecOps::load(s);
		return Mat4<T>(
			DVecOps::add(dvec[0], ss),
			DVecOps::add(dvec[1], ss)
		);
	}
	FORCE_INLINE Mat4<T> operator-(T s) const
	{
		DVecT ss = DVecOps::load(s);
		return Mat4<T>(
			DVecOps::sub(dvec[0], ss),
			DVecOps::sub(dvec[1], ss)
		);
	}
	FORCE_INLINE Mat4<T> operator*(T s) const
	{
		DVecT ss = DVecOps::load(s);
		return Mat4<T>(
			DVecOps::mul(dvec[0], ss),
			DVecOps::mul(dvec[1], ss)
		);
	}
	FORCE_INLINE Mat4<T> operator/(T s) const
	{
		DVecT ss = DVecOps::load(s);
		return Mat4<T>(
			DVecOps::div(dvec[0], ss),
			DVecOps::div(dvec[1], ss)
		);
	}
	/// @}

	/// Returns transposed matrix
	FORCE_INLINE Mat4<T> getTranspose() const
	{
		VecT
			a0 = VecOps::unpacklo(vec[0], vec[1]),
			a1 = VecOps::unpackhi(vec[0], vec[1]),
			a2 = VecOps::unpacklo(vec[2], vec[3]),
			a3 = VecOps::unpackhi(vec[2], vec[3]);
		
		VecT
			t0 = VecOps::template shuffle<0, 1, 0, 1>(a0, a2),
			t1 = VecOps::template shuffle<2, 3, 2, 3>(a0, a2),
			t2 = VecOps::template shuffle<0, 1, 0, 1>(a1, a3),
			t3 = VecOps::template shuffle<2, 3, 2, 3>(a1, a3);
		
		return Mat4<T>(
			VecOps::template shuffle<0, 1, 0, 1>(a0, a2),
			VecOps::template shuffle<2, 3, 2, 3>(a0, a2),
			VecOps::template shuffle<0, 1, 0, 1>(a1, a3),
			VecOps::template shuffle<2, 3, 2, 3>(a1, a3)
		);
	}

	/// Transposes matrix in place
	FORCE_INLINE Mat4<T> & transpose()
	{
		VecT
			a0 = VecOps::unpacklo(vec[0], vec[1]),
			a1 = VecOps::unpackhi(vec[0], vec[1]),
			a2 = VecOps::unpacklo(vec[2], vec[3]),
			a3 = VecOps::unpackhi(vec[2], vec[3]);
		
		vec[0] = VecOps::template shuffle<0, 1, 0, 1>(a0, a2),
		vec[1] = VecOps::template shuffle<2, 3, 2, 3>(a0, a2),
		vec[2] = VecOps::template shuffle<0, 1, 0, 1>(a1, a3),
		vec[3] = VecOps::template shuffle<2, 3, 2, 3>(a1, a3);

		// Voila!
		return *this;
	}

protected:
	/// Transposed dot product
	/// Second matrix is transposed w.r.t to the original matrix
	Mat4<T> multiplyTransposed(const Mat4<T> & m) const
	{
		/// With loop unrolling is slightly more efficient
		/// I'll leave this here commented in case
		/// I'll need it
		/* for (uint8 i = 0; i < 4; ++i)
		{
			VecT
				r0 = VecOps::mul(vec[0], m.vec[0]),
				r1 = VecOps::mul(vec[0], m.vec[1]),
				r2 = VecOps::mul(vec[0], m.vec[2]),
				r3 = VecOps::mul(vec[0], m.vec[3]);
			
			r0 = VecOps::hadd(r0, r1),
			r2 = VecOps::hadd(r2, r3);

			VecOps::hadd(r0, r2);
		} */

		VecT
			a0 = VecOps::mul(vec[0], m.vec[0]),
			a1 = VecOps::mul(vec[0], m.vec[1]),
			a2 = VecOps::mul(vec[0], m.vec[2]),
			a3 = VecOps::mul(vec[0], m.vec[3]),

			b0 = VecOps::mul(vec[1], m.vec[0]),
			b1 = VecOps::mul(vec[1], m.vec[1]),
			b2 = VecOps::mul(vec[1], m.vec[2]),
			b3 = VecOps::mul(vec[1], m.vec[3]),

			c0 = VecOps::mul(vec[2], m.vec[0]),
			c1 = VecOps::mul(vec[2], m.vec[1]),
			c2 = VecOps::mul(vec[2], m.vec[2]),
			c3 = VecOps::mul(vec[2], m.vec[3]),

			d0 = VecOps::mul(vec[3], m.vec[0]),
			d1 = VecOps::mul(vec[3], m.vec[1]),
			d2 = VecOps::mul(vec[3], m.vec[2]),
			d3 = VecOps::mul(vec[3], m.vec[3]);
		
		a0 = VecOps::hadd(a0, a1),
		a2 = VecOps::hadd(a2, a3),

		b0 = VecOps::hadd(b0, b1),
		b2 = VecOps::hadd(b2, b3),

		c0 = VecOps::hadd(c0, c1),
		c2 = VecOps::hadd(c2, c3),

		d0 = VecOps::hadd(d0, d1),
		d2 = VecOps::hadd(d2, d3);

		return Mat4<T>(
			VecOps::hadd(a0, a2),
			VecOps::hadd(b0, b2),
			VecOps::hadd(c0, c2),
			VecOps::hadd(d0, d2)
		);
	}

public:
	/// Matrix-matrix dot product (multiplication)
	Mat4<T> operator*(const Mat4<T> & m) const
	{
		// Transpose matrix for column access
		return multiplyTransposed(m.getTranspose());
	}

	/// Dot product compound assignment
	Mat4<T> & operator*=(const Mat4<T> & m)
	{
		// Transpose matrix for column access
		*this = multiplyTransposed(m,getTranspose());
	}

	/**
	 * Matrix-vector dot product
	 * 
	 * @param [in] v vector operand
	 * @return transformed vector
	 * @{
	 */
	FORCE_INLINE Vec4<T, true> operator*(const Vec4<T, true> & v) const
	{
		VecT
			x = VecOps::mul(vec[0], v.data),
			y = VecOps::mul(vec[1], v.data),
			z = VecOps::mul(vec[2], v.data),
			w = VecOps::mul(vec[3], v.data);

		return Vec4<T, true>(VecOps::hadd(
			VecOps::hadd(x, y),
			VecOps::hadd(z, w)
		));
	}
	FORCE_INLINE Vec3<T, true> operator*(const Vec3<T, true> & v) const { return (Vec3<T, true>)operator*(Vec4<T, true>(v)); }
	/// @}

protected:
	#define A dvec[0]
	#define B dvec[1]

	/// Compute upper part of the algebraic complements matrix
	FORCE_INLINE DVecT getUpperAlgebraicComplements() const
	{
		const DVecT
			a0 = DVecOps::shuffle(A, 5, 4, 4, 4, 1, 0, 0, 0),
			a1 = DVecOps::shuffle(A, 6, 6, 5, 5, 2, 2, 1, 1),
			a2 = DVecOps::shuffle(A, 7, 7, 7, 6, 3, 3, 3, 2),

			b0 = DVecOps::shuffle(B, 2, 2, 1, 1, 2, 2, 1, 1),
			b1 = DVecOps::shuffle(B, 7, 7, 7, 6, 7, 7, 7, 6),
			b2 = DVecOps::shuffle(B, 3, 3, 3, 2, 3, 3, 3, 2),
			b3 = DVecOps::shuffle(B, 6, 6, 5, 5, 6, 6, 5, 5),
			b4 = DVecOps::shuffle(B, 1, 0, 0, 0, 1, 0, 0, 0),
			b5 = DVecOps::shuffle(B, 5, 4, 4, 4, 5, 4, 4, 4);

		return DVecOps::mul(
			DVecOps::row,
			DVecOps::add(
				DVecOps::sub(
					DVecOps::mul(
						a0,
						DVecOps::sub(
							DVecOps::mul(
								b0,
								b1
							),
							DVecOps::mul(
								b2,
								b3
							)
						)
					),
					DVecOps::mul(
						a1,
						DVecOps::sub(
							DVecOps::mul(
								b4,
								b1
							),
							DVecOps::mul(
								b2,
								b5
							)
						)
					)
				),
				DVecOps::mul(
					a2,
					DVecOps::sub(
						DVecOps::mul(
							b4,
							b3
						),
						DVecOps::mul(
							b0,
							b5
						)
					)
				)
			)
		);
	}

	/// Compute lower part of the algebraic complements matrix
	FORCE_INLINE DVecT getLowerAlgebraicComplements() const
	{
		const DVecT
			b0 = DVecOps::shuffle(B, 5, 4, 4, 4, 1, 0, 0, 0),
			b1 = DVecOps::shuffle(B, 6, 6, 5, 5, 2, 2, 1, 1),
			b2 = DVecOps::shuffle(B, 7, 7, 7, 6, 3, 3, 3, 2),

			a0 = DVecOps::shuffle(A, 2, 2, 1, 1, 2, 2, 1, 1),
			a1 = DVecOps::shuffle(A, 7, 7, 7, 6, 7, 7, 7, 6),
			a2 = DVecOps::shuffle(A, 3, 3, 3, 2, 3, 3, 3, 2),
			a3 = DVecOps::shuffle(A, 6, 6, 5, 5, 6, 6, 5, 5),
			a4 = DVecOps::shuffle(A, 1, 0, 0, 0, 1, 0, 0, 0),
			a5 = DVecOps::shuffle(A, 5, 4, 4, 4, 5, 4, 4, 4);
		
		return DVecOps::mul(
			DVecOps::load(T(1), T(-1), T(1), T(-1), T(-1), T(1), T(-1), T(1)),
			DVecOps::add(
				DVecOps::sub(
					DVecOps::mul(
						b0,
						DVecOps::sub(
							DVecOps::mul(
								a0,
								a1
							),
							DVecOps::mul(
								a2,
								a3
							)
						)
					),
					DVecOps::mul(
						b1,
						DVecOps::sub(
							DVecOps::mul(
								a4,
								a1
							),
							DVecOps::mul(
								a2,
								a5
							)
						)
					)
				),
				DVecOps::mul(
					b2,
					DVecOps::sub(
						DVecOps::mul(
							a4,
							a3
						),
						DVecOps::mul(
							a0,
							a5
						)
					)
				)
			)
		);
	}

	#undef A
	#undef B

public:
	/// Calculate matrix of algebraic complements
	Mat4<T> getAlgebraicComplementsMatrix() const
	{
		return Mat4<T>(
			/// Upper vector
			getUpperAlgebraicComplements(),
			/// Lower vector
			getLowerAlgebraicComplements()
		);
	}

	/// Get matrix inverse
	Mat4<T> operator!() const
	{
		// Get complements matrix
		Mat4<T> c = getAlgebraicComplementsMatrix();

		// Compute determinant
		const T det = array[0] * c[0] + array[1] * c[1] + array[2] * c[2] + array[3] * c[3];

		if (LIKELY(det != T(0)))
		{
			c.dvec[0] = DVecOps::div(c.dvec[0], DVecOps::load(det)),
			c.dvec[1] = DVecOps::div(c.dvec[1], DVecOps::load(det));
			return c.transpose();
		}
		else
			return Mat4<T>(T(0));
	}

	/// Invert matrix in place
	Mat4<T> & invert()
	{
		// Get complements matrix
		Mat4<T> c = getAlgebraicComplementsMatrix();

		// Compute determinant
		const T det = array[0] * c[0] + array[1] * c[1] + array[2] * c[2] + array[3] * c[3];

		if (LIKELY(det != T(0)))
		{
			dvec[0] = DVecOps::div(c.dvec[0], DVecOps::load(det)),
			dvec[1] = DVecOps::div(c.dvec[1], DVecOps::load(det));
			transpose();
		}

		return *this;
	}

	/// Inverse optimized for transform matrix
	FORCE_INLINE Mat4<T> getInverseTransform() const
	{
		// First, transpose
		Mat4<T> mt = getTranspose();

		VecT
			s0 = VecOps::mul(mt.vec[0], mt.vec[0]),
			s1 = VecOps::mul(mt.vec[1], mt.vec[1]),
			s2 = VecOps::mul(mt.vec[2], mt.vec[2]),

			t0 = VecOps::mul(mt.vec[0], mt.vec[3]),
			t1 = VecOps::mul(mt.vec[1], mt.vec[3]),
			t2 = VecOps::mul(mt.vec[2], mt.vec[3]);
		
		s0 = VecOps::hadd(s0, s1),
		s2 = VecOps::hadd(s2, VecOps::zero);

		t0 = VecOps::hadd(t0, t1),
		t2 = VecOps::hadd(t2, VecOps::zero);

		s0 = VecOps::hadd(s0, s2);
		t0 = VecOps::hadd(t0, t2);

		// Set last row
		mt.vec[3] = VecOps::load(T(0), T(0), T(0), T(1));

		// Set translation
		mt.vec[0] = VecOps::add(mt.vec[0], VecOps::template shuffle<3, 3, 3, 0>(t0));
		mt.vec[1] = VecOps::add(mt.vec[1], VecOps::template shuffle<3, 3, 3, 1>(t0));
		mt.vec[2] = VecOps::add(mt.vec[2], VecOps::template shuffle<3, 3, 3, 2>(t0));

		// Scale matrix
		mt.vec[0] = VecOps::div(mt.vec[0], VecOps::template shuffle<0, 0, 0, 0>(s0)),
		mt.vec[1] = VecOps::div(mt.vec[1], VecOps::template shuffle<1, 1, 1, 1>(s0)),
		mt.vec[2] = VecOps::div(mt.vec[2], VecOps::template shuffle<2, 2, 2, 2>(s0));

		return mt;
	}

	/// Print matrix to stdout or specified fd
	FORCE_INLINE void print(FILE * out = stdout) const;

	//////////////////////////////////////////////////
	// Constructors
	//////////////////////////////////////////////////
	
	/// Create a diag matrix from single scalar value
	static CONSTEXPR FORCE_INLINE Mat4<T> eye(T s)
	{
		return Mat4<float32, true>(
			s, T(0), T(0), T(0),
			T(0), s, T(0), T(0),
			T(0), T(0), s, T(0),
			T(0), T(0), T(0), s
		);
	}

	/// Create a diag matrix with specified diagonal
	/// @{
						static CONSTEXPR FORCE_INLINE Mat4<T> diag(T a, T b, T c, T d)
						{
							return Mat4<T>(
								a, T(0), T(0), T(0),
								T(0), b, T(0), T(0),
								T(0), T(0), c, T(0),
								T(0), T(0), T(0), d
							);
						}
	template<bool bHVI>	static CONSTEXPR FORCE_INLINE Mat4<T> diag(const Vec4<T, bHVI> & v) { return diag(v.x, v.y, v.z, v.w); }
	/// @}

	/// Create a transform matrix with scaling
	/// @{
						static FORCE_INLINE Mat4<T> scaling(T x, T y, T z)
						{
							return Mat4<T>(
								x, T(0), T(0), T(0),
								T(0), y, T(0), T(0),
								T(0), T(0), z, T(0),
								T(0), T(0), T(0), T(1)
							);
						}
	template<bool bHVI>	static CONSTEXPR FORCE_INLINE Mat4<T> scaling(const Vec3<T, bHVI> & v) { return scaling(v.x, v.y, v.z); }
	/// @}

	/// Create a transform matrix with translation
	/// @{
						static CONSTEXPR FORCE_INLINE Mat4<T> translation(T x, T y, T z)
						{
							return Mat4<T>(
									T(1),  T(0),  T(0), x,
									T(0),  T(1),  T(0), y,
									T(0),  T(0),  T(1), z,
									T(0),  T(0),  T(0),  T(1)
							);
						}
	template<bool bHVI>	static CONSTEXPR FORCE_INLINE Mat4<T> translation(const Vec3<T, bHVI> & v) { return translation(v.x, v.y, v.z); }
	/// @}

	/// Create a transform matrix with rotation
	/// @{
	template<bool bHVI>	static FORCE_INLINE Mat4<T> rotation(const Quat<T, bHVI> & q)
						{
							/// @ref http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm
							return Mat4<T>(
								q.w, -q.z, q.y, q.x,
								q.z, q.w, -q.x, q.y,
								-q.y, q.x, q.w, q.z,
								-q.x, -q.y, -q.z, q.w
							).multiplyTransposed(Mat4<T>(
								q.w, q.z, -q.y, q.x,
								-q.z, q.w, q.x, q.y,
								q.y, -q.x, q.w, q.z,
								-q.x, -q.y, -q.z, q.w
							));
						}
	template<bool bHVI>	static FORCE_INLINE Mat4<T> rotation(T angle, const Vec3<T, bHVI> & axis) { return rotation(Quat<T, bHVI>(angle, axis)); }
	/// @}

	/// Create a complete transform matrix
	static FORCE_INLINE Mat4<T> transform(const Vec3<T> & t, const Quat<T> & r, const Vec3<T> & s = Vec3<T>::unit)
	{
		// Set rotation
		Mat4<T> out = Mat4<T>(
			r.w, r.z, -r.y, r.x,
			-r.z, r.w, r.x, r.y,
			r.y, -r.x, r.w, r.z,
			-r.x, -r.y, -r.z, r.w
		).multiplyTransposed(Mat4<T>(
			r.w, -r.z, r.y, r.x,
			r.z, r.w, -r.x, r.y,
			-r.y, r.x, r.w, r.z,
			-r.x, -r.y, -r.z, r.w
		));

		// Scale rotation
		out.vec[0] = VecOps::mul(out.vec[0], VecOps::load(s.x)),
		out.vec[1] = VecOps::mul(out.vec[1], VecOps::load(s.y)),
		out.vec[2] = VecOps::mul(out.vec[2], VecOps::load(s.z));

		// Set translation
		out.vec[3] = t.data;
		
		// Transpose
		out.transpose();
		out(3, 3) = 1.f;

		return out;
	}

	/// Create a projection matrix for OpenGL
	/// 
	/// X-right, Y-up, Z-forward (left-handed coordinate system)
	/// @{
	/**
	 * @param [in] r,l,t,b,f,n extent of projection box
	 * @return projection matrix
	 */
	static CONSTEXPR FORCE_INLINE Mat4<T> glProjection(T r, T l, T t, T b, T f, T n);
	/**
	 * @param [in] fov camera field of view
	 * @param [in] near,far distance of near and far planes
	 * @return projection matrix
	 */
	static CONSTEXPR FORCE_INLINE Mat4<T> glProjection(T fov/* = PlatformMath::PI */, T n = T(1), T f = T(1000));
	/// @}
};

template<>
FORCE_INLINE void Mat4<float32, true>::print(FILE * out) const
{
	fprintf(out, "m4f (%.3f, %.3f, %.3f, %.3f)\n", array[0x0], array[0x1], array[0x2], array[0x3]);
	fprintf(out, "    (%.3f, %.3f, %.3f, %.3f)\n", array[0x4], array[0x5], array[0x6], array[0x7]);
	fprintf(out, "    (%.3f, %.3f, %.3f, %.3f)\n", array[0x8], array[0x9], array[0xa], array[0xb]);
	fprintf(out, "    (%.3f, %.3f, %.3f, %.3f)\n", array[0xc], array[0xd], array[0xe], array[0xf]);
}

//////////////////////////////////////////////////
// Constructors
//////////////////////////////////////////////////

template<>
FORCE_INLINE Mat4<float32, true> Mat4<float32, true>::glProjection(
	float32 r, float32 l,
	float32 t, float32 b,
	float32 n, float32 f
)
{
	/// @see http://www.songho.ca/opengl/gl_projectionmatrix.html
	return Mat4<float32>(
		(2.f * n) / (r - l), 0.f, -(r + l) / (r - l), 0.f,
		0.f, (2.f * n) / (t - b), -(t + b) / (t - b), 0.f,
		0.f, 0.f, (n + f) / (n - f), (2.f * n * f) / (n - f),
		0.f, 0.f, 1.f, 0.f
	);
}

template<>
FORCE_INLINE Mat4<float32, true> Mat4<float32, true>::glProjection(float32 fov, float32 n, float32 f)
{
	// Default aspect ratio
	float32 aspect = 16.f / 9.f;
	fov = 1.f / PlatformMath::tan(fov / 2.f);
	/// @todo Get framebuffer size if possible

	return Mat4<float32>(
		fov, 0.f, 0.f, 0.f,
		0.f, fov * aspect, 0.f, 0.f,
		0.f, 0.f, (f + n) / (f - n), (2.f * n * f) / (n - f),
		0.f, 0.f, 1.f, 0.f
	);
}