#pragma once

//welcome to the header file I stole from the TGE sdk. It's just a dump of the classes I needed so don't be surprised if 50% of the stuff doesn't work.

#ifndef _MPOINT2_H_
#define _MPOINT2_H_

inline int getMax(int a, int b)
{
	return a>b ? a : b;
}

inline int getMin(int a, int b)
{
	return a<b ? a : b;
}

class ColorF
{
public:
	float red;
	float green;
	float blue;
	float alpha;
	ColorF(float r, float g, float b, float a);
};

ColorF::ColorF(const float r, const float g, const float b, const float a)
{
	red = r;
	green = g;
	blue = b;
	alpha = a;
}

class ColorI
{
public:
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	unsigned char alpha;

public:
	ColorI() { }
	ColorI(const ColorI& in_rCopy);
	ColorI(const unsigned char in_r,
		const unsigned char in_g,
		const unsigned char in_b,
		const unsigned char in_a = unsigned char(255));
	ColorI(const ColorI& in_rCopy, const unsigned char in_a);

	void set(const unsigned char in_r,
		const unsigned char in_g,
		const unsigned char in_b,
		const unsigned char in_a = unsigned char(255));

	void set(const ColorI& in_rCopy,
		const unsigned char in_a);

	ColorI& operator*=(const float in_mul);
	ColorI  operator*(const float in_mul) const;

	ColorI  operator+(const ColorI& in_rAdd) const;
	ColorI&  operator+=(const ColorI& in_rAdd);

	ColorI& operator*=(const int in_mul);
	ColorI& operator/=(const int in_mul);
	ColorI  operator*(const int in_mul) const;
	ColorI  operator/(const int in_mul) const;

	bool operator==(const ColorI&) const;
	bool operator!=(const ColorI&) const;

	void interpolate(const ColorI& in_rC1,
		const ColorI& in_rC2,
		const float  in_factor);

	unsigned int getARGBPack() const;
	unsigned int getRGBAPack() const;
	unsigned int getABGRPack() const;

	unsigned int getBGRPack() const;
	unsigned int getRGBPack() const;

	unsigned int getRGBEndian() const;
	unsigned int getARGBEndian() const;

	unsigned short get565()  const;
	unsigned short get4444() const;

	operator const unsigned char*() const { return &red; }

	static const ColorI ZERO;
	static const ColorI ONE;
	static const ColorI WHITE;
	static const ColorI BLACK;
	static const ColorI RED;
	static const ColorI GREEN;
	static const ColorI BLUE;
};

inline void ColorI::set(const unsigned char in_r,
	const unsigned char in_g,
	const unsigned char in_b,
	const unsigned char in_a)
{
	red = in_r;
	green = in_g;
	blue = in_b;
	alpha = in_a;
}

inline void ColorI::set(const ColorI& in_rCopy,
	const unsigned char in_a)
{
	red = in_rCopy.red;
	green = in_rCopy.green;
	blue = in_rCopy.blue;
	alpha = in_a;
}

inline ColorI::ColorI(const ColorI& in_rCopy)
{
	red = in_rCopy.red;
	green = in_rCopy.green;
	blue = in_rCopy.blue;
	alpha = in_rCopy.alpha;
}

inline ColorI::ColorI(const unsigned char in_r,
	const unsigned char in_g,
	const unsigned char in_b,
	const unsigned char in_a)
{
	set(in_r, in_g, in_b, in_a);
}

inline ColorI::ColorI(const ColorI& in_rCopy,
	const unsigned char in_a)
{
	set(in_rCopy, in_a);
}

inline ColorI& ColorI::operator*=(const float in_mul)
{
	red = unsigned char((float(red)   * in_mul) + 0.5f);
	green = unsigned char((float(green) * in_mul) + 0.5f);
	blue = unsigned char((float(blue)  * in_mul) + 0.5f);
	alpha = unsigned char((float(alpha) * in_mul) + 0.5f);

	return *this;
}

inline ColorI& ColorI::operator*=(const int in_mul)
{
	red = red    * in_mul;
	green = green  * in_mul;
	blue = blue   * in_mul;
	alpha = alpha  * in_mul;

	return *this;
}

inline ColorI& ColorI::operator/=(const int in_mul)
{
	red = red / in_mul;
	green = green / in_mul;
	blue = blue / in_mul;
	alpha = alpha / in_mul;

	return *this;
}

inline ColorI ColorI::operator+(const ColorI &in_add) const
{
	ColorI tmp;

	tmp.red = red + in_add.red;
	tmp.green = green + in_add.green;
	tmp.blue = blue + in_add.blue;
	tmp.alpha = alpha + in_add.alpha;

	return tmp;
}

inline ColorI ColorI::operator*(const float in_mul) const
{
	ColorI temp(*this);
	temp *= in_mul;
	return temp;
}

inline ColorI ColorI::operator*(const int in_mul) const
{
	ColorI temp(*this);
	temp *= in_mul;
	return temp;
}

inline ColorI ColorI::operator/(const int in_mul) const
{
	ColorI temp(*this);
	temp /= in_mul;
	return temp;
}

inline bool ColorI::operator==(const ColorI& in_Cmp) const
{
	return (red == in_Cmp.red && green == in_Cmp.green && blue == in_Cmp.blue && alpha == in_Cmp.alpha);
}

inline bool ColorI::operator!=(const ColorI& in_Cmp) const
{
	return (red != in_Cmp.red || green != in_Cmp.green || blue != in_Cmp.blue || alpha != in_Cmp.alpha);
}

inline ColorI& ColorI::operator+=(const ColorI& in_rAdd)
{
	red += in_rAdd.red;
	green += in_rAdd.green;
	blue += in_rAdd.blue;
	alpha += in_rAdd.alpha;

	return *this;
}

inline void ColorI::interpolate(const ColorI& in_rC1,
	const ColorI& in_rC2,
	const float  in_factor)
{
	float f2 = 1.0f - in_factor;
	red = unsigned char(((float(in_rC1.red)   * f2) + (float(in_rC2.red)   * in_factor)) + 0.5f);
	green = unsigned char(((float(in_rC1.green) * f2) + (float(in_rC2.green) * in_factor)) + 0.5f);
	blue = unsigned char(((float(in_rC1.blue)  * f2) + (float(in_rC2.blue)  * in_factor)) + 0.5f);
	alpha = unsigned char(((float(in_rC1.alpha) * f2) + (float(in_rC2.alpha) * in_factor)) + 0.5f);
}

inline unsigned int ColorI::getARGBPack() const
{
	return (unsigned int(alpha) << 24) |
		(unsigned int(red) << 16) |
		(unsigned int(green) << 8) |
		(unsigned int(blue) << 0);
}

inline unsigned int ColorI::getRGBAPack() const
{
	return (unsigned int(red) << 0) |
		(unsigned int(green) << 8) |
		(unsigned int(blue) << 16) |
		(unsigned int(alpha) << 24);
}

inline unsigned int ColorI::getABGRPack() const
{
	return (unsigned int(alpha) << 24) |
		(unsigned int(blue) << 16) |
		(unsigned int(green) << 8) |
		(unsigned int(red) << 0);
}


inline unsigned int ColorI::getBGRPack() const
{
	return (unsigned int(blue) << 16) |
		(unsigned int(green) << 8) |
		(unsigned int(red) << 0);
}

inline unsigned int ColorI::getRGBPack() const
{
	return (unsigned int(red) << 16) |
		(unsigned int(green) << 8) |
		(unsigned int(blue) << 0);
}

inline unsigned int ColorI::getRGBEndian() const
{
#if defined(TORQUE_BIG_ENDIAN)
	return(getRGBPack());
#else
	return(getBGRPack());
#endif
}

inline unsigned int ColorI::getARGBEndian() const
{
#if defined(TORQUE_BIG_ENDIAN)
	return(getABGRPack());
#else
	return(getARGBPack());
#endif
}

inline unsigned short ColorI::get565() const
{
	return unsigned short((unsigned short(red >> 3) << 11) |
		(unsigned short(green >> 2) << 5) |
		(unsigned short(blue >> 3) << 0));
}

inline unsigned short ColorI::get4444() const
{
	return unsigned short(unsigned short(unsigned short(alpha >> 4) << 12) |
		unsigned short(unsigned short(red >> 4) << 8) |
		unsigned short(unsigned short(green >> 4) << 4) |
		unsigned short(unsigned short(blue >> 4) << 0));
}



class Point3F
{
	//-------------------------------------- Public data
public:
	float x;
	float y;
	float z;

public:
	Point3F();
	Point3F(const Point3F&);
	Point3F(float _x, float _y, float _z);
	explicit Point3F(float xyz);

	//-------------------------------------- Non-math mutators and misc functions
public:
	void set(float xyz);
	void set(float _x, float _y, float _z);
	void set(const Point3F&);

	void setMin(const Point3F&);
	void setMax(const Point3F&);

	void interpolate(const Point3F&, const Point3F&, float);
	void zero();

	/// Returns the smallest absolute value.
	float least() const;

	/// Returns the greatest absolute value.
	float most() const;

	operator float*() { return &x; }
	operator const float*() const { return &x; }

	//-------------------------------------- Queries
public:
	bool  isZero() const;
	bool  isUnitLength() const;
	float   len()    const;
	float   lenSquared() const;
	float   magnitudeSafe() const;
	int   getLeastComponentIndex() const;
	int   getGreatestComponentIndex() const;

	//-------------------------------------- Mathematical mutators
public:
	void neg();
	void normalize();
	void normalizeSafe();
	void normalize(float val);
	void convolve(const Point3F&);
	void convolveInverse(const Point3F&);

	//-------------------------------------- Overloaded operators
public:
	// Comparison operators
	bool operator==(const Point3F&) const;
	bool operator!=(const Point3F&) const;

	// Arithmetic w/ other points
	Point3F  operator+(const Point3F&) const;
	Point3F  operator-(const Point3F&) const;
	Point3F& operator+=(const Point3F&);
	Point3F& operator-=(const Point3F&);

	// Arithmetic w/ scalars
	Point3F  operator*(float) const;
	Point3F  operator/(float) const;
	Point3F& operator*=(float);
	Point3F& operator/=(float);

	Point3F  operator*(const Point3F&) const;
	Point3F& operator*=(const Point3F&);
	Point3F  operator/(const Point3F&) const;
	Point3F& operator/=(const Point3F&);

	// Unary operators
	Point3F operator-() const;

	//-------------------------------------- Public static constants
public:
	const static Point3F One;
	const static Point3F Zero;
	const static Point3F Max;
	const static Point3F Min;
	const static Point3F UnitX;
	const static Point3F UnitY;
	const static Point3F UnitZ;
};

inline Point3F::Point3F()
#if defined(TORQUE_OS_LINUX)
: x(0.f), y(0.f), z(0.f)
#endif
{
	// Uninitialized points are definitely a problem.
	// Enable the following code to see how often they crop up.
#ifdef DEBUG_MATH
	*(U32 *)&x = 0x7FFFFFFA;
	*(U32 *)&y = 0x7FFFFFFB;
	*(U32 *)&z = 0x7FFFFFFC;
#endif
}

inline float mDot(const Point3F &p1, const Point3F &p2)
{
	return (p1.x*p2.x + p1.y*p2.y + p1.z*p2.z);
}

inline Point3F::Point3F(const Point3F& _copy)
: x(_copy.x), y(_copy.y), z(_copy.z)
{
	//
}

inline Point3F::Point3F(float _x, float _y, float _z)
: x(_x), y(_y), z(_z)
{
	//
}

inline Point3F::Point3F(float xyz)
: x(xyz), y(xyz), z(xyz)
{
	//
}

inline void Point3F::set(float xyz)
{
	x = y = z = xyz;
}

inline void Point3F::set(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

inline void Point3F::set(const Point3F& copy)
{
	x = copy.x;
	y = copy.y;
	z = copy.z;
}

inline void Point3F::setMin(const Point3F& _test)
{
	x = (_test.x < x) ? _test.x : x;
	y = (_test.y < y) ? _test.y : y;
	z = (_test.z < z) ? _test.z : z;
}

inline void Point3F::setMax(const Point3F& _test)
{
	x = (_test.x > x) ? _test.x : x;
	y = (_test.y > y) ? _test.y : y;
	z = (_test.z > z) ? _test.z : z;
}

inline void Point3F::zero()
{
	x = y = z = 0.0f;
}

inline bool Point3F::isZero() const
{
	return false;
}

inline bool Point3F::isUnitLength() const
{
	return false;
}

inline int Point3F::getLeastComponentIndex() const
{
	int idx;

	if (fabs(x) < fabs(y))
	{
		if (fabs(x) < fabs(z))
			idx = 0;
		else
			idx = 2;
	}
	else
	{
		if (fabs(y) < fabs(z))
			idx = 1;
		else
			idx = 2;
	}

	return idx;
}

inline int Point3F::getGreatestComponentIndex() const
{
	int idx;

	if (fabs(x) > fabs(y))
	{
		if (fabs(x) > fabs(z))
			idx = 0;
		else
			idx = 2;
	}
	else
	{
		if (fabs(y) > fabs(z))
			idx = 1;
		else
			idx = 2;
	}

	return idx;
}

inline float Point3F::least() const
{
	return getMin(fabs(x), getMin(fabs(y), fabs(z)));
}

inline float Point3F::most() const
{
	return getMax(fabs(x), getMax(fabs(y), fabs(z)));
}

inline void Point3F::neg()
{
	x = -x;
	y = -y;
	z = -z;
}

inline void Point3F::convolve(const Point3F& c)
{
	x *= c.x;
	y *= c.y;
	z *= c.z;
}

inline void Point3F::convolveInverse(const Point3F& c)
{
	x /= c.x;
	y /= c.y;
	z /= c.z;
}

inline float Point3F::lenSquared() const
{
	return (x * x) + (y * y) + (z * z);
}

inline float Point3F::len() const
{
	return sqrt(x*x + y*y + z*z);
}

inline void Point3F::normalize()
{
	float* p = *this;
	float squared = p[0] * p[0] + p[1] * p[1] + p[2] * p[2];
	// This can happen in Container::castRay -> ForceFieldBare::castRay
	//AssertFatal(squared != 0.0, "Error, zero length vector normalized!");
	if (squared != 0.0f) {
		float factor = 1.0f / sqrt(squared);
		p[0] *= factor;
		p[1] *= factor;
		p[2] *= factor;
	}
	else {
		p[0] = 0.0f;
		p[1] = 0.0f;
		p[2] = 1.0f;
	}
}

inline float Point3F::magnitudeSafe() const
{
	if (isZero())
	{
		return 0.0f;
	}
	else
	{
		return len();
	}
}

inline bool Point3F::operator==(const Point3F& _test) const
{
	return (x == _test.x) && (y == _test.y) && (z == _test.z);
}

inline bool Point3F::operator!=(const Point3F& _test) const
{
	return operator==(_test) == false;
}

inline Point3F Point3F::operator+(const Point3F& _add) const
{
	return Point3F(x + _add.x, y + _add.y, z + _add.z);
}

inline Point3F Point3F::operator-(const Point3F& _rSub) const
{
	return Point3F(x - _rSub.x, y - _rSub.y, z - _rSub.z);
}

inline Point3F& Point3F::operator+=(const Point3F& _add)
{
	x += _add.x;
	y += _add.y;
	z += _add.z;

	return *this;
}

inline Point3F& Point3F::operator-=(const Point3F& _rSub)
{
	x -= _rSub.x;
	y -= _rSub.y;
	z -= _rSub.z;

	return *this;
}

inline Point3F Point3F::operator*(float _mul) const
{
	return Point3F(x * _mul, y * _mul, z * _mul);
}

inline Point3F Point3F::operator/(float _div) const
{

	float inv = 1.0f / _div;

	return Point3F(x * inv, y * inv, z * inv);
}

inline Point3F& Point3F::operator*=(float _mul)
{
	x *= _mul;
	y *= _mul;
	z *= _mul;

	return *this;
}

inline Point3F& Point3F::operator/=(float _div)
{


	float inv = 1.0f / _div;
	x *= inv;
	y *= inv;
	z *= inv;

	return *this;
}

inline Point3F Point3F::operator*(const Point3F &_vec) const
{
	return Point3F(x * _vec.x, y * _vec.y, z * _vec.z);
}

inline Point3F& Point3F::operator*=(const Point3F &_vec)
{
	x *= _vec.x;
	y *= _vec.y;
	z *= _vec.z;
	return *this;
}

inline Point3F Point3F::operator/(const Point3F &_vec) const
{
	return Point3F(x / _vec.x, y / _vec.y, z / _vec.z);
}

inline Point3F& Point3F::operator/=(const Point3F &_vec)
{
	x /= _vec.x;
	y /= _vec.y;
	z /= _vec.z;
	return *this;
}

inline Point3F Point3F::operator-() const
{
	return Point3F(-x, -y, -z);
}




#ifndef _MMATRIX_H_
#define _MMATRIX_H_

class MatrixF
{
private:
	float m[16];     ///< Note: Torque uses row-major matrices

public:
	/// Create an uninitialized matrix.
	///
	/// @param   identity    If true, initialize to the identity matrix.
	explicit MatrixF(bool identity = false);

	/// Get the index in m to element in column i, row j
	///
	/// This is necessary as we have m as a one dimensional array.
	///
	/// @param   i   Column desired.
	/// @param   j   Row desired.
	static int idx(int i, int j) { return (i + j * 4); }

	/// Initialize matrix with a cross product of p.
	MatrixF& setCrossProduct(const Point3F &p);

	/// Initialize matrix with a tensor product of p.
	MatrixF& setTensorProduct(const Point3F &p, const Point3F& q);

	operator float*() { return (m); }              ///< Allow people to get at m.
	operator const float*() const { return (float*)(m); }  ///< Allow people to get at m.

	bool isAffine() const;                       ///< Check to see if this is an affine matrix.
	bool isIdentity() const;                     ///< Checks for identity matrix.

	/// Make this an identity matrix.
	MatrixF& identity();

	/// Invert m.
	MatrixF& inverse();
	/// Copy the inversion of this into out matrix.
	void invertTo(MatrixF *out);

	/// Take inverse of matrix assuming it is affine (rotation,
	/// scale, sheer, translation only).
	MatrixF& affineInverse();

	/// Swap rows and columns.
	MatrixF& transpose();

	/// M * Matrix(p) -> M
	MatrixF& scale(const Point3F &s);
	MatrixF& scale(float s) { return scale(Point3F(s, s, s)); }

	/// Return scale assuming scale was applied via mat.scale(s).
	Point3F getScale() const;

	/// Compute the inverse of the matrix.
	///
	/// Computes inverse of full 4x4 matrix. Returns false and performs no inverse if
	/// the determinant is 0.
	///
	/// Note: In most cases you want to use the normal inverse function.  This method should
	///       be used if the matrix has something other than (0,0,0,1) in the bottom row.
	bool fullInverse();

	/// Swaps rows and columns into matrix.
	void transposeTo(float *matrix) const;

	/// Normalize the matrix.
	void normalize();

	/// Copy the requested column into a Point3F.
	///
	/// This drops the bottom-most row.
	void getColumn(int col, Point3F *cptr) const;
	Point3F getColumn3F(int col) const { Point3F ret; getColumn(col, &ret); return ret; }

	/// Set the specified column from a Point3F.
	///
	/// The bottom-most row is not set.
	void setColumn(int col, const Point3F& cptr);

	/// Copy the specified row into a Point3F.
	///
	/// Right-most item is dropped.
	void getRow(int row, Point3F *cptr) const;
	Point3F getRow3F(int row) const { Point3F ret; getRow(row, &ret); return ret; }

	/// Set the specified row from a Point3F.
	///
	/// The right-most item is not set.
	void setRow(int row, const Point3F& cptr);

	/// Get the position of the matrix.
	///
	/// This is the 4th column of the matrix.
	Point3F getPosition() const;

	/// Set the position of the matrix.
	///
	/// This is the 4th column of the matrix.
	void setPosition(const Point3F &pos) { setColumn(3, pos); }

	/// Add the passed delta to the matrix position.
	void displace(const Point3F &delta);

	/// Get the x axis of the matrix.
	///
	/// This is the 1st column of the matrix and is
	/// normally considered the right vector.
	Point3F getRightVector() const;

	/// Get the y axis of the matrix.
	///
	/// This is the 2nd column of the matrix and is
	/// normally considered the forward vector.   
	Point3F getForwardVector() const;

	/// Get the z axis of the matrix.
	///
	/// This is the 3rd column of the matrix and is
	/// normally considered the up vector.   
	Point3F getUpVector() const;

	MatrixF&  mul(const MatrixF &a);                    ///< M * a -> M
	MatrixF&  mulL(const MatrixF &a);                   ///< a * M -> M
	MatrixF&  mul(const MatrixF &a, const MatrixF &b);  ///< a * b -> M

	// Scalar multiplies
	MatrixF&  mul(const float a);                         ///< M * a -> M
	MatrixF&  mul(const MatrixF &a, const float b);       ///< a * b -> M


	void mulP(Point3F& p) const;                      ///< M * p -> p (assume w = 1.0f)
	void mulP(const Point3F &p, Point3F *d) const;     ///< M * p -> d (assume w = 1.0f)
	void mulV(Point3F& p) const;                      ///< M * v -> v (assume w = 0.0f)
	void mulV(const Point3F &p, Point3F *d) const;     ///< M * v -> d (assume w = 0.0f)

	MatrixF& add(const MatrixF& m);

	/// Convenience function to allow people to treat this like an array.
	float& operator ()(int row, int col) { return m[idx(col, row)]; }
	float operator ()(int row, int col) const { return m[idx(col, row)]; }

	void dumpMatrix(const char *caption = NULL) const;

	// Math operator overloads
	//------------------------------------
	friend MatrixF operator * (const MatrixF &m1, const MatrixF &m2);
	MatrixF& operator *= (const MatrixF &m);

	// Static identity matrix
	const static MatrixF Identity;
};


//--------------------------------------
// Inline Functions

inline MatrixF::MatrixF(bool _identity)
{
	if (_identity)
		identity();
}

inline MatrixF& MatrixF::setCrossProduct(const Point3F &p)
{
	m[1] = -(m[4] = p.z);
	m[8] = -(m[2] = p.y);
	m[6] = -(m[9] = p.x);
	m[0] = m[3] = m[5] = m[7] = m[10] = m[11] =
		m[12] = m[13] = m[14] = 0.0f;
	m[15] = 1;
	return (*this);
}

inline MatrixF& MatrixF::setTensorProduct(const Point3F &p, const Point3F &q)
{
	m[0] = p.x * q.x;
	m[1] = p.x * q.y;
	m[2] = p.x * q.z;
	m[4] = p.y * q.x;
	m[5] = p.y * q.y;
	m[6] = p.y * q.z;
	m[8] = p.z * q.x;
	m[9] = p.z * q.y;
	m[10] = p.z * q.z;
	m[3] = m[7] = m[11] = m[12] = m[13] = m[14] = 0.0f;
	m[15] = 1.0f;
	return (*this);
}

inline bool MatrixF::isIdentity() const
{
	return
		m[0] == 1.0f &&
		m[1] == 0.0f &&
		m[2] == 0.0f &&
		m[3] == 0.0f &&
		m[4] == 0.0f &&
		m[5] == 1.0f &&
		m[6] == 0.0f &&
		m[7] == 0.0f &&
		m[8] == 0.0f &&
		m[9] == 0.0f &&
		m[10] == 1.0f &&
		m[11] == 0.0f &&
		m[12] == 0.0f &&
		m[13] == 0.0f &&
		m[14] == 0.0f &&
		m[15] == 1.0f;
}

inline MatrixF& MatrixF::identity()
{
	m[0] = 1.0f;
	m[1] = 0.0f;
	m[2] = 0.0f;
	m[3] = 0.0f;
	m[4] = 0.0f;
	m[5] = 1.0f;
	m[6] = 0.0f;
	m[7] = 0.0f;
	m[8] = 0.0f;
	m[9] = 0.0f;
	m[10] = 1.0f;
	m[11] = 0.0f;
	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = 0.0f;
	m[15] = 1.0f;
	return (*this);
}

inline Point3F MatrixF::getScale() const
{
	Point3F scale;
	scale.x = sqrt(m[0] * m[0] + m[4] * m[4] + m[8] * m[8]);
	scale.y = sqrt(m[1] * m[1] + m[5] * m[5] + m[9] * m[9]);
	scale.z = sqrt(m[2] * m[2] + m[6] * m[6] + m[10] * m[10]);
	return scale;
}


inline MatrixF& MatrixF::mul(const float a)
{
	for (int i = 0; i < 16; i++)
		m[i] *= a;

	return *this;
}


inline MatrixF& MatrixF::mul(const MatrixF &a, const float b)
{
	*this = a;
	mul(b);

	return *this;
}

inline MatrixF& MatrixF::add(const MatrixF& a)
{
	for (int i = 0; i < 16; ++i)
		m[i] += a.m[i];

	return *this;
}

inline void MatrixF::getColumn(int col, Point3F *cptr) const
{
	cptr->x = m[col];
	cptr->y = m[col + 4];
	cptr->z = m[col + 8];
}


inline void MatrixF::setColumn(int col, const Point3F &cptr)
{
	m[col] = cptr.x;
	m[col + 4] = cptr.y;
	m[col + 8] = cptr.z;
}


inline void MatrixF::getRow(int col, Point3F *cptr) const
{
	col *= 4;
	cptr->x = m[col++];
	cptr->y = m[col++];
	cptr->z = m[col];
}

inline void MatrixF::setRow(int col, const Point3F &cptr)
{
	col *= 4;
	m[col++] = cptr.x;
	m[col++] = cptr.y;
	m[col] = cptr.z;
}

inline Point3F MatrixF::getPosition() const
{
	return Point3F(m[3], m[3 + 4], m[3 + 8]);
}

inline void MatrixF::displace(const Point3F &delta)
{
	m[3] += delta.x;
	m[3 + 4] += delta.y;
	m[3 + 8] += delta.z;
}

inline Point3F MatrixF::getForwardVector() const
{
	Point3F vec;
	getColumn(1, &vec);
	return vec;
}

inline Point3F MatrixF::getRightVector() const
{
	Point3F vec;
	getColumn(0, &vec);
	return vec;
}

inline Point3F MatrixF::getUpVector() const
{
	Point3F vec;
	getColumn(2, &vec);
	return vec;
}

#endif












class Point2I
{
public:
	int x;
	int y;

public:
	Point2I();                               ///< Create an uninitialized point.
	Point2I(const Point2I&);                 ///< Copy constructor
	Point2I(int in_x, int in_y);             ///< Create point from two co-ordinates.

	//-------------------------------------- Non-math mutators and misc functions
	void set(int in_x, int in_y);            ///< Set (x,y) position
	void setMin(const Point2I&);             ///< Store lesser co-ordinates from parameter in this point.
	void setMax(const Point2I&);             ///< Store greater co-ordinates from parameter in this point.

	//-------------------------------------- Math mutators
	void neg();                              ///< Invert sign of point's co-ordinates.
	void convolve(const Point2I&);           ///< Convolve this point by parameter.

	//-------------------------------------- Queries
	bool isZero() const;                     ///< Is this point at the origin? (No epsilon used)
	float  len() const;                        ///< Get the length of the point
	int  lenSquared() const;                 ///< Get the length-squared of the point

	//-------------------------------------- Overloaded operators
public:
	operator int*() { return &x; }
	operator const int*() const { return &x; }

	// Comparison operators
	bool operator==(const Point2I&) const;
	bool operator!=(const Point2I&) const;

	// Arithmetic w/ other points
	Point2I  operator+(const Point2I&) const;
	Point2I  operator-(const Point2I&) const;
	Point2I& operator+=(const Point2I&);
	Point2I& operator-=(const Point2I&);

	// Arithmetic w/ scalars
	Point2I  operator*(int) const;
	Point2I& operator*=(int);
	Point2I  operator/(int) const;
	Point2I& operator/=(int);

	Point2I  operator*(const Point2I&) const;
	Point2I& operator*=(const Point2I&);
	Point2I  operator/(const Point2I&) const;
	Point2I& operator/=(const Point2I&);

	// Unary operators
	Point2I operator-() const;

	//-------------------------------------- Public static constants
public:
	const static Point2I One;
	const static Point2I Zero;
	const static Point2I Min;
	const static Point2I Max;
};

inline Point2I::Point2I()
{
	//
}


inline Point2I::Point2I(const Point2I& _copy)
: x(_copy.x), y(_copy.y)
{
	//
}


inline Point2I::Point2I(int _x, int _y)
: x(_x), y(_y)
{
	//
}


inline void Point2I::set(int _x, int _y)
{
	x = _x;
	y = _y;
}


inline void Point2I::setMin(const Point2I& _test)
{
	x = (_test.x < x) ? _test.x : x;
	y = (_test.y < y) ? _test.y : y;
}


inline void Point2I::setMax(const Point2I& _test)
{
	x = (_test.x > x) ? _test.x : x;
	y = (_test.y > y) ? _test.y : y;
}


inline void Point2I::neg()
{
	x = -x;
	y = -y;
}

inline void Point2I::convolve(const Point2I& c)
{
	x *= c.x;
	y *= c.y;
}

inline bool Point2I::isZero() const
{
	return ((x == 0) && (y == 0));
}


inline float Point2I::len() const
{
	return sqrt(float(x*x + y*y));
}

inline int Point2I::lenSquared() const
{
	return x*x + y*y;
}

inline bool Point2I::operator==(const Point2I& _test) const
{
	return ((x == _test.x) && (y == _test.y));
}


inline bool Point2I::operator!=(const Point2I& _test) const
{
	return (operator==(_test) == false);
}


inline Point2I Point2I::operator+(const Point2I& _add) const
{
	return Point2I(x + _add.x, y + _add.y);
}


inline Point2I Point2I::operator-(const Point2I& _rSub) const
{
	return Point2I(x - _rSub.x, y - _rSub.y);
}


inline Point2I& Point2I::operator+=(const Point2I& _add)
{
	x += _add.x;
	y += _add.y;

	return *this;
}


inline Point2I& Point2I::operator-=(const Point2I& _rSub)
{
	x -= _rSub.x;
	y -= _rSub.y;

	return *this;
}


inline Point2I Point2I::operator-() const
{
	return Point2I(-x, -y);
}


inline Point2I Point2I::operator*(int mul) const
{
	return Point2I(x * mul, y * mul);
}

inline Point2I Point2I::operator/(int div) const
{
	return Point2I(x / div, y / div);
}


inline Point2I& Point2I::operator*=(int mul)
{
	x *= mul;
	y *= mul;

	return *this;
}


inline Point2I& Point2I::operator/=(int div)
{
	x /= div;
	y /= div;

	return *this;
}

inline Point2I Point2I::operator*(const Point2I &_vec) const
{
	return Point2I(x * _vec.x, y * _vec.y);
}

inline Point2I& Point2I::operator*=(const Point2I &_vec)
{
	x *= _vec.x;
	y *= _vec.y;
	return *this;
}

inline Point2I Point2I::operator/(const Point2I &_vec) const
{
	return Point2I(x / _vec.x, y / _vec.y);
}

inline Point2I& Point2I::operator/=(const Point2I &_vec)
{
	x /= _vec.x;
	y /= _vec.y;
	return *this;
}

inline Point2I operator*(int mul, const Point2I& multiplicand)
{
	return multiplicand * mul;
}

#endif


#ifndef _MRECT_H_
#define _MRECT_H_

#ifndef _MPOINT2_H_
#include "math/mPoint2.h"
#endif

class RectI
{
public:
	Point2I  point;
	Point2I  extent;

public:
	RectI() { }
	RectI(const Point2I& in_rMin,
		const Point2I& in_rExtent);
	RectI(const int in_left, const int in_top,
		const int in_width, const int in_height);

	void set(const Point2I& in_rMin, const Point2I& in_rExtent);
	void set(const int in_left, const int in_top,
		const int in_width, const int in_height);

	bool intersect(const RectI& clipRect);
	bool pointInRect(const Point2I& pt) const;
	bool overlaps(RectI R) const;
	bool contains(const RectI& R) const;
	void inset(int x, int y);

	void unionRects(const RectI&);

	int   len_x() const;
	int   len_y() const;

	/// Returns the area of the rectangle.
	int area() const { return extent.x * extent.y; }

	bool operator==(const RectI&) const;
	bool operator!=(const RectI&) const;

	bool isValidRect() const { return (extent.x > 0 && extent.y > 0); }

public:

	/// A rect of zero extent.
	static const RectI Zero;

	/// A rect of 1,1 extent.
	static const RectI One;

};

inline
RectI::RectI(const Point2I& in_rMin,
const Point2I& in_rExtent)
: point(in_rMin),
extent(in_rExtent)
{
	//
}

inline
RectI::RectI(const int in_left, const int in_top,
const int in_width, const int in_height)
: point(in_left, in_top),
extent(in_width, in_height)
{
	//
}

inline void RectI::set(const Point2I& in_rMin, const Point2I& in_rExtent)
{
	point = in_rMin;
	extent = in_rExtent;
}

inline void RectI::set(const int in_left, const int in_top,
	const int in_width, const int in_height)
{
	point.set(in_left, in_top);
	extent.set(in_width, in_height);
}

inline bool RectI::intersect(const RectI& clipRect)
{
	Point2I bottomL;
	bottomL.x = getMin(point.x + extent.x - 1, clipRect.point.x + clipRect.extent.x - 1);
	bottomL.y = getMin(point.y + extent.y - 1, clipRect.point.y + clipRect.extent.y - 1);

	point.x = getMax(point.x, clipRect.point.x);
	point.y = getMax(point.y, clipRect.point.y);

	extent.x = bottomL.x - point.x + 1;
	extent.y = bottomL.y - point.y + 1;

	return isValidRect();
}

inline bool RectI::pointInRect(const Point2I &pt) const
{
	return (pt.x >= point.x && pt.x < point.x + extent.x && pt.y >= point.y && pt.y < point.y + extent.y);
}

inline bool RectI::contains(const RectI& R) const
{
	if (point.x <= R.point.x && point.y <= R.point.y)
	if (R.point.x + R.extent.x <= point.x + extent.x)
	if (R.point.y + R.extent.y <= point.y + extent.y)
		return true;
	return false;
}

inline bool RectI::overlaps(RectI R) const
{
	return R.intersect(*this);
}

inline void RectI::inset(int x, int y)
{
	point.x += x;
	point.y += y;
	extent.x -= 2 * x;
	extent.y -= 2 * y;
}

struct Collision
{
	DWORD object;
	Point3F point;
	Point3F normal;
	int material;
	int face;
	float faceDot;
	float distance;
};

struct RayInfo : public Collision
{
	float t;
};

#endif