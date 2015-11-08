
#ifndef NSVEC4_H
#define NSVEC4_H

#include "nsvec3.h"

template <class T>
nsvec4<T> operator*(const int32 & lhs_, const nsvec4<T> & rhs_);

template <class T>
nsvec4<T> operator*(const float & lhs_, const nsvec4<T> & rhs_);

template <class T>
nsvec4<T> operator*(const double & lhs_, const nsvec4<T> & rhs_);

template<class T>
nsvec4<T> operator/(const int32 & lhs_, const nsvec4<T> & rhs_);

template <class T>
nsvec4<T> operator/(const float & lhs_, const nsvec4<T> & rhs_);

template <class T>
nsvec4<T> operator/(const double & lhs_, const nsvec4<T> & rhs_);

template <class T>
nsvec4<T> abs(const nsvec4<T> & vec_);

template <class T>
nsvec4<T> axis_angle(const nsvec3<T> & euler_, typename nsvec3<T>::EulerOrder order_, bool rads_ = false);

template <class T>
nsvec4<T> axis_angle(const nsquat<T> & orientation_, bool rads_ = false);

template <class T>
nsvec4<T> axis_angle(const nsmat3<T> & rotation_mat3_, bool rads_ = false);

template <class T>
nsvec4<T> axis_angle(const nsmat4<T> & transform_, bool rads_ = false);

template <class T>
nsvec4<T> axis_angle(const nsvec3<T> & vec_, const nsvec3<T> & to_vec_, bool rads_ = false);

template <class T>
nsvec4<T> ceil(const nsvec4<T> & vec_);

template <class T>
nsvec4<T> clamp(const nsvec4<T> & vec_, const T & min_, const T & max_);

template <class T>
T distance(const nsvec4<T> & lvec_, const nsvec4<T> & rvec_);

template <class T>
T dot(const nsvec4<T> & lhs_, const nsvec4<T> & rhs_);

template <class T>
nsvec4<T> floor(const nsvec4<T> & vec_);

template <class T>
nsvec4<T> fract(const nsvec4<T> & vec_);

template <class T>
T length(const nsvec4<T> & vec_);

template <class T, class T2>
nsvec4<T> lerp(const nsvec4<T> & lhs_, const nsvec4<T> & rhs_, T2 scaling_factor_);

template <class T>
nsvec4<T> min(const nsvec4<T> & lhs_, const nsvec4<T> & rhs_);

template <class T>
nsvec4<T> max(const nsvec4<T> & lhs_, const nsvec4<T> & rhs_);

template <class T>
nsvec4<T> normalize(const nsvec4<T> & rhs_);

template <class T>
nsvec4<T> round(const nsvec4<T> & vec_);

template<class PUPer, class T>
void pup(PUPer & p, nsvec4<T> & v4);

template <class T>
struct nsvec4
{
	nsvec4(const nsvec4<T> & copy_) : x(copy_.x), y(copy_.y), z(copy_.z), w(copy_.w) {}
	nsvec4(const T & val = static_cast<T>(0)) : x(val), y(val), z(val), w(val) {}
	nsvec4(const T & x_, const T & y_, const T & z_ = static_cast<T>(0), const T & w_ = static_cast<T>(0)) : x(x_), y(y_), z(z_), w(w_) {}
	nsvec4(const nsvec3<T> & xyz, const T & w_ = static_cast<T>(1)) : x(xyz.x), y(xyz.y), z(xyz.z), w(w_) {}
	nsvec4(const T & x_, const nsvec3<T> & yzw) : x(x_), y(yzw.x), z(yzw.y), w(yzw.z) {}
	nsvec4(const nsvec2<T> & xy, const T & z_ = static_cast<T>(0), const T & w_ = static_cast<T>(0)) : x(xy.x), y(xy.y), z(z_), w(w_) {}
	nsvec4(const T & x_, const nsvec2<T> & yz, const T & w_ = static_cast<T>(0)) : x(x_), y(yz.x), z(yz.z), w(w_) {}
	nsvec4(const T & x_, const T & y_, const nsvec2<T> & zw) : x(x_), y(y_), z(zw.x), w(zw.y) {}

	nsvec3<T> & abs()
	{
		x = static_cast<T>(std::abs(x));
		y = static_cast<T>(std::abs(y));
		z = static_cast<T>(std::abs(z));
		w = static_cast<T>(std::abs(w));
		return *this;
	}

	nsvec4<T> & axis_angle_from(const nsvec3<T> & euler_, typename nsvec3<T>::EulerOrder order_, bool rads_ = false)
	{
		return axis_angle_from(nsquat<T>().from(euler_, order_, rads_), rads_);
	}

	nsvec4<T> & axis_angle_from(const nsquat<T> & orientation_, bool rads_ = false)
	{
		// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToAngle/index.htm
		w = static_cast<T>(2 * std::acos(orientation_.w));

		if (!rads_)
			w = degrees(w);

		T den = static_cast<T>(std::sqrt(1 - orientation_.w*orientation_.w));
		if (den < EPS)
		{
			x = orientation_.x;
			y = orientation_.y;
			z = orientation_.z;
		}
		else
		{
			x = orientation_.x / den;
			y = orientation_.y / den;
			z = orientation_.z / den;
		}
		return *this;
	}

	nsvec4<T> & axis_angle_from(const nsmat3<T> & rotation_mat3_, bool rads_ = false)
	{
		return axis_angle_from(nsquat<T>().from(rotation_mat3_), rads_);
	}

	nsvec4<T> & axis_angle_from(const nsmat4<T> & transform_, bool rads_ = false)
	{
		return axis_angle_from(nsquat<T>().from(transform_), rads_);
	}

	nsvec4<T> & axis_angle_from(const nsvec3<T> & vec_, const nsvec3<T> & to_vec_, bool rads_ = false)
	{
		return axis_angle_from(nsquat<T>().from(vec_, to_vec_), rads_);
	}

	nsvec4<T> & ceil()
	{
		x = static_cast<T>(std::ceil(x));
		y = static_cast<T>(std::ceil(y));
		z = static_cast<T>(std::ceil(z));
		w = static_cast<T>(std::ceil(w));
		return *this;
	}

	nsvec4<T> & clamp(const T & min_ = static_cast<T>(0), const T & max_ = static_cast<T>(0))
	{
		if (x < min_)
			x = min_;
		if (y < min_)
			y = min_;
		if (z < min_)
			z = min_;
		if (w < min_)
			w = min_;
		if (x > max_)
			x = max_;
		if (y > max_)
			y = max_;
		if (z > max_)
			z = max_;
		if (w > max_)
			w = max_;
		return *this;
	}

	T distance_to(const nsvec4<T> & vec_) const
	{
		return ((vec_ - *this).length());
	}

	nsvec4<T> & floor()
	{
		x = static_cast<T>(std::floor(x));
		y = static_cast<T>(std::floor(y));
		z = static_cast<T>(std::floor(z));
		w = static_cast<T>(std::floor(w));
		return *this;
	}

	nsvec4<T> & fract()
	{
		for (uint32 i = 0; i < 4; ++i)
		{
			if (data[i] >= 0)
				data[i] -= static_cast<T>(std::floor(data[i]));
			else
				data[i] -= static_cast<T>(std::ceil(data[i]));
		}
		return *this;
	}

	T length() const
	{
		return static_cast<T>(sqrt(x*x + y*y + z*z + w*w));
	}

	T length_sq() const
	{
		return x*x + y*y + z*z + w*w;
	}

	template<class T2 >
	nsvec4<T> & lerp(const nsvec4<T> & vec_, const T2 & scaling_factor_)
	{
		x += static_cast<T>((vec_.x - x)*scaling_factor_);
		y += static_cast<T>((vec_.y - y)*scaling_factor_);
		z += static_cast<T>((vec_.z - z)*scaling_factor_);
		w += static_cast<T>((vec_.w - w)*scaling_factor_);
		return *this;
	}

	T min()
	{
		T m1 = xyz().min();
		if (m1 < w)
			return m1;
		return w;
	}

	nsvec4<T> & minimize(const nsvec4<T> & rhs_)
	{
		if (x < rhs_.x)
			x = rhs_.x;
		if (y < rhs_.y)
			y = rhs_.y;
		if (z < rhs_.z)
			z = rhs_.z;
		if (w < rhs_.w)
			w = rhs_.w;
		return *this;
	}

	T max()
	{
		T m1 = xyz().max();
		if (m1 > w)
			return m1;
		return w;
	}

	nsvec4<T> & maximize(const nsvec4<T> & rhs_)
	{
		if (x > rhs_.x)
			x = rhs_.x;
		if (y > rhs_.y)
			y = rhs_.y;
		if (z > rhs_.z)
			z = rhs_.z;
		if (w > rhs_.w)
			w = rhs_.w;
		return *this;
	}

	nsvec4<T> & normalize()
	{
		T l = length();
		if (l == 0)
			return *this;
		return *this *= (1 / l);
	}

	nsvec4<T> & round()
	{
		x = static_cast<T>(std::round(x));
		y = static_cast<T>(std::round(y));
		z = static_cast<T>(std::round(z));
		w = static_cast<T>(std::round(w));
		return *this;
	}

	nsvec4<T> & round_to_zero()
	{
		if (std::abs(x) < EPS_VEC_0)
			x = 0;
		if (std::abs(y) < EPS_VEC_0)
			y = 0;
		if (std::abs(z) < EPS_VEC_0)
			z = 0;
		if (std::abs(w) < EPS_VEC_0)
			w = 0;
		return *this;
	}

	nsvec3<T> & scaling_from(const nsmat3<T> & transform_)
	{
		x = transform_[0].length();
		y = transform_[1].length();
		z = transform_[2].length();
		w = static_cast<T>(1);
		return *this;
	}

	nsvec3<T> & scaling_from(const nsmat4<T> & transform_)
	{
		x = sqrt(transform_[0][0] * transform_[0][0] + transform_[0][1] * transform_[0][1] + transform_[0][2] * transform_[0][2]);
		y = sqrt(transform_[1][0] * transform_[1][0] + transform_[1][1] * transform_[1][1] + transform_[1][2] * transform_[1][2]);
		z = sqrt(transform_[2][0] * transform_[2][0] + transform_[2][1] * transform_[2][1] + transform_[2][2] * transform_[2][2]);
		w = static_cast<T>(1);
		return *this;
	}

	nsvec4<T> & set(const T & val_)
	{
		x = y = z = w = val_;
		return *this;
	}

	nsvec4<T> & set(const T & x_, const T & y_, const T & z_, const T & w_)
	{
		x = x_; y = y_; z = z_; w = w_;
		return *this;
	}

	nsvec4<T> & set(const nsvec3<T> & xyz, const T & w_)
	{
		x = xyz.x; y = xyz.y; z = xyz.z; w = w_;
		return *this;
	}

	nsvec4<T> & set(const T & x_, const nsvec3<T> & yzw)
	{
		x = x_; y = yzw.x; z = yzw.y; w = yzw.z;
		return *this;
	}

	nsvec4<T> & set(const nsvec2<T> & xy, const T & z_, const T & w_)
	{
		x = xy.x; y = xy.y; z = z_; w = w_;
		return *this;
	}

	nsvec4<T> & set(const T & x_, const nsvec2<T> & yz, const T & w_)
	{
		x = x_; y = yz.x; z = yz.y; w = w_;
		return *this;
	}

	nsvec4<T> & set(const T & x_, const T & y_, const nsvec2<T> & zw)
	{
		x = x_; y = y_; z = zw.x; w = zw.y;
		return *this;
	}

	nsvec4<T> & set_length(const T & len_)
	{
		T l = length();

		if (l == static_cast<T>(0))
			return *this;

		T mult = len_ / l;
		(*this) *= mult;
		return *this;
	}

	nsstring to_string()
	{
		nsstringstream ss;
		ss << "[" << x << " " << y << " " << z << " " << w << "]";
		return ss.str();
	}

	nsvec4<T> & translation_from(const nsmat4<T> & transform_)
	{
		*this = transform_(3);
		w = static_cast<T>(1);
		return *this;
	}

	// overloaded operators
	nsvec4<T> operator+(const nsvec4<T> & rhs_) const
	{
		return nsvec4<T>(x + rhs_.x, y + rhs_.y, z + rhs_.z, w + rhs_.w);
	}

	nsvec4<T> operator-(const nsvec4<T> & rhs_) const
	{
		return nsvec4<T>(x - rhs_.x, y - rhs_.y, z - rhs_.z, w - rhs_.w);
	}

	T operator*(const nsvec4<T> & rhs_) const // dot product
	{
		return x*rhs_.x + y*rhs_.y + z*rhs_.z + w*rhs_.w;
	}

	nsmat4<T> operator^(const nsvec4<T> & rhs_) const
	{
		nsmat4<T> ret;
		ret[0] = x * rhs_;
		ret[1] = y * rhs_;
		ret[2] = z * rhs_;
		ret[3] = w * rhs_;
		return ret;
	}

	nsvec4<T> operator%(const nsvec4<T> & rhs_) const // component wise scalar product
	{
		return nsvec4<T>(x*rhs_.x, y*rhs_.y, z*rhs_.z, w*rhs_.w);
	}

	nsvec4<T> operator/(const nsvec4<T> & rhs_) const
	{
		return nsvec4<T>(x / rhs_.x, y / rhs_.y, z / rhs_.z, w / rhs_.w);
	}

	nsvec4<T> operator*(const T & rhs_) const
	{
		return nsvec4<T>(x * rhs_, y * rhs_, z * rhs_, w * rhs_);
	}

	nsvec4<T> operator/(const T & rhs_) const
	{
		return nsvec4<T>(x / rhs_, y / rhs_, z / rhs_, w / rhs_);
	}

	nsvec4<T> & operator=(const nsvec4<T> & rhs_)
	{
		if (this == &rhs_)
			return *this;
		x = rhs_.x;
		y = rhs_.y;
		z = rhs_.z;
		w = rhs_.w;
		return *this;
	}

	nsvec4<T> operator++(int32)
	{
		nsvec4<T> ret(*this);
		++(*this);
		return ret;
	}

	nsvec4<T> operator--(int32)
	{
		nsvec4<T> ret(*this);
		--(*this);
		return ret;
	}

	nsvec4<T> & operator++()
	{
		++x; ++y; ++z; ++w;
		return *this;
	}

	nsvec4<T> & operator--()
	{
		--x; --y; --z; --w;
		return *this;
	}

	nsvec4<T> & operator+=(const nsvec4<T> & rhs_)
	{
		x += rhs_.x; y += rhs_.y; z += rhs_.z; w += rhs_.w;
		return *this;
	}

	nsvec4<T> & operator-=(const nsvec4<T> & rhs_)
	{
		x -= rhs_.x; y -= rhs_.y; z -= rhs_.z; w -= rhs_.w;
		return *this;
	}

	nsvec4<T> & operator%=(const nsvec4<T> & rhs_)
	{
		x *= rhs_.x; y *= rhs_.y; z *= rhs_.z; w *= rhs_.w;
		return *this;
	}

	nsvec4<T> & operator/=(const nsvec4<T> & rhs_)
	{
		x /= rhs_.x; y /= rhs_.y; z /= rhs_.z; w /= rhs_.w;
		return *this;
	}

	nsvec4<T> & operator*=(const T & rhs_)
	{
		x *= rhs_; y *= rhs_; z *= rhs_; w *= rhs_;
		return *this;
	}

	nsvec4<T> & operator/=(const T & rhs_)
	{
		x /= rhs_; y /= rhs_; z /= rhs_; w /= rhs_;
		return *this;
	}

	bool operator==(const nsvec4<T> & rhs_) const
	{
		return ((x == rhs_.x) && (y == rhs_.y) && (z == rhs_.z) && (w == rhs_.w));
	}

	bool operator!=(const nsvec4<T> & rhs_) const
	{
		return !(*this == rhs_);
	}

	bool operator==(const T & rhs_) const
	{
		return ((x == rhs_) && (y == rhs_) && (z == rhs_) && (w == rhs_));
	}

	bool operator!=(const T & rhs_) const
	{
		return !(*this == rhs_);
	}

	const T & operator[](const uint32 & val_) const
	{
		if (val_ > 3)
			throw(std::out_of_range("vec4 index out of range"));
		return data[val_];
	}

	T & operator[](const uint32 & val_)
	{
		if (val_ > 3)
			throw(std::out_of_range("vec4 index out of range"));
		return data[val_];
	}

	// Vec4 Swizzles
	inline nsvec4<T> xxxx() const { return nsvec4<T>(x, x, x, x); }
	inline nsvec4<T> xxxy() const { return nsvec4<T>(x, x, x, y); }
	inline nsvec4<T> xxxz() const { return nsvec4<T>(x, x, x, z); }
	inline nsvec4<T> xxxw() const { return nsvec4<T>(x, x, x, w); }
	inline nsvec4<T> xxyx() const { return nsvec4<T>(x, x, y, x); }
	inline nsvec4<T> xxyy() const { return nsvec4<T>(x, x, y, y); }
	inline nsvec4<T> xxyz() const { return nsvec4<T>(x, x, y, z); }
	inline nsvec4<T> xxyw() const { return nsvec4<T>(x, x, y, w); }
	inline nsvec4<T> xxzx() const { return nsvec4<T>(x, x, z, x); }
	inline nsvec4<T> xxzy() const { return nsvec4<T>(x, x, z, y); }
	inline nsvec4<T> xxzz() const { return nsvec4<T>(x, x, z, z); }
	inline nsvec4<T> xxzw() const { return nsvec4<T>(x, x, z, w); }
	inline nsvec4<T> xxwx() const { return nsvec4<T>(x, x, w, x); }
	inline nsvec4<T> xxwy() const { return nsvec4<T>(x, x, w, y); }
	inline nsvec4<T> xxwz() const { return nsvec4<T>(x, x, w, z); }
	inline nsvec4<T> xxww() const { return nsvec4<T>(x, x, w, w); }
	inline nsvec4<T> xyxx() const { return nsvec4<T>(x, y, x, x); }
	inline nsvec4<T> xyxy() const { return nsvec4<T>(x, y, x, y); }
	inline nsvec4<T> xyxz() const { return nsvec4<T>(x, y, x, z); }
	inline nsvec4<T> xyxw() const { return nsvec4<T>(x, y, x, w); }
	inline nsvec4<T> xyyx() const { return nsvec4<T>(x, y, y, x); }
	inline nsvec4<T> xyyy() const { return nsvec4<T>(x, y, y, y); }
	inline nsvec4<T> xyyz() const { return nsvec4<T>(x, y, y, z); }
	inline nsvec4<T> xyyw() const { return nsvec4<T>(x, y, y, w); }
	inline nsvec4<T> xywx() const { return nsvec4<T>(x, y, w, x); }
	inline nsvec4<T> xywy() const { return nsvec4<T>(x, y, w, y); }
	inline nsvec4<T> xywz() const { return nsvec4<T>(x, y, w, z); }
	inline nsvec4<T> xyww() const { return nsvec4<T>(x, y, w, w); }
	inline nsvec4<T> xzxx() const { return nsvec4<T>(x, z, x, x); }
	inline nsvec4<T> xzxy() const { return nsvec4<T>(x, z, x, y); }
	inline nsvec4<T> xzxz() const { return nsvec4<T>(x, z, x, z); }
	inline nsvec4<T> xzxw() const { return nsvec4<T>(x, z, x, w); }
	inline nsvec4<T> xzyx() const { return nsvec4<T>(x, z, y, x); }
	inline nsvec4<T> xzyy() const { return nsvec4<T>(x, z, y, y); }
	inline nsvec4<T> xzyz() const { return nsvec4<T>(x, z, y, z); }
	inline nsvec4<T> xzyw() const { return nsvec4<T>(x, z, y, w); }
	inline nsvec4<T> xzzx() const { return nsvec4<T>(x, z, z, x); }
	inline nsvec4<T> xzzy() const { return nsvec4<T>(x, z, z, y); }
	inline nsvec4<T> xzzz() const { return nsvec4<T>(x, z, z, z); }
	inline nsvec4<T> xzzw() const { return nsvec4<T>(x, z, z, w); }
	inline nsvec4<T> xzwx() const { return nsvec4<T>(x, z, w, x); }
	inline nsvec4<T> xzwy() const { return nsvec4<T>(x, z, w, y); }
	inline nsvec4<T> xzwz() const { return nsvec4<T>(x, z, w, z); }
	inline nsvec4<T> xzww() const { return nsvec4<T>(x, z, w, w); }
	inline nsvec4<T> xwxx() const { return nsvec4<T>(x, w, x, x); }
	inline nsvec4<T> xwxy() const { return nsvec4<T>(x, w, x, y); }
	inline nsvec4<T> xwxz() const { return nsvec4<T>(x, w, x, z); }
	inline nsvec4<T> xwxw() const { return nsvec4<T>(x, w, x, w); }
	inline nsvec4<T> xwyx() const { return nsvec4<T>(x, w, y, x); }
	inline nsvec4<T> xwyy() const { return nsvec4<T>(x, w, y, y); }
	inline nsvec4<T> xwyz() const { return nsvec4<T>(x, w, y, z); }
	inline nsvec4<T> xwyw() const { return nsvec4<T>(x, w, y, w); }
	inline nsvec4<T> xwzx() const { return nsvec4<T>(x, w, z, x); }
	inline nsvec4<T> xwzy() const { return nsvec4<T>(x, w, z, y); }
	inline nsvec4<T> xwzz() const { return nsvec4<T>(x, w, z, z); }
	inline nsvec4<T> xwzw() const { return nsvec4<T>(x, w, z, w); }
	inline nsvec4<T> xwwx() const { return nsvec4<T>(x, w, w, x); }
	inline nsvec4<T> xwwy() const { return nsvec4<T>(x, w, w, y); }
	inline nsvec4<T> xwwz() const { return nsvec4<T>(x, w, w, z); }
	inline nsvec4<T> xwww() const { return nsvec4<T>(x, w, w, w); }
	inline nsvec4<T> yxxx() const { return nsvec4<T>(y, x, x, x); }
	inline nsvec4<T> yxxy() const { return nsvec4<T>(y, x, x, y); }
	inline nsvec4<T> yxxz() const { return nsvec4<T>(y, x, x, z); }
	inline nsvec4<T> yxxw() const { return nsvec4<T>(y, x, x, w); }
	inline nsvec4<T> yxyx() const { return nsvec4<T>(y, x, y, x); }
	inline nsvec4<T> yxyy() const { return nsvec4<T>(y, x, y, y); }
	inline nsvec4<T> yxyz() const { return nsvec4<T>(y, x, y, z); }
	inline nsvec4<T> yxyw() const { return nsvec4<T>(y, x, y, w); }
	inline nsvec4<T> yxzx() const { return nsvec4<T>(y, x, z, x); }
	inline nsvec4<T> yxzy() const { return nsvec4<T>(y, x, z, y); }
	inline nsvec4<T> yxzz() const { return nsvec4<T>(y, x, z, z); }
	inline nsvec4<T> yxzw() const { return nsvec4<T>(y, x, z, w); }
	inline nsvec4<T> yxwx() const { return nsvec4<T>(y, x, w, x); }
	inline nsvec4<T> yxwy() const { return nsvec4<T>(y, x, w, y); }
	inline nsvec4<T> yxwz() const { return nsvec4<T>(y, x, w, z); }
	inline nsvec4<T> yxww() const { return nsvec4<T>(y, x, w, w); }
	inline nsvec4<T> yyxx() const { return nsvec4<T>(y, y, x, x); }
	inline nsvec4<T> yyxy() const { return nsvec4<T>(y, y, x, y); }
	inline nsvec4<T> yyxz() const { return nsvec4<T>(y, y, x, z); }
	inline nsvec4<T> yyxw() const { return nsvec4<T>(y, y, x, w); }
	inline nsvec4<T> yyyx() const { return nsvec4<T>(y, y, y, x); }
	inline nsvec4<T> yyyy() const { return nsvec4<T>(y, y, y, y); }
	inline nsvec4<T> yyyz() const { return nsvec4<T>(y, y, y, z); }
	inline nsvec4<T> yyyw() const { return nsvec4<T>(y, y, y, w); }
	inline nsvec4<T> yyzx() const { return nsvec4<T>(y, y, z, x); }
	inline nsvec4<T> yyzy() const { return nsvec4<T>(y, y, z, y); }
	inline nsvec4<T> yyzz() const { return nsvec4<T>(y, y, z, z); }
	inline nsvec4<T> yyzw() const { return nsvec4<T>(y, y, z, w); }
	inline nsvec4<T> yywx() const { return nsvec4<T>(y, y, w, x); }
	inline nsvec4<T> yywy() const { return nsvec4<T>(y, y, w, y); }
	inline nsvec4<T> yywz() const { return nsvec4<T>(y, y, w, z); }
	inline nsvec4<T> yyww() const { return nsvec4<T>(y, y, w, w); }
	inline nsvec4<T> yzxx() const { return nsvec4<T>(y, z, x, x); }
	inline nsvec4<T> yzxy() const { return nsvec4<T>(y, z, x, y); }
	inline nsvec4<T> yzxz() const { return nsvec4<T>(y, z, x, z); }
	inline nsvec4<T> yzxw() const { return nsvec4<T>(y, z, x, w); }
	inline nsvec4<T> yzyx() const { return nsvec4<T>(y, z, y, x); }
	inline nsvec4<T> yzyy() const { return nsvec4<T>(y, z, y, y); }
	inline nsvec4<T> yzyz() const { return nsvec4<T>(y, z, y, z); }
	inline nsvec4<T> yzyw() const { return nsvec4<T>(y, z, y, w); }
	inline nsvec4<T> yzzx() const { return nsvec4<T>(y, z, z, x); }
	inline nsvec4<T> yzzy() const { return nsvec4<T>(y, z, z, y); }
	inline nsvec4<T> yzzz() const { return nsvec4<T>(y, z, z, z); }
	inline nsvec4<T> yzzw() const { return nsvec4<T>(y, z, z, w); }
	inline nsvec4<T> yzwx() const { return nsvec4<T>(y, z, w, x); }
	inline nsvec4<T> yzwy() const { return nsvec4<T>(y, z, w, y); }
	inline nsvec4<T> yzwz() const { return nsvec4<T>(y, z, w, z); }
	inline nsvec4<T> yzww() const { return nsvec4<T>(y, z, w, w); }
	inline nsvec4<T> ywxx() const { return nsvec4<T>(y, w, x, x); }
	inline nsvec4<T> ywxy() const { return nsvec4<T>(y, w, x, y); }
	inline nsvec4<T> ywxz() const { return nsvec4<T>(y, w, x, z); }
	inline nsvec4<T> ywxw() const { return nsvec4<T>(y, w, x, w); }
	inline nsvec4<T> ywyx() const { return nsvec4<T>(y, w, y, x); }
	inline nsvec4<T> ywyy() const { return nsvec4<T>(y, w, y, y); }
	inline nsvec4<T> ywyz() const { return nsvec4<T>(y, w, y, z); }
	inline nsvec4<T> ywyw() const { return nsvec4<T>(y, w, y, w); }
	inline nsvec4<T> ywzx() const { return nsvec4<T>(y, w, z, x); }
	inline nsvec4<T> ywzy() const { return nsvec4<T>(y, w, z, y); }
	inline nsvec4<T> ywzz() const { return nsvec4<T>(y, w, z, z); }
	inline nsvec4<T> ywzw() const { return nsvec4<T>(y, w, z, w); }
	inline nsvec4<T> ywwx() const { return nsvec4<T>(y, w, w, x); }
	inline nsvec4<T> ywwy() const { return nsvec4<T>(y, w, w, y); }
	inline nsvec4<T> ywwz() const { return nsvec4<T>(y, w, w, z); }
	inline nsvec4<T> ywww() const { return nsvec4<T>(y, w, w, w); }
	inline nsvec4<T> zxxx() const { return nsvec4<T>(z, x, x, x); }
	inline nsvec4<T> zxxy() const { return nsvec4<T>(z, x, x, y); }
	inline nsvec4<T> zxxz() const { return nsvec4<T>(z, x, x, z); }
	inline nsvec4<T> zxxw() const { return nsvec4<T>(z, x, x, w); }
	inline nsvec4<T> zxyx() const { return nsvec4<T>(z, x, y, x); }
	inline nsvec4<T> zxyy() const { return nsvec4<T>(z, x, y, y); }
	inline nsvec4<T> zxyz() const { return nsvec4<T>(z, x, y, z); }
	inline nsvec4<T> zxyw() const { return nsvec4<T>(z, x, y, w); }
	inline nsvec4<T> zxzx() const { return nsvec4<T>(z, x, z, x); }
	inline nsvec4<T> zxzy() const { return nsvec4<T>(z, x, z, y); }
	inline nsvec4<T> zxzz() const { return nsvec4<T>(z, x, z, z); }
	inline nsvec4<T> zxzw() const { return nsvec4<T>(z, x, z, w); }
	inline nsvec4<T> zxwx() const { return nsvec4<T>(z, x, w, x); }
	inline nsvec4<T> zxwy() const { return nsvec4<T>(z, x, w, y); }
	inline nsvec4<T> zxwz() const { return nsvec4<T>(z, x, w, z); }
	inline nsvec4<T> zxww() const { return nsvec4<T>(z, x, w, w); }
	inline nsvec4<T> zyxx() const { return nsvec4<T>(z, y, x, x); }
	inline nsvec4<T> zyxy() const { return nsvec4<T>(z, y, x, y); }
	inline nsvec4<T> zyxz() const { return nsvec4<T>(z, y, x, z); }
	inline nsvec4<T> zyxw() const { return nsvec4<T>(z, y, x, w); }
	inline nsvec4<T> zyyx() const { return nsvec4<T>(z, y, y, x); }
	inline nsvec4<T> zyyy() const { return nsvec4<T>(z, y, y, y); }
	inline nsvec4<T> zyyz() const { return nsvec4<T>(z, y, y, z); }
	inline nsvec4<T> zyyw() const { return nsvec4<T>(z, y, y, w); }
	inline nsvec4<T> zyzx() const { return nsvec4<T>(z, y, z, x); }
	inline nsvec4<T> zyzy() const { return nsvec4<T>(z, y, z, y); }
	inline nsvec4<T> zyzz() const { return nsvec4<T>(z, y, z, z); }
	inline nsvec4<T> zyzw() const { return nsvec4<T>(z, y, z, w); }
	inline nsvec4<T> zywx() const { return nsvec4<T>(z, y, w, x); }
	inline nsvec4<T> zywy() const { return nsvec4<T>(z, y, w, y); }
	inline nsvec4<T> zywz() const { return nsvec4<T>(z, y, w, z); }
	inline nsvec4<T> zyww() const { return nsvec4<T>(z, y, w, w); }
	inline nsvec4<T> zzxx() const { return nsvec4<T>(z, z, x, x); }
	inline nsvec4<T> zzxy() const { return nsvec4<T>(z, z, x, y); }
	inline nsvec4<T> zzxz() const { return nsvec4<T>(z, z, x, z); }
	inline nsvec4<T> zzxw() const { return nsvec4<T>(z, z, x, w); }
	inline nsvec4<T> zzyx() const { return nsvec4<T>(z, z, y, x); }
	inline nsvec4<T> zzyy() const { return nsvec4<T>(z, z, y, y); }
	inline nsvec4<T> zzyz() const { return nsvec4<T>(z, z, y, z); }
	inline nsvec4<T> zzyw() const { return nsvec4<T>(z, z, y, w); }
	inline nsvec4<T> zzzx() const { return nsvec4<T>(z, z, z, x); }
	inline nsvec4<T> zzzy() const { return nsvec4<T>(z, z, z, y); }
	inline nsvec4<T> zzzz() const { return nsvec4<T>(z, z, z, z); }
	inline nsvec4<T> zzzw() const { return nsvec4<T>(z, z, z, w); }
	inline nsvec4<T> zzwx() const { return nsvec4<T>(z, z, w, x); }
	inline nsvec4<T> zzwy() const { return nsvec4<T>(z, z, w, y); }
	inline nsvec4<T> zzwz() const { return nsvec4<T>(z, z, w, z); }
	inline nsvec4<T> zzww() const { return nsvec4<T>(z, z, w, w); }
	inline nsvec4<T> zwxx() const { return nsvec4<T>(z, w, x, x); }
	inline nsvec4<T> zwxy() const { return nsvec4<T>(z, w, x, y); }
	inline nsvec4<T> zwxz() const { return nsvec4<T>(z, w, x, z); }
	inline nsvec4<T> zwxw() const { return nsvec4<T>(z, w, x, w); }
	inline nsvec4<T> zwyx() const { return nsvec4<T>(z, w, y, x); }
	inline nsvec4<T> zwyy() const { return nsvec4<T>(z, w, y, y); }
	inline nsvec4<T> zwyz() const { return nsvec4<T>(z, w, y, z); }
	inline nsvec4<T> zwyw() const { return nsvec4<T>(z, w, y, w); }
	inline nsvec4<T> zwzx() const { return nsvec4<T>(z, w, z, x); }
	inline nsvec4<T> zwzy() const { return nsvec4<T>(z, w, z, y); }
	inline nsvec4<T> zwzz() const { return nsvec4<T>(z, w, z, z); }
	inline nsvec4<T> zwzw() const { return nsvec4<T>(z, w, z, w); }
	inline nsvec4<T> zwwx() const { return nsvec4<T>(z, w, w, x); }
	inline nsvec4<T> zwwy() const { return nsvec4<T>(z, w, w, y); }
	inline nsvec4<T> zwwz() const { return nsvec4<T>(z, w, w, z); }
	inline nsvec4<T> zwww() const { return nsvec4<T>(z, w, w, w); }
	inline nsvec4<T> wxxx() const { return nsvec4<T>(w, x, x, x); }
	inline nsvec4<T> wxxy() const { return nsvec4<T>(w, x, x, y); }
	inline nsvec4<T> wxxz() const { return nsvec4<T>(w, x, x, z); }
	inline nsvec4<T> wxxw() const { return nsvec4<T>(w, x, x, w); }
	inline nsvec4<T> wxyx() const { return nsvec4<T>(w, x, y, x); }
	inline nsvec4<T> wxyy() const { return nsvec4<T>(w, x, y, y); }
	inline nsvec4<T> wxyz() const { return nsvec4<T>(w, x, y, z); }
	inline nsvec4<T> wxyw() const { return nsvec4<T>(w, x, y, w); }
	inline nsvec4<T> wxzx() const { return nsvec4<T>(w, x, z, x); }
	inline nsvec4<T> wxzy() const { return nsvec4<T>(w, x, z, y); }
	inline nsvec4<T> wxzz() const { return nsvec4<T>(w, x, z, z); }
	inline nsvec4<T> wxzw() const { return nsvec4<T>(w, x, z, w); }
	inline nsvec4<T> wxwx() const { return nsvec4<T>(w, x, w, x); }
	inline nsvec4<T> wxwy() const { return nsvec4<T>(w, x, w, y); }
	inline nsvec4<T> wxwz() const { return nsvec4<T>(w, x, w, z); }
	inline nsvec4<T> wxww() const { return nsvec4<T>(w, x, w, w); }
	inline nsvec4<T> wyxx() const { return nsvec4<T>(w, y, x, x); }
	inline nsvec4<T> wyxy() const { return nsvec4<T>(w, y, x, y); }
	inline nsvec4<T> wyxz() const { return nsvec4<T>(w, y, x, z); }
	inline nsvec4<T> wyxw() const { return nsvec4<T>(w, y, x, w); }
	inline nsvec4<T> wyyx() const { return nsvec4<T>(w, y, y, x); }
	inline nsvec4<T> wyyy() const { return nsvec4<T>(w, y, y, y); }
	inline nsvec4<T> wyyz() const { return nsvec4<T>(w, y, y, z); }
	inline nsvec4<T> wyyw() const { return nsvec4<T>(w, y, y, w); }
	inline nsvec4<T> wyzx() const { return nsvec4<T>(w, y, z, x); }
	inline nsvec4<T> wyzy() const { return nsvec4<T>(w, y, z, y); }
	inline nsvec4<T> wyzz() const { return nsvec4<T>(w, y, z, z); }
	inline nsvec4<T> wyzw() const { return nsvec4<T>(w, y, z, w); }
	inline nsvec4<T> wywx() const { return nsvec4<T>(w, y, w, x); }
	inline nsvec4<T> wywy() const { return nsvec4<T>(w, y, w, y); }
	inline nsvec4<T> wywz() const { return nsvec4<T>(w, y, w, z); }
	inline nsvec4<T> wyww() const { return nsvec4<T>(w, y, w, w); }
	inline nsvec4<T> wzxx() const { return nsvec4<T>(w, z, x, x); }
	inline nsvec4<T> wzxy() const { return nsvec4<T>(w, z, x, y); }
	inline nsvec4<T> wzxz() const { return nsvec4<T>(w, z, x, z); }
	inline nsvec4<T> wzxw() const { return nsvec4<T>(w, z, x, w); }
	inline nsvec4<T> wzyx() const { return nsvec4<T>(w, z, y, x); }
	inline nsvec4<T> wzyy() const { return nsvec4<T>(w, z, y, y); }
	inline nsvec4<T> wzyz() const { return nsvec4<T>(w, z, y, z); }
	inline nsvec4<T> wzyw() const { return nsvec4<T>(w, z, y, w); }
	inline nsvec4<T> wzzx() const { return nsvec4<T>(w, z, z, x); }
	inline nsvec4<T> wzzy() const { return nsvec4<T>(w, z, z, y); }
	inline nsvec4<T> wzzz() const { return nsvec4<T>(w, z, z, z); }
	inline nsvec4<T> wzzw() const { return nsvec4<T>(w, z, z, w); }
	inline nsvec4<T> wzwx() const { return nsvec4<T>(w, z, w, x); }
	inline nsvec4<T> wzwy() const { return nsvec4<T>(w, z, w, y); }
	inline nsvec4<T> wzwz() const { return nsvec4<T>(w, z, w, z); }
	inline nsvec4<T> wzww() const { return nsvec4<T>(w, z, w, w); }
	inline nsvec4<T> wwxx() const { return nsvec4<T>(w, w, x, x); }
	inline nsvec4<T> wwxy() const { return nsvec4<T>(w, w, x, y); }
	inline nsvec4<T> wwxz() const { return nsvec4<T>(w, w, x, z); }
	inline nsvec4<T> wwxw() const { return nsvec4<T>(w, w, x, w); }
	inline nsvec4<T> wwyx() const { return nsvec4<T>(w, w, y, x); }
	inline nsvec4<T> wwyy() const { return nsvec4<T>(w, w, y, y); }
	inline nsvec4<T> wwyz() const { return nsvec4<T>(w, w, y, z); }
	inline nsvec4<T> wwyw() const { return nsvec4<T>(w, w, y, w); }
	inline nsvec4<T> wwzx() const { return nsvec4<T>(w, w, z, x); }
	inline nsvec4<T> wwzy() const { return nsvec4<T>(w, w, z, y); }
	inline nsvec4<T> wwzz() const { return nsvec4<T>(w, w, z, z); }
	inline nsvec4<T> wwzw() const { return nsvec4<T>(w, w, z, w); }
	inline nsvec4<T> wwwx() const { return nsvec4<T>(w, w, w, x); }
	inline nsvec4<T> wwwy() const { return nsvec4<T>(w, w, w, y); }
	inline nsvec4<T> wwwz() const { return nsvec4<T>(w, w, w, z); }
	inline nsvec4<T> wwww() const { return nsvec4<T>(w, w, w, w); }

	inline nsvec4<T> rrrr() const { return nsvec4<T>(r, r, r, r); }
	inline nsvec4<T> rrrg() const { return nsvec4<T>(r, r, r, g); }
	inline nsvec4<T> rrrb() const { return nsvec4<T>(r, r, r, b); }
	inline nsvec4<T> rrra() const { return nsvec4<T>(r, r, r, a); }
	inline nsvec4<T> rrgr() const { return nsvec4<T>(r, r, g, r); }
	inline nsvec4<T> rrgg() const { return nsvec4<T>(r, r, g, g); }
	inline nsvec4<T> rrgb() const { return nsvec4<T>(r, r, g, b); }
	inline nsvec4<T> rrga() const { return nsvec4<T>(r, r, g, a); }
	inline nsvec4<T> rrbr() const { return nsvec4<T>(r, r, b, r); }
	inline nsvec4<T> rrbg() const { return nsvec4<T>(r, r, b, g); }
	inline nsvec4<T> rrbb() const { return nsvec4<T>(r, r, b, b); }
	inline nsvec4<T> rrba() const { return nsvec4<T>(r, r, b, a); }
	inline nsvec4<T> rrar() const { return nsvec4<T>(r, r, a, r); }
	inline nsvec4<T> rrag() const { return nsvec4<T>(r, r, a, g); }
	inline nsvec4<T> rrab() const { return nsvec4<T>(r, r, a, b); }
	inline nsvec4<T> rraa() const { return nsvec4<T>(r, r, a, a); }
	inline nsvec4<T> rgrr() const { return nsvec4<T>(r, g, r, r); }
	inline nsvec4<T> rgrg() const { return nsvec4<T>(r, g, r, g); }
	inline nsvec4<T> rgrb() const { return nsvec4<T>(r, g, r, b); }
	inline nsvec4<T> rgra() const { return nsvec4<T>(r, g, r, a); }
	inline nsvec4<T> rggr() const { return nsvec4<T>(r, g, g, r); }
	inline nsvec4<T> rggg() const { return nsvec4<T>(r, g, g, g); }
	inline nsvec4<T> rggb() const { return nsvec4<T>(r, g, g, b); }
	inline nsvec4<T> rgga() const { return nsvec4<T>(r, g, g, a); }
	inline nsvec4<T> rgbr() const { return nsvec4<T>(r, g, b, r); }
	inline nsvec4<T> rgbg() const { return nsvec4<T>(r, g, b, g); }
	inline nsvec4<T> rgbb() const { return nsvec4<T>(r, g, b, b); }
	inline nsvec4<T> rgar() const { return nsvec4<T>(r, g, a, r); }
	inline nsvec4<T> rgag() const { return nsvec4<T>(r, g, a, g); }
	inline nsvec4<T> rgab() const { return nsvec4<T>(r, g, a, b); }
	inline nsvec4<T> rgaa() const { return nsvec4<T>(r, g, a, a); }
	inline nsvec4<T> rbrr() const { return nsvec4<T>(r, b, r, r); }
	inline nsvec4<T> rbrg() const { return nsvec4<T>(r, b, r, g); }
	inline nsvec4<T> rbrb() const { return nsvec4<T>(r, b, r, b); }
	inline nsvec4<T> rbra() const { return nsvec4<T>(r, b, r, a); }
	inline nsvec4<T> rbgr() const { return nsvec4<T>(r, b, g, r); }
	inline nsvec4<T> rbgg() const { return nsvec4<T>(r, b, g, g); }
	inline nsvec4<T> rbgb() const { return nsvec4<T>(r, b, g, b); }
	inline nsvec4<T> rbga() const { return nsvec4<T>(r, b, g, a); }
	inline nsvec4<T> rbbr() const { return nsvec4<T>(r, b, b, r); }
	inline nsvec4<T> rbbg() const { return nsvec4<T>(r, b, b, g); }
	inline nsvec4<T> rbbb() const { return nsvec4<T>(r, b, b, b); }
	inline nsvec4<T> rbba() const { return nsvec4<T>(r, b, b, a); }
	inline nsvec4<T> rbar() const { return nsvec4<T>(r, b, a, r); }
	inline nsvec4<T> rbag() const { return nsvec4<T>(r, b, a, g); }
	inline nsvec4<T> rbab() const { return nsvec4<T>(r, b, a, b); }
	inline nsvec4<T> rbaa() const { return nsvec4<T>(r, b, a, a); }
	inline nsvec4<T> rarr() const { return nsvec4<T>(r, a, r, r); }
	inline nsvec4<T> rarg() const { return nsvec4<T>(r, a, r, g); }
	inline nsvec4<T> rarb() const { return nsvec4<T>(r, a, r, b); }
	inline nsvec4<T> rara() const { return nsvec4<T>(r, a, r, a); }
	inline nsvec4<T> ragr() const { return nsvec4<T>(r, a, g, r); }
	inline nsvec4<T> ragg() const { return nsvec4<T>(r, a, g, g); }
	inline nsvec4<T> ragb() const { return nsvec4<T>(r, a, g, b); }
	inline nsvec4<T> raga() const { return nsvec4<T>(r, a, g, a); }
	inline nsvec4<T> rabr() const { return nsvec4<T>(r, a, b, r); }
	inline nsvec4<T> rabg() const { return nsvec4<T>(r, a, b, g); }
	inline nsvec4<T> rabb() const { return nsvec4<T>(r, a, b, b); }
	inline nsvec4<T> raba() const { return nsvec4<T>(r, a, b, a); }
	inline nsvec4<T> raar() const { return nsvec4<T>(r, a, a, r); }
	inline nsvec4<T> raag() const { return nsvec4<T>(r, a, a, g); }
	inline nsvec4<T> raab() const { return nsvec4<T>(r, a, a, b); }
	inline nsvec4<T> raaa() const { return nsvec4<T>(r, a, a, a); }
	inline nsvec4<T> grrr() const { return nsvec4<T>(g, r, r, r); }
	inline nsvec4<T> grrg() const { return nsvec4<T>(g, r, r, g); }
	inline nsvec4<T> grrb() const { return nsvec4<T>(g, r, r, b); }
	inline nsvec4<T> grra() const { return nsvec4<T>(g, r, r, a); }
	inline nsvec4<T> grgr() const { return nsvec4<T>(g, r, g, r); }
	inline nsvec4<T> grgg() const { return nsvec4<T>(g, r, g, g); }
	inline nsvec4<T> grgb() const { return nsvec4<T>(g, r, g, b); }
	inline nsvec4<T> grga() const { return nsvec4<T>(g, r, g, a); }
	inline nsvec4<T> grbr() const { return nsvec4<T>(g, r, b, r); }
	inline nsvec4<T> grbg() const { return nsvec4<T>(g, r, b, g); }
	inline nsvec4<T> grbb() const { return nsvec4<T>(g, r, b, b); }
	inline nsvec4<T> grba() const { return nsvec4<T>(g, r, b, a); }
	inline nsvec4<T> grar() const { return nsvec4<T>(g, r, a, r); }
	inline nsvec4<T> grag() const { return nsvec4<T>(g, r, a, g); }
	inline nsvec4<T> grab() const { return nsvec4<T>(g, r, a, b); }
	inline nsvec4<T> graa() const { return nsvec4<T>(g, r, a, a); }
	inline nsvec4<T> ggrr() const { return nsvec4<T>(g, g, r, r); }
	inline nsvec4<T> ggrg() const { return nsvec4<T>(g, g, r, g); }
	inline nsvec4<T> ggrb() const { return nsvec4<T>(g, g, r, b); }
	inline nsvec4<T> ggra() const { return nsvec4<T>(g, g, r, a); }
	inline nsvec4<T> gggr() const { return nsvec4<T>(g, g, g, r); }
	inline nsvec4<T> gggg() const { return nsvec4<T>(g, g, g, g); }
	inline nsvec4<T> gggb() const { return nsvec4<T>(g, g, g, b); }
	inline nsvec4<T> ggga() const { return nsvec4<T>(g, g, g, a); }
	inline nsvec4<T> ggbr() const { return nsvec4<T>(g, g, b, r); }
	inline nsvec4<T> ggbg() const { return nsvec4<T>(g, g, b, g); }
	inline nsvec4<T> ggbb() const { return nsvec4<T>(g, g, b, b); }
	inline nsvec4<T> ggba() const { return nsvec4<T>(g, g, b, a); }
	inline nsvec4<T> ggar() const { return nsvec4<T>(g, g, a, r); }
	inline nsvec4<T> ggag() const { return nsvec4<T>(g, g, a, g); }
	inline nsvec4<T> ggab() const { return nsvec4<T>(g, g, a, b); }
	inline nsvec4<T> ggaa() const { return nsvec4<T>(g, g, a, a); }
	inline nsvec4<T> gbrr() const { return nsvec4<T>(g, b, r, r); }
	inline nsvec4<T> gbrg() const { return nsvec4<T>(g, b, r, g); }
	inline nsvec4<T> gbrb() const { return nsvec4<T>(g, b, r, b); }
	inline nsvec4<T> gbra() const { return nsvec4<T>(g, b, r, a); }
	inline nsvec4<T> gbgr() const { return nsvec4<T>(g, b, g, r); }
	inline nsvec4<T> gbgg() const { return nsvec4<T>(g, b, g, g); }
	inline nsvec4<T> gbgb() const { return nsvec4<T>(g, b, g, b); }
	inline nsvec4<T> gbga() const { return nsvec4<T>(g, b, g, a); }
	inline nsvec4<T> gbbr() const { return nsvec4<T>(g, b, b, r); }
	inline nsvec4<T> gbbg() const { return nsvec4<T>(g, b, b, g); }
	inline nsvec4<T> gbbb() const { return nsvec4<T>(g, b, b, b); }
	inline nsvec4<T> gbba() const { return nsvec4<T>(g, b, b, a); }
	inline nsvec4<T> gbar() const { return nsvec4<T>(g, b, a, r); }
	inline nsvec4<T> gbag() const { return nsvec4<T>(g, b, a, g); }
	inline nsvec4<T> gbab() const { return nsvec4<T>(g, b, a, b); }
	inline nsvec4<T> gbaa() const { return nsvec4<T>(g, b, a, a); }
	inline nsvec4<T> garr() const { return nsvec4<T>(g, a, r, r); }
	inline nsvec4<T> garg() const { return nsvec4<T>(g, a, r, g); }
	inline nsvec4<T> garb() const { return nsvec4<T>(g, a, r, b); }
	inline nsvec4<T> gara() const { return nsvec4<T>(g, a, r, a); }
	inline nsvec4<T> gagr() const { return nsvec4<T>(g, a, g, r); }
	inline nsvec4<T> gagg() const { return nsvec4<T>(g, a, g, g); }
	inline nsvec4<T> gagb() const { return nsvec4<T>(g, a, g, b); }
	inline nsvec4<T> gaga() const { return nsvec4<T>(g, a, g, a); }
	inline nsvec4<T> gabr() const { return nsvec4<T>(g, a, b, r); }
	inline nsvec4<T> gabg() const { return nsvec4<T>(g, a, b, g); }
	inline nsvec4<T> gabb() const { return nsvec4<T>(g, a, b, b); }
	inline nsvec4<T> gaba() const { return nsvec4<T>(g, a, b, a); }
	inline nsvec4<T> gaar() const { return nsvec4<T>(g, a, a, r); }
	inline nsvec4<T> gaag() const { return nsvec4<T>(g, a, a, g); }
	inline nsvec4<T> gaab() const { return nsvec4<T>(g, a, a, b); }
	inline nsvec4<T> gaaa() const { return nsvec4<T>(g, a, a, a); }
	inline nsvec4<T> brrr() const { return nsvec4<T>(b, r, r, r); }
	inline nsvec4<T> brrg() const { return nsvec4<T>(b, r, r, g); }
	inline nsvec4<T> brrb() const { return nsvec4<T>(b, r, r, b); }
	inline nsvec4<T> brra() const { return nsvec4<T>(b, r, r, a); }
	inline nsvec4<T> brgr() const { return nsvec4<T>(b, r, g, r); }
	inline nsvec4<T> brgg() const { return nsvec4<T>(b, r, g, g); }
	inline nsvec4<T> brgb() const { return nsvec4<T>(b, r, g, b); }
	inline nsvec4<T> brga() const { return nsvec4<T>(b, r, g, a); }
	inline nsvec4<T> brbr() const { return nsvec4<T>(b, r, b, r); }
	inline nsvec4<T> brbg() const { return nsvec4<T>(b, r, b, g); }
	inline nsvec4<T> brbb() const { return nsvec4<T>(b, r, b, b); }
	inline nsvec4<T> brba() const { return nsvec4<T>(b, r, b, a); }
	inline nsvec4<T> brar() const { return nsvec4<T>(b, r, a, r); }
	inline nsvec4<T> brag() const { return nsvec4<T>(b, r, a, g); }
	inline nsvec4<T> brab() const { return nsvec4<T>(b, r, a, b); }
	inline nsvec4<T> braa() const { return nsvec4<T>(b, r, a, a); }
	inline nsvec4<T> bgrr() const { return nsvec4<T>(b, g, r, r); }
	inline nsvec4<T> bgrg() const { return nsvec4<T>(b, g, r, g); }
	inline nsvec4<T> bgrb() const { return nsvec4<T>(b, g, r, b); }
	inline nsvec4<T> bgra() const { return nsvec4<T>(b, g, r, a); }
	inline nsvec4<T> bggr() const { return nsvec4<T>(b, g, g, r); }
	inline nsvec4<T> bggg() const { return nsvec4<T>(b, g, g, g); }
	inline nsvec4<T> bggb() const { return nsvec4<T>(b, g, g, b); }
	inline nsvec4<T> bgga() const { return nsvec4<T>(b, g, g, a); }
	inline nsvec4<T> bgbr() const { return nsvec4<T>(b, g, b, r); }
	inline nsvec4<T> bgbg() const { return nsvec4<T>(b, g, b, g); }
	inline nsvec4<T> bgbb() const { return nsvec4<T>(b, g, b, b); }
	inline nsvec4<T> bgba() const { return nsvec4<T>(b, g, b, a); }
	inline nsvec4<T> bgar() const { return nsvec4<T>(b, g, a, r); }
	inline nsvec4<T> bgag() const { return nsvec4<T>(b, g, a, g); }
	inline nsvec4<T> bgab() const { return nsvec4<T>(b, g, a, b); }
	inline nsvec4<T> bgaa() const { return nsvec4<T>(b, g, a, a); }
	inline nsvec4<T> bbrr() const { return nsvec4<T>(b, b, r, r); }
	inline nsvec4<T> bbrg() const { return nsvec4<T>(b, b, r, g); }
	inline nsvec4<T> bbrb() const { return nsvec4<T>(b, b, r, b); }
	inline nsvec4<T> bbra() const { return nsvec4<T>(b, b, r, a); }
	inline nsvec4<T> bbgr() const { return nsvec4<T>(b, b, g, r); }
	inline nsvec4<T> bbgg() const { return nsvec4<T>(b, b, g, g); }
	inline nsvec4<T> bbgb() const { return nsvec4<T>(b, b, g, b); }
	inline nsvec4<T> bbga() const { return nsvec4<T>(b, b, g, a); }
	inline nsvec4<T> bbbr() const { return nsvec4<T>(b, b, b, r); }
	inline nsvec4<T> bbbg() const { return nsvec4<T>(b, b, b, g); }
	inline nsvec4<T> bbbb() const { return nsvec4<T>(b, b, b, b); }
	inline nsvec4<T> bbba() const { return nsvec4<T>(b, b, b, a); }
	inline nsvec4<T> bbar() const { return nsvec4<T>(b, b, a, r); }
	inline nsvec4<T> bbag() const { return nsvec4<T>(b, b, a, g); }
	inline nsvec4<T> bbab() const { return nsvec4<T>(b, b, a, b); }
	inline nsvec4<T> bbaa() const { return nsvec4<T>(b, b, a, a); }
	inline nsvec4<T> barr() const { return nsvec4<T>(b, a, r, r); }
	inline nsvec4<T> barg() const { return nsvec4<T>(b, a, r, g); }
	inline nsvec4<T> barb() const { return nsvec4<T>(b, a, r, b); }
	inline nsvec4<T> bara() const { return nsvec4<T>(b, a, r, a); }
	inline nsvec4<T> bagr() const { return nsvec4<T>(b, a, g, r); }
	inline nsvec4<T> bagg() const { return nsvec4<T>(b, a, g, g); }
	inline nsvec4<T> bagb() const { return nsvec4<T>(b, a, g, b); }
	inline nsvec4<T> baga() const { return nsvec4<T>(b, a, g, a); }
	inline nsvec4<T> babr() const { return nsvec4<T>(b, a, b, r); }
	inline nsvec4<T> babg() const { return nsvec4<T>(b, a, b, g); }
	inline nsvec4<T> babb() const { return nsvec4<T>(b, a, b, b); }
	inline nsvec4<T> baba() const { return nsvec4<T>(b, a, b, a); }
	inline nsvec4<T> baar() const { return nsvec4<T>(b, a, a, r); }
	inline nsvec4<T> baag() const { return nsvec4<T>(b, a, a, g); }
	inline nsvec4<T> baab() const { return nsvec4<T>(b, a, a, b); }
	inline nsvec4<T> baaa() const { return nsvec4<T>(b, a, a, a); }
	inline nsvec4<T> arrr() const { return nsvec4<T>(a, r, r, r); }
	inline nsvec4<T> arrg() const { return nsvec4<T>(a, r, r, g); }
	inline nsvec4<T> arrb() const { return nsvec4<T>(a, r, r, b); }
	inline nsvec4<T> arra() const { return nsvec4<T>(a, r, r, a); }
	inline nsvec4<T> argr() const { return nsvec4<T>(a, r, g, r); }
	inline nsvec4<T> argg() const { return nsvec4<T>(a, r, g, g); }
	inline nsvec4<T> argb() const { return nsvec4<T>(a, r, g, b); }
	inline nsvec4<T> arga() const { return nsvec4<T>(a, r, g, a); }
	inline nsvec4<T> arbr() const { return nsvec4<T>(a, r, b, r); }
	inline nsvec4<T> arbg() const { return nsvec4<T>(a, r, b, g); }
	inline nsvec4<T> arbb() const { return nsvec4<T>(a, r, b, b); }
	inline nsvec4<T> arba() const { return nsvec4<T>(a, r, b, a); }
	inline nsvec4<T> arar() const { return nsvec4<T>(a, r, a, r); }
	inline nsvec4<T> arag() const { return nsvec4<T>(a, r, a, g); }
	inline nsvec4<T> arab() const { return nsvec4<T>(a, r, a, b); }
	inline nsvec4<T> araa() const { return nsvec4<T>(a, r, a, a); }
	inline nsvec4<T> agrr() const { return nsvec4<T>(a, g, r, r); }
	inline nsvec4<T> agrg() const { return nsvec4<T>(a, g, r, g); }
	inline nsvec4<T> agrb() const { return nsvec4<T>(a, g, r, b); }
	inline nsvec4<T> agra() const { return nsvec4<T>(a, g, r, a); }
	inline nsvec4<T> aggr() const { return nsvec4<T>(a, g, g, r); }
	inline nsvec4<T> aggg() const { return nsvec4<T>(a, g, g, g); }
	inline nsvec4<T> aggb() const { return nsvec4<T>(a, g, g, b); }
	inline nsvec4<T> agga() const { return nsvec4<T>(a, g, g, a); }
	inline nsvec4<T> agbr() const { return nsvec4<T>(a, g, b, r); }
	inline nsvec4<T> agbg() const { return nsvec4<T>(a, g, b, g); }
	inline nsvec4<T> agbb() const { return nsvec4<T>(a, g, b, b); }
	inline nsvec4<T> agba() const { return nsvec4<T>(a, g, b, a); }
	inline nsvec4<T> agar() const { return nsvec4<T>(a, g, a, r); }
	inline nsvec4<T> agag() const { return nsvec4<T>(a, g, a, g); }
	inline nsvec4<T> agab() const { return nsvec4<T>(a, g, a, b); }
	inline nsvec4<T> agaa() const { return nsvec4<T>(a, g, a, a); }
	inline nsvec4<T> abrr() const { return nsvec4<T>(a, b, r, r); }
	inline nsvec4<T> abrg() const { return nsvec4<T>(a, b, r, g); }
	inline nsvec4<T> abrb() const { return nsvec4<T>(a, b, r, b); }
	inline nsvec4<T> abra() const { return nsvec4<T>(a, b, r, a); }
	inline nsvec4<T> abgr() const { return nsvec4<T>(a, b, g, r); }
	inline nsvec4<T> abgg() const { return nsvec4<T>(a, b, g, g); }
	inline nsvec4<T> abgb() const { return nsvec4<T>(a, b, g, b); }
	inline nsvec4<T> abga() const { return nsvec4<T>(a, b, g, a); }
	inline nsvec4<T> abbr() const { return nsvec4<T>(a, b, b, r); }
	inline nsvec4<T> abbg() const { return nsvec4<T>(a, b, b, g); }
	inline nsvec4<T> abbb() const { return nsvec4<T>(a, b, b, b); }
	inline nsvec4<T> abba() const { return nsvec4<T>(a, b, b, a); }
	inline nsvec4<T> abar() const { return nsvec4<T>(a, b, a, r); }
	inline nsvec4<T> abag() const { return nsvec4<T>(a, b, a, g); }
	inline nsvec4<T> abab() const { return nsvec4<T>(a, b, a, b); }
	inline nsvec4<T> abaa() const { return nsvec4<T>(a, b, a, a); }
	inline nsvec4<T> aarr() const { return nsvec4<T>(a, a, r, r); }
	inline nsvec4<T> aarg() const { return nsvec4<T>(a, a, r, g); }
	inline nsvec4<T> aarb() const { return nsvec4<T>(a, a, r, b); }
	inline nsvec4<T> aara() const { return nsvec4<T>(a, a, r, a); }
	inline nsvec4<T> aagr() const { return nsvec4<T>(a, a, g, r); }
	inline nsvec4<T> aagg() const { return nsvec4<T>(a, a, g, g); }
	inline nsvec4<T> aagb() const { return nsvec4<T>(a, a, g, b); }
	inline nsvec4<T> aaga() const { return nsvec4<T>(a, a, g, a); }
	inline nsvec4<T> aabr() const { return nsvec4<T>(a, a, b, r); }
	inline nsvec4<T> aabg() const { return nsvec4<T>(a, a, b, g); }
	inline nsvec4<T> aabb() const { return nsvec4<T>(a, a, b, b); }
	inline nsvec4<T> aaba() const { return nsvec4<T>(a, a, b, a); }
	inline nsvec4<T> aaar() const { return nsvec4<T>(a, a, a, r); }
	inline nsvec4<T> aaag() const { return nsvec4<T>(a, a, a, g); }
	inline nsvec4<T> aaab() const { return nsvec4<T>(a, a, a, b); }
	inline nsvec4<T> aaaa() const { return nsvec4<T>(a, a, a, a); }

	inline nsvec4<T> ssss() const { return nsvec4<T>(s, s, s, s); }
	inline nsvec4<T> ssst() const { return nsvec4<T>(s, s, s, t); }
	inline nsvec4<T> sssp() const { return nsvec4<T>(s, s, s, p); }
	inline nsvec4<T> sssq() const { return nsvec4<T>(s, s, s, q); }
	inline nsvec4<T> ssts() const { return nsvec4<T>(s, s, t, s); }
	inline nsvec4<T> sstt() const { return nsvec4<T>(s, s, t, t); }
	inline nsvec4<T> sstp() const { return nsvec4<T>(s, s, t, p); }
	inline nsvec4<T> sstq() const { return nsvec4<T>(s, s, t, q); }
	inline nsvec4<T> ssps() const { return nsvec4<T>(s, s, p, s); }
	inline nsvec4<T> sspt() const { return nsvec4<T>(s, s, p, t); }
	inline nsvec4<T> sspp() const { return nsvec4<T>(s, s, p, p); }
	inline nsvec4<T> sspq() const { return nsvec4<T>(s, s, p, q); }
	inline nsvec4<T> ssqs() const { return nsvec4<T>(s, s, q, s); }
	inline nsvec4<T> ssqt() const { return nsvec4<T>(s, s, q, t); }
	inline nsvec4<T> ssqp() const { return nsvec4<T>(s, s, q, p); }
	inline nsvec4<T> ssqq() const { return nsvec4<T>(s, s, q, q); }
	inline nsvec4<T> stss() const { return nsvec4<T>(s, t, s, s); }
	inline nsvec4<T> stst() const { return nsvec4<T>(s, t, s, t); }
	inline nsvec4<T> stsp() const { return nsvec4<T>(s, t, s, p); }
	inline nsvec4<T> stsq() const { return nsvec4<T>(s, t, s, q); }
	inline nsvec4<T> stts() const { return nsvec4<T>(s, t, t, s); }
	inline nsvec4<T> sttt() const { return nsvec4<T>(s, t, t, t); }
	inline nsvec4<T> sttp() const { return nsvec4<T>(s, t, t, p); }
	inline nsvec4<T> sttq() const { return nsvec4<T>(s, t, t, q); }
	inline nsvec4<T> stps() const { return nsvec4<T>(s, t, p, s); }
	inline nsvec4<T> stpt() const { return nsvec4<T>(s, t, p, t); }
	inline nsvec4<T> stpp() const { return nsvec4<T>(s, t, p, p); }
	inline nsvec4<T> stqs() const { return nsvec4<T>(s, t, q, s); }
	inline nsvec4<T> stqt() const { return nsvec4<T>(s, t, q, t); }
	inline nsvec4<T> stqp() const { return nsvec4<T>(s, t, q, p); }
	inline nsvec4<T> stqq() const { return nsvec4<T>(s, t, q, q); }
	inline nsvec4<T> spss() const { return nsvec4<T>(s, p, s, s); }
	inline nsvec4<T> spst() const { return nsvec4<T>(s, p, s, t); }
	inline nsvec4<T> spsp() const { return nsvec4<T>(s, p, s, p); }
	inline nsvec4<T> spsq() const { return nsvec4<T>(s, p, s, q); }
	inline nsvec4<T> spts() const { return nsvec4<T>(s, p, t, s); }
	inline nsvec4<T> sptt() const { return nsvec4<T>(s, p, t, t); }
	inline nsvec4<T> sptp() const { return nsvec4<T>(s, p, t, p); }
	inline nsvec4<T> sptq() const { return nsvec4<T>(s, p, t, q); }
	inline nsvec4<T> spps() const { return nsvec4<T>(s, p, p, s); }
	inline nsvec4<T> sppt() const { return nsvec4<T>(s, p, p, t); }
	inline nsvec4<T> sppp() const { return nsvec4<T>(s, p, p, p); }
	inline nsvec4<T> sppq() const { return nsvec4<T>(s, p, p, q); }
	inline nsvec4<T> spqs() const { return nsvec4<T>(s, p, q, s); }
	inline nsvec4<T> spqt() const { return nsvec4<T>(s, p, q, t); }
	inline nsvec4<T> spqp() const { return nsvec4<T>(s, p, q, p); }
	inline nsvec4<T> spqq() const { return nsvec4<T>(s, p, q, q); }
	inline nsvec4<T> sqss() const { return nsvec4<T>(s, q, s, s); }
	inline nsvec4<T> sqst() const { return nsvec4<T>(s, q, s, t); }
	inline nsvec4<T> sqsp() const { return nsvec4<T>(s, q, s, p); }
	inline nsvec4<T> sqsq() const { return nsvec4<T>(s, q, s, q); }
	inline nsvec4<T> sqts() const { return nsvec4<T>(s, q, t, s); }
	inline nsvec4<T> sqtt() const { return nsvec4<T>(s, q, t, t); }
	inline nsvec4<T> sqtp() const { return nsvec4<T>(s, q, t, p); }
	inline nsvec4<T> sqtq() const { return nsvec4<T>(s, q, t, q); }
	inline nsvec4<T> sqps() const { return nsvec4<T>(s, q, p, s); }
	inline nsvec4<T> sqpt() const { return nsvec4<T>(s, q, p, t); }
	inline nsvec4<T> sqpp() const { return nsvec4<T>(s, q, p, p); }
	inline nsvec4<T> sqpq() const { return nsvec4<T>(s, q, p, q); }
	inline nsvec4<T> sqqs() const { return nsvec4<T>(s, q, q, s); }
	inline nsvec4<T> sqqt() const { return nsvec4<T>(s, q, q, t); }
	inline nsvec4<T> sqqp() const { return nsvec4<T>(s, q, q, p); }
	inline nsvec4<T> sqqq() const { return nsvec4<T>(s, q, q, q); }
	inline nsvec4<T> tsss() const { return nsvec4<T>(t, s, s, s); }
	inline nsvec4<T> tsst() const { return nsvec4<T>(t, s, s, t); }
	inline nsvec4<T> tssp() const { return nsvec4<T>(t, s, s, p); }
	inline nsvec4<T> tssq() const { return nsvec4<T>(t, s, s, q); }
	inline nsvec4<T> tsts() const { return nsvec4<T>(t, s, t, s); }
	inline nsvec4<T> tstt() const { return nsvec4<T>(t, s, t, t); }
	inline nsvec4<T> tstp() const { return nsvec4<T>(t, s, t, p); }
	inline nsvec4<T> tstq() const { return nsvec4<T>(t, s, t, q); }
	inline nsvec4<T> tsps() const { return nsvec4<T>(t, s, p, s); }
	inline nsvec4<T> tspt() const { return nsvec4<T>(t, s, p, t); }
	inline nsvec4<T> tspp() const { return nsvec4<T>(t, s, p, p); }
	inline nsvec4<T> tspq() const { return nsvec4<T>(t, s, p, q); }
	inline nsvec4<T> tsqs() const { return nsvec4<T>(t, s, q, s); }
	inline nsvec4<T> tsqt() const { return nsvec4<T>(t, s, q, t); }
	inline nsvec4<T> tsqp() const { return nsvec4<T>(t, s, q, p); }
	inline nsvec4<T> tsqq() const { return nsvec4<T>(t, s, q, q); }
	inline nsvec4<T> ttss() const { return nsvec4<T>(t, t, s, s); }
	inline nsvec4<T> ttst() const { return nsvec4<T>(t, t, s, t); }
	inline nsvec4<T> ttsp() const { return nsvec4<T>(t, t, s, p); }
	inline nsvec4<T> ttsq() const { return nsvec4<T>(t, t, s, q); }
	inline nsvec4<T> ttts() const { return nsvec4<T>(t, t, t, s); }
	inline nsvec4<T> tttt() const { return nsvec4<T>(t, t, t, t); }
	inline nsvec4<T> tttp() const { return nsvec4<T>(t, t, t, p); }
	inline nsvec4<T> tttq() const { return nsvec4<T>(t, t, t, q); }
	inline nsvec4<T> ttps() const { return nsvec4<T>(t, t, p, s); }
	inline nsvec4<T> ttpt() const { return nsvec4<T>(t, t, p, t); }
	inline nsvec4<T> ttpp() const { return nsvec4<T>(t, t, p, p); }
	inline nsvec4<T> ttpq() const { return nsvec4<T>(t, t, p, q); }
	inline nsvec4<T> ttqs() const { return nsvec4<T>(t, t, q, s); }
	inline nsvec4<T> ttqt() const { return nsvec4<T>(t, t, q, t); }
	inline nsvec4<T> ttqp() const { return nsvec4<T>(t, t, q, p); }
	inline nsvec4<T> ttqq() const { return nsvec4<T>(t, t, q, q); }
	inline nsvec4<T> tpss() const { return nsvec4<T>(t, p, s, s); }
	inline nsvec4<T> tpst() const { return nsvec4<T>(t, p, s, t); }
	inline nsvec4<T> tpsp() const { return nsvec4<T>(t, p, s, p); }
	inline nsvec4<T> tpsq() const { return nsvec4<T>(t, p, s, q); }
	inline nsvec4<T> tpts() const { return nsvec4<T>(t, p, t, s); }
	inline nsvec4<T> tptt() const { return nsvec4<T>(t, p, t, t); }
	inline nsvec4<T> tptp() const { return nsvec4<T>(t, p, t, p); }
	inline nsvec4<T> tptq() const { return nsvec4<T>(t, p, t, q); }
	inline nsvec4<T> tpps() const { return nsvec4<T>(t, p, p, s); }
	inline nsvec4<T> tppt() const { return nsvec4<T>(t, p, p, t); }
	inline nsvec4<T> tppp() const { return nsvec4<T>(t, p, p, p); }
	inline nsvec4<T> tppq() const { return nsvec4<T>(t, p, p, q); }
	inline nsvec4<T> tpqs() const { return nsvec4<T>(t, p, q, s); }
	inline nsvec4<T> tpqt() const { return nsvec4<T>(t, p, q, t); }
	inline nsvec4<T> tpqp() const { return nsvec4<T>(t, p, q, p); }
	inline nsvec4<T> tpqq() const { return nsvec4<T>(t, p, q, q); }
	inline nsvec4<T> tqss() const { return nsvec4<T>(t, q, s, s); }
	inline nsvec4<T> tqst() const { return nsvec4<T>(t, q, s, t); }
	inline nsvec4<T> tqsp() const { return nsvec4<T>(t, q, s, p); }
	inline nsvec4<T> tqsq() const { return nsvec4<T>(t, q, s, q); }
	inline nsvec4<T> tqts() const { return nsvec4<T>(t, q, t, s); }
	inline nsvec4<T> tqtt() const { return nsvec4<T>(t, q, t, t); }
	inline nsvec4<T> tqtp() const { return nsvec4<T>(t, q, t, p); }
	inline nsvec4<T> tqtq() const { return nsvec4<T>(t, q, t, q); }
	inline nsvec4<T> tqps() const { return nsvec4<T>(t, q, p, s); }
	inline nsvec4<T> tqpt() const { return nsvec4<T>(t, q, p, t); }
	inline nsvec4<T> tqpp() const { return nsvec4<T>(t, q, p, p); }
	inline nsvec4<T> tqpq() const { return nsvec4<T>(t, q, p, q); }
	inline nsvec4<T> tqqs() const { return nsvec4<T>(t, q, q, s); }
	inline nsvec4<T> tqqt() const { return nsvec4<T>(t, q, q, t); }
	inline nsvec4<T> tqqp() const { return nsvec4<T>(t, q, q, p); }
	inline nsvec4<T> tqqq() const { return nsvec4<T>(t, q, q, q); }
	inline nsvec4<T> psss() const { return nsvec4<T>(p, s, s, s); }
	inline nsvec4<T> psst() const { return nsvec4<T>(p, s, s, t); }
	inline nsvec4<T> pssp() const { return nsvec4<T>(p, s, s, p); }
	inline nsvec4<T> pssq() const { return nsvec4<T>(p, s, s, q); }
	inline nsvec4<T> psts() const { return nsvec4<T>(p, s, t, s); }
	inline nsvec4<T> pstt() const { return nsvec4<T>(p, s, t, t); }
	inline nsvec4<T> pstp() const { return nsvec4<T>(p, s, t, p); }
	inline nsvec4<T> pstq() const { return nsvec4<T>(p, s, t, q); }
	inline nsvec4<T> psps() const { return nsvec4<T>(p, s, p, s); }
	inline nsvec4<T> pspt() const { return nsvec4<T>(p, s, p, t); }
	inline nsvec4<T> pspp() const { return nsvec4<T>(p, s, p, p); }
	inline nsvec4<T> pspq() const { return nsvec4<T>(p, s, p, q); }
	inline nsvec4<T> psqs() const { return nsvec4<T>(p, s, q, s); }
	inline nsvec4<T> psqt() const { return nsvec4<T>(p, s, q, t); }
	inline nsvec4<T> psqp() const { return nsvec4<T>(p, s, q, p); }
	inline nsvec4<T> psqq() const { return nsvec4<T>(p, s, q, q); }
	inline nsvec4<T> ptss() const { return nsvec4<T>(p, t, s, s); }
	inline nsvec4<T> ptst() const { return nsvec4<T>(p, t, s, t); }
	inline nsvec4<T> ptsp() const { return nsvec4<T>(p, t, s, p); }
	inline nsvec4<T> ptsq() const { return nsvec4<T>(p, t, s, q); }
	inline nsvec4<T> ptts() const { return nsvec4<T>(p, t, t, s); }
	inline nsvec4<T> pttt() const { return nsvec4<T>(p, t, t, t); }
	inline nsvec4<T> pttp() const { return nsvec4<T>(p, t, t, p); }
	inline nsvec4<T> pttq() const { return nsvec4<T>(p, t, t, q); }
	inline nsvec4<T> ptps() const { return nsvec4<T>(p, t, p, s); }
	inline nsvec4<T> ptpt() const { return nsvec4<T>(p, t, p, t); }
	inline nsvec4<T> ptpp() const { return nsvec4<T>(p, t, p, p); }
	inline nsvec4<T> ptpq() const { return nsvec4<T>(p, t, p, q); }
	inline nsvec4<T> ptqs() const { return nsvec4<T>(p, t, q, s); }
	inline nsvec4<T> ptqt() const { return nsvec4<T>(p, t, q, t); }
	inline nsvec4<T> ptqp() const { return nsvec4<T>(p, t, q, p); }
	inline nsvec4<T> ptqq() const { return nsvec4<T>(p, t, q, q); }
	inline nsvec4<T> ppss() const { return nsvec4<T>(p, p, s, s); }
	inline nsvec4<T> ppst() const { return nsvec4<T>(p, p, s, t); }
	inline nsvec4<T> ppsp() const { return nsvec4<T>(p, p, s, p); }
	inline nsvec4<T> ppsq() const { return nsvec4<T>(p, p, s, q); }
	inline nsvec4<T> ppts() const { return nsvec4<T>(p, p, t, s); }
	inline nsvec4<T> pptt() const { return nsvec4<T>(p, p, t, t); }
	inline nsvec4<T> pptp() const { return nsvec4<T>(p, p, t, p); }
	inline nsvec4<T> pptq() const { return nsvec4<T>(p, p, t, q); }
	inline nsvec4<T> ppps() const { return nsvec4<T>(p, p, p, s); }
	inline nsvec4<T> pppt() const { return nsvec4<T>(p, p, p, t); }
	inline nsvec4<T> pppp() const { return nsvec4<T>(p, p, p, p); }
	inline nsvec4<T> pppq() const { return nsvec4<T>(p, p, p, q); }
	inline nsvec4<T> ppqs() const { return nsvec4<T>(p, p, q, s); }
	inline nsvec4<T> ppqt() const { return nsvec4<T>(p, p, q, t); }
	inline nsvec4<T> ppqp() const { return nsvec4<T>(p, p, q, p); }
	inline nsvec4<T> ppqq() const { return nsvec4<T>(p, p, q, q); }
	inline nsvec4<T> pqss() const { return nsvec4<T>(p, q, s, s); }
	inline nsvec4<T> pqst() const { return nsvec4<T>(p, q, s, t); }
	inline nsvec4<T> pqsp() const { return nsvec4<T>(p, q, s, p); }
	inline nsvec4<T> pqsq() const { return nsvec4<T>(p, q, s, q); }
	inline nsvec4<T> pqts() const { return nsvec4<T>(p, q, t, s); }
	inline nsvec4<T> pqtt() const { return nsvec4<T>(p, q, t, t); }
	inline nsvec4<T> pqtp() const { return nsvec4<T>(p, q, t, p); }
	inline nsvec4<T> pqtq() const { return nsvec4<T>(p, q, t, q); }
	inline nsvec4<T> pqps() const { return nsvec4<T>(p, q, p, s); }
	inline nsvec4<T> pqpt() const { return nsvec4<T>(p, q, p, t); }
	inline nsvec4<T> pqpp() const { return nsvec4<T>(p, q, p, p); }
	inline nsvec4<T> pqpq() const { return nsvec4<T>(p, q, p, q); }
	inline nsvec4<T> pqqs() const { return nsvec4<T>(p, q, q, s); }
	inline nsvec4<T> pqqt() const { return nsvec4<T>(p, q, q, t); }
	inline nsvec4<T> pqqp() const { return nsvec4<T>(p, q, q, p); }
	inline nsvec4<T> pqqq() const { return nsvec4<T>(p, q, q, q); }
	inline nsvec4<T> qsss() const { return nsvec4<T>(q, s, s, s); }
	inline nsvec4<T> qsst() const { return nsvec4<T>(q, s, s, t); }
	inline nsvec4<T> qssp() const { return nsvec4<T>(q, s, s, p); }
	inline nsvec4<T> qssq() const { return nsvec4<T>(q, s, s, q); }
	inline nsvec4<T> qsts() const { return nsvec4<T>(q, s, t, s); }
	inline nsvec4<T> qstt() const { return nsvec4<T>(q, s, t, t); }
	inline nsvec4<T> qstp() const { return nsvec4<T>(q, s, t, p); }
	inline nsvec4<T> qstq() const { return nsvec4<T>(q, s, t, q); }
	inline nsvec4<T> qsps() const { return nsvec4<T>(q, s, p, s); }
	inline nsvec4<T> qspt() const { return nsvec4<T>(q, s, p, t); }
	inline nsvec4<T> qspp() const { return nsvec4<T>(q, s, p, p); }
	inline nsvec4<T> qspq() const { return nsvec4<T>(q, s, p, q); }
	inline nsvec4<T> qsqs() const { return nsvec4<T>(q, s, q, s); }
	inline nsvec4<T> qsqt() const { return nsvec4<T>(q, s, q, t); }
	inline nsvec4<T> qsqp() const { return nsvec4<T>(q, s, q, p); }
	inline nsvec4<T> qsqq() const { return nsvec4<T>(q, s, q, q); }
	inline nsvec4<T> qtss() const { return nsvec4<T>(q, t, s, s); }
	inline nsvec4<T> qtst() const { return nsvec4<T>(q, t, s, t); }
	inline nsvec4<T> qtsp() const { return nsvec4<T>(q, t, s, p); }
	inline nsvec4<T> qtsq() const { return nsvec4<T>(q, t, s, q); }
	inline nsvec4<T> qtts() const { return nsvec4<T>(q, t, t, s); }
	inline nsvec4<T> qttt() const { return nsvec4<T>(q, t, t, t); }
	inline nsvec4<T> qttp() const { return nsvec4<T>(q, t, t, p); }
	inline nsvec4<T> qttq() const { return nsvec4<T>(q, t, t, q); }
	inline nsvec4<T> qtps() const { return nsvec4<T>(q, t, p, s); }
	inline nsvec4<T> qtpt() const { return nsvec4<T>(q, t, p, t); }
	inline nsvec4<T> qtpp() const { return nsvec4<T>(q, t, p, p); }
	inline nsvec4<T> qtpq() const { return nsvec4<T>(q, t, p, q); }
	inline nsvec4<T> qtqs() const { return nsvec4<T>(q, t, q, s); }
	inline nsvec4<T> qtqt() const { return nsvec4<T>(q, t, q, t); }
	inline nsvec4<T> qtqp() const { return nsvec4<T>(q, t, q, p); }
	inline nsvec4<T> qtqq() const { return nsvec4<T>(q, t, q, q); }
	inline nsvec4<T> qpss() const { return nsvec4<T>(q, p, s, s); }
	inline nsvec4<T> qpst() const { return nsvec4<T>(q, p, s, t); }
	inline nsvec4<T> qpsp() const { return nsvec4<T>(q, p, s, p); }
	inline nsvec4<T> qpsq() const { return nsvec4<T>(q, p, s, q); }
	inline nsvec4<T> qpts() const { return nsvec4<T>(q, p, t, s); }
	inline nsvec4<T> qptt() const { return nsvec4<T>(q, p, t, t); }
	inline nsvec4<T> qptp() const { return nsvec4<T>(q, p, t, p); }
	inline nsvec4<T> qptq() const { return nsvec4<T>(q, p, t, q); }
	inline nsvec4<T> qpps() const { return nsvec4<T>(q, p, p, s); }
	inline nsvec4<T> qppt() const { return nsvec4<T>(q, p, p, t); }
	inline nsvec4<T> qppp() const { return nsvec4<T>(q, p, p, p); }
	inline nsvec4<T> qppq() const { return nsvec4<T>(q, p, p, q); }
	inline nsvec4<T> qpqs() const { return nsvec4<T>(q, p, q, s); }
	inline nsvec4<T> qpqt() const { return nsvec4<T>(q, p, q, t); }
	inline nsvec4<T> qpqp() const { return nsvec4<T>(q, p, q, p); }
	inline nsvec4<T> qpqq() const { return nsvec4<T>(q, p, q, q); }
	inline nsvec4<T> qqss() const { return nsvec4<T>(q, q, s, s); }
	inline nsvec4<T> qqst() const { return nsvec4<T>(q, q, s, t); }
	inline nsvec4<T> qqsp() const { return nsvec4<T>(q, q, s, p); }
	inline nsvec4<T> qqsq() const { return nsvec4<T>(q, q, s, q); }
	inline nsvec4<T> qqts() const { return nsvec4<T>(q, q, t, s); }
	inline nsvec4<T> qqtt() const { return nsvec4<T>(q, q, t, t); }
	inline nsvec4<T> qqtp() const { return nsvec4<T>(q, q, t, p); }
	inline nsvec4<T> qqtq() const { return nsvec4<T>(q, q, t, q); }
	inline nsvec4<T> qqps() const { return nsvec4<T>(q, q, p, s); }
	inline nsvec4<T> qqpt() const { return nsvec4<T>(q, q, p, t); }
	inline nsvec4<T> qqpp() const { return nsvec4<T>(q, q, p, p); }
	inline nsvec4<T> qqpq() const { return nsvec4<T>(q, q, p, q); }
	inline nsvec4<T> qqqs() const { return nsvec4<T>(q, q, q, s); }
	inline nsvec4<T> qqqt() const { return nsvec4<T>(q, q, q, t); }
	inline nsvec4<T> qqqp() const { return nsvec4<T>(q, q, q, p); }
	inline nsvec4<T> qqqq() const { return nsvec4<T>(q, q, q, q); }

	// Vec 3 Swizzles
	inline nsvec3<T> xxx() const { return nsvec3<T>(x, x, x); }
	inline nsvec3<T> xxy() const { return nsvec3<T>(x, x, y); }
	inline nsvec3<T> xxz() const { return nsvec3<T>(x, x, z); }
	inline nsvec3<T> xxw() const { return nsvec3<T>(x, x, w); }
	inline nsvec3<T> xyx() const { return nsvec3<T>(x, y, x); }
	inline nsvec3<T> xyy() const { return nsvec3<T>(x, y, y); }
	inline nsvec3<T> xyz() const { return nsvec3<T>(x, y, z); }
	inline nsvec3<T> xyw() const { return nsvec3<T>(x, y, w); }
	inline nsvec3<T> xzx() const { return nsvec3<T>(x, z, x); }
	inline nsvec3<T> xzy() const { return nsvec3<T>(x, z, y); }
	inline nsvec3<T> xzz() const { return nsvec3<T>(x, z, z); }
	inline nsvec3<T> xzw() const { return nsvec3<T>(x, z, w); }
	inline nsvec3<T> xwx() const { return nsvec3<T>(x, w, x); }
	inline nsvec3<T> xwy() const { return nsvec3<T>(x, w, y); }
	inline nsvec3<T> xwz() const { return nsvec3<T>(x, w, z); }
	inline nsvec3<T> xww() const { return nsvec3<T>(x, w, w); }
	inline nsvec3<T> yxx() const { return nsvec3<T>(y, x, x); }
	inline nsvec3<T> yxy() const { return nsvec3<T>(y, x, y); }
	inline nsvec3<T> yxz() const { return nsvec3<T>(y, x, z); }
	inline nsvec3<T> yxw() const { return nsvec3<T>(y, x, w); }
	inline nsvec3<T> yyx() const { return nsvec3<T>(y, y, x); }
	inline nsvec3<T> yyy() const { return nsvec3<T>(y, y, y); }
	inline nsvec3<T> yyz() const { return nsvec3<T>(y, y, z); }
	inline nsvec3<T> yyw() const { return nsvec3<T>(y, y, w); }
	inline nsvec3<T> yzx() const { return nsvec3<T>(y, z, x); }
	inline nsvec3<T> yzy() const { return nsvec3<T>(y, z, y); }
	inline nsvec3<T> yzz() const { return nsvec3<T>(y, z, z); }
	inline nsvec3<T> yzw() const { return nsvec3<T>(y, z, w); }
	inline nsvec3<T> ywx() const { return nsvec3<T>(y, w, x); }
	inline nsvec3<T> ywy() const { return nsvec3<T>(y, w, y); }
	inline nsvec3<T> ywz() const { return nsvec3<T>(y, w, z); }
	inline nsvec3<T> yww() const { return nsvec3<T>(y, w, w); }
	inline nsvec3<T> zxx() const { return nsvec3<T>(z, x, x); }
	inline nsvec3<T> zxy() const { return nsvec3<T>(z, x, y); }
	inline nsvec3<T> zxz() const { return nsvec3<T>(z, x, z); }
	inline nsvec3<T> zxw() const { return nsvec3<T>(z, x, w); }
	inline nsvec3<T> zyx() const { return nsvec3<T>(z, y, x); }
	inline nsvec3<T> zyy() const { return nsvec3<T>(z, y, y); }
	inline nsvec3<T> zyz() const { return nsvec3<T>(z, y, z); }
	inline nsvec3<T> zyw() const { return nsvec3<T>(z, y, w); }
	inline nsvec3<T> zzx() const { return nsvec3<T>(z, z, x); }
	inline nsvec3<T> zzy() const { return nsvec3<T>(z, z, y); }
	inline nsvec3<T> zzz() const { return nsvec3<T>(z, z, z); }
	inline nsvec3<T> zzw() const { return nsvec3<T>(z, z, w); }
	inline nsvec3<T> zwx() const { return nsvec3<T>(z, w, x); }
	inline nsvec3<T> zwy() const { return nsvec3<T>(z, w, y); }
	inline nsvec3<T> zwz() const { return nsvec3<T>(z, w, z); }
	inline nsvec3<T> zww() const { return nsvec3<T>(z, w, w); }
	inline nsvec3<T> wxx() const { return nsvec3<T>(w, x, x); }
	inline nsvec3<T> wxy() const { return nsvec3<T>(w, x, y); }
	inline nsvec3<T> wxz() const { return nsvec3<T>(w, x, z); }
	inline nsvec3<T> wxw() const { return nsvec3<T>(w, x, w); }
	inline nsvec3<T> wyx() const { return nsvec3<T>(w, y, x); }
	inline nsvec3<T> wyy() const { return nsvec3<T>(w, y, y); }
	inline nsvec3<T> wyz() const { return nsvec3<T>(w, y, z); }
	inline nsvec3<T> wyw() const { return nsvec3<T>(w, y, w); }
	inline nsvec3<T> wzx() const { return nsvec3<T>(w, z, x); }
	inline nsvec3<T> wzy() const { return nsvec3<T>(w, z, y); }
	inline nsvec3<T> wzz() const { return nsvec3<T>(w, z, z); }
	inline nsvec3<T> wzw() const { return nsvec3<T>(w, z, w); }
	inline nsvec3<T> wwx() const { return nsvec3<T>(w, w, x); }
	inline nsvec3<T> wwy() const { return nsvec3<T>(w, w, y); }
	inline nsvec3<T> wwz() const { return nsvec3<T>(w, w, z); }
	inline nsvec3<T> www() const { return nsvec3<T>(w, w, w); }

	inline nsvec3<T> rrr() const { return nsvec3<T>(r, r, r); }
	inline nsvec3<T> rrg() const { return nsvec3<T>(r, r, g); }
	inline nsvec3<T> rrb() const { return nsvec3<T>(r, r, b); }
	inline nsvec3<T> rra() const { return nsvec3<T>(r, r, a); }
	inline nsvec3<T> rgr() const { return nsvec3<T>(r, g, r); }
	inline nsvec3<T> rgg() const { return nsvec3<T>(r, g, g); }
	inline nsvec3<T> rgb() const { return nsvec3<T>(r, g, b); }
	inline nsvec3<T> rga() const { return nsvec3<T>(r, g, a); }
	inline nsvec3<T> rbr() const { return nsvec3<T>(r, b, r); }
	inline nsvec3<T> rbg() const { return nsvec3<T>(r, b, g); }
	inline nsvec3<T> rbb() const { return nsvec3<T>(r, b, b); }
	inline nsvec3<T> rba() const { return nsvec3<T>(r, b, a); }
	inline nsvec3<T> rar() const { return nsvec3<T>(r, a, r); }
	inline nsvec3<T> rag() const { return nsvec3<T>(r, a, g); }
	inline nsvec3<T> rab() const { return nsvec3<T>(r, a, b); }
	inline nsvec3<T> raa() const { return nsvec3<T>(r, a, a); }
	inline nsvec3<T> grr() const { return nsvec3<T>(g, r, r); }
	inline nsvec3<T> grg() const { return nsvec3<T>(g, r, g); }
	inline nsvec3<T> grb() const { return nsvec3<T>(g, r, b); }
	inline nsvec3<T> gra() const { return nsvec3<T>(g, r, a); }
	inline nsvec3<T> ggr() const { return nsvec3<T>(g, g, r); }
	inline nsvec3<T> ggg() const { return nsvec3<T>(g, g, g); }
	inline nsvec3<T> ggb() const { return nsvec3<T>(g, g, b); }
	inline nsvec3<T> gga() const { return nsvec3<T>(g, g, a); }
	inline nsvec3<T> gbr() const { return nsvec3<T>(g, b, r); }
	inline nsvec3<T> gbg() const { return nsvec3<T>(g, b, g); }
	inline nsvec3<T> gbb() const { return nsvec3<T>(g, b, b); }
	inline nsvec3<T> gba() const { return nsvec3<T>(g, b, a); }
	inline nsvec3<T> gar() const { return nsvec3<T>(g, a, r); }
	inline nsvec3<T> gag() const { return nsvec3<T>(g, a, g); }
	inline nsvec3<T> gab() const { return nsvec3<T>(g, a, b); }
	inline nsvec3<T> gaa() const { return nsvec3<T>(g, a, a); }
	inline nsvec3<T> brr() const { return nsvec3<T>(b, r, r); }
	inline nsvec3<T> brg() const { return nsvec3<T>(b, r, g); }
	inline nsvec3<T> brb() const { return nsvec3<T>(b, r, b); }
	inline nsvec3<T> bra() const { return nsvec3<T>(b, r, a); }
	inline nsvec3<T> bgr() const { return nsvec3<T>(b, g, r); }
	inline nsvec3<T> bgg() const { return nsvec3<T>(b, g, g); }
	inline nsvec3<T> bgb() const { return nsvec3<T>(b, g, b); }
	inline nsvec3<T> bga() const { return nsvec3<T>(b, g, a); }
	inline nsvec3<T> bbr() const { return nsvec3<T>(b, b, r); }
	inline nsvec3<T> bbg() const { return nsvec3<T>(b, b, g); }
	inline nsvec3<T> bbb() const { return nsvec3<T>(b, b, b); }
	inline nsvec3<T> bba() const { return nsvec3<T>(b, b, a); }
	inline nsvec3<T> bar() const { return nsvec3<T>(b, a, r); }
	inline nsvec3<T> bag() const { return nsvec3<T>(b, a, g); }
	inline nsvec3<T> bab() const { return nsvec3<T>(b, a, b); }
	inline nsvec3<T> baa() const { return nsvec3<T>(b, a, a); }
	inline nsvec3<T> arr() const { return nsvec3<T>(a, r, r); }
	inline nsvec3<T> arg() const { return nsvec3<T>(a, r, g); }
	inline nsvec3<T> arb() const { return nsvec3<T>(a, r, b); }
	inline nsvec3<T> ara() const { return nsvec3<T>(a, r, a); }
	inline nsvec3<T> agr() const { return nsvec3<T>(a, g, r); }
	inline nsvec3<T> agg() const { return nsvec3<T>(a, g, g); }
	inline nsvec3<T> agb() const { return nsvec3<T>(a, g, b); }
	inline nsvec3<T> aga() const { return nsvec3<T>(a, g, a); }
	inline nsvec3<T> abr() const { return nsvec3<T>(a, b, r); }
	inline nsvec3<T> abg() const { return nsvec3<T>(a, b, g); }
	inline nsvec3<T> abb() const { return nsvec3<T>(a, b, b); }
	inline nsvec3<T> aba() const { return nsvec3<T>(a, b, a); }
	inline nsvec3<T> aar() const { return nsvec3<T>(a, a, r); }
	inline nsvec3<T> aag() const { return nsvec3<T>(a, a, g); }
	inline nsvec3<T> aab() const { return nsvec3<T>(a, a, b); }
	inline nsvec3<T> aaa() const { return nsvec3<T>(a, a, a); }

	inline nsvec3<T> sss() const { return nsvec3<T>(s, s, s); }
	inline nsvec3<T> sst() const { return nsvec3<T>(s, s, t); }
	inline nsvec3<T> ssp() const { return nsvec3<T>(s, s, p); }
	inline nsvec3<T> ssq() const { return nsvec3<T>(s, s, q); }
	inline nsvec3<T> sts() const { return nsvec3<T>(s, t, s); }
	inline nsvec3<T> stt() const { return nsvec3<T>(s, t, t); }
	inline nsvec3<T> stp() const { return nsvec3<T>(s, t, p); }
	inline nsvec3<T> stq() const { return nsvec3<T>(s, t, q); }
	inline nsvec3<T> sps() const { return nsvec3<T>(s, p, s); }
	inline nsvec3<T> spt() const { return nsvec3<T>(s, p, t); }
	inline nsvec3<T> spp() const { return nsvec3<T>(s, p, p); }
	inline nsvec3<T> spq() const { return nsvec3<T>(s, p, q); }
	inline nsvec3<T> sqs() const { return nsvec3<T>(s, q, s); }
	inline nsvec3<T> sqt() const { return nsvec3<T>(s, q, t); }
	inline nsvec3<T> sqp() const { return nsvec3<T>(s, q, p); }
	inline nsvec3<T> sqq() const { return nsvec3<T>(s, q, q); }
	inline nsvec3<T> tss() const { return nsvec3<T>(t, s, s); }
	inline nsvec3<T> tst() const { return nsvec3<T>(t, s, t); }
	inline nsvec3<T> tsp() const { return nsvec3<T>(t, s, p); }
	inline nsvec3<T> tsq() const { return nsvec3<T>(t, s, q); }
	inline nsvec3<T> tts() const { return nsvec3<T>(t, t, s); }
	inline nsvec3<T> ttt() const { return nsvec3<T>(t, t, t); }
	inline nsvec3<T> ttp() const { return nsvec3<T>(t, t, p); }
	inline nsvec3<T> ttq() const { return nsvec3<T>(t, t, q); }
	inline nsvec3<T> tps() const { return nsvec3<T>(t, p, s); }
	inline nsvec3<T> tpt() const { return nsvec3<T>(t, p, t); }
	inline nsvec3<T> tpp() const { return nsvec3<T>(t, p, p); }
	inline nsvec3<T> tpq() const { return nsvec3<T>(t, p, q); }
	inline nsvec3<T> tqs() const { return nsvec3<T>(t, q, s); }
	inline nsvec3<T> tqt() const { return nsvec3<T>(t, q, t); }
	inline nsvec3<T> tqp() const { return nsvec3<T>(t, q, p); }
	inline nsvec3<T> tqq() const { return nsvec3<T>(t, q, q); }
	inline nsvec3<T> pss() const { return nsvec3<T>(p, s, s); }
	inline nsvec3<T> pst() const { return nsvec3<T>(p, s, t); }
	inline nsvec3<T> psp() const { return nsvec3<T>(p, s, p); }
	inline nsvec3<T> psq() const { return nsvec3<T>(p, s, q); }
	inline nsvec3<T> pts() const { return nsvec3<T>(p, t, s); }
	inline nsvec3<T> ptt() const { return nsvec3<T>(p, t, t); }
	inline nsvec3<T> ptp() const { return nsvec3<T>(p, t, p); }
	inline nsvec3<T> ptq() const { return nsvec3<T>(p, t, q); }
	inline nsvec3<T> pps() const { return nsvec3<T>(p, p, s); }
	inline nsvec3<T> ppt() const { return nsvec3<T>(p, p, t); }
	inline nsvec3<T> ppp() const { return nsvec3<T>(p, p, p); }
	inline nsvec3<T> ppq() const { return nsvec3<T>(p, p, q); }
	inline nsvec3<T> pqs() const { return nsvec3<T>(p, q, s); }
	inline nsvec3<T> pqt() const { return nsvec3<T>(p, q, t); }
	inline nsvec3<T> pqp() const { return nsvec3<T>(p, q, p); }
	inline nsvec3<T> pqq() const { return nsvec3<T>(p, q, q); }
	inline nsvec3<T> qss() const { return nsvec3<T>(q, s, s); }
	inline nsvec3<T> qst() const { return nsvec3<T>(q, s, t); }
	inline nsvec3<T> qsp() const { return nsvec3<T>(q, s, p); }
	inline nsvec3<T> qsq() const { return nsvec3<T>(q, s, q); }
	inline nsvec3<T> qts() const { return nsvec3<T>(q, t, s); }
	inline nsvec3<T> qtt() const { return nsvec3<T>(q, t, t); }
	inline nsvec3<T> qtp() const { return nsvec3<T>(q, t, p); }
	inline nsvec3<T> qtq() const { return nsvec3<T>(q, t, q); }
	inline nsvec3<T> qps() const { return nsvec3<T>(q, p, s); }
	inline nsvec3<T> qpt() const { return nsvec3<T>(q, p, t); }
	inline nsvec3<T> qpp() const { return nsvec3<T>(q, p, p); }
	inline nsvec3<T> qpq() const { return nsvec3<T>(q, p, q); }
	inline nsvec3<T> qqs() const { return nsvec3<T>(q, q, s); }
	inline nsvec3<T> qqt() const { return nsvec3<T>(q, q, t); }
	inline nsvec3<T> qqp() const { return nsvec3<T>(q, q, p); }
	inline nsvec3<T> qqq() const { return nsvec3<T>(q, q, q); }

	// Vec 2 Swizzles
	inline nsvec2<T> xx() const { return nsvec2<T>(x, x); }
	inline nsvec2<T> xy() const { return nsvec2<T>(x, y); }
	inline nsvec2<T> xz() const { return nsvec2<T>(x, z); }
	inline nsvec2<T> xw() const { return nsvec2<T>(x, w); }
	inline nsvec2<T> yx() const { return nsvec2<T>(y, x); }
	inline nsvec2<T> yy() const { return nsvec2<T>(y, y); }
	inline nsvec2<T> yz() const { return nsvec2<T>(y, z); }
	inline nsvec2<T> yw() const { return nsvec2<T>(y, w); }
	inline nsvec2<T> zx() const { return nsvec2<T>(z, x); }
	inline nsvec2<T> zy() const { return nsvec2<T>(z, y); }
	inline nsvec2<T> zz() const { return nsvec2<T>(z, z); }
	inline nsvec2<T> zw() const { return nsvec2<T>(z, w); }
	inline nsvec2<T> wx() const { return nsvec2<T>(w, x); }
	inline nsvec2<T> wy() const { return nsvec2<T>(w, y); }
	inline nsvec2<T> wz() const { return nsvec2<T>(w, z); }
	inline nsvec2<T> ww() const { return nsvec2<T>(w, w); }

	inline nsvec2<T> rr() const { return nsvec2<T>(r, r); }
	inline nsvec2<T> rg() const { return nsvec2<T>(r, g); }
	inline nsvec2<T> rb() const { return nsvec2<T>(r, b); }
	inline nsvec2<T> ra() const { return nsvec2<T>(r, a); }
	inline nsvec2<T> gr() const { return nsvec2<T>(g, r); }
	inline nsvec2<T> gg() const { return nsvec2<T>(g, g); }
	inline nsvec2<T> gb() const { return nsvec2<T>(g, b); }
	inline nsvec2<T> ga() const { return nsvec2<T>(g, a); }
	inline nsvec2<T> br() const { return nsvec2<T>(b, r); }
	inline nsvec2<T> bg() const { return nsvec2<T>(b, g); }
	inline nsvec2<T> bb() const { return nsvec2<T>(b, b); }
	inline nsvec2<T> ba() const { return nsvec2<T>(b, a); }
	inline nsvec2<T> ar() const { return nsvec2<T>(a, r); }
	inline nsvec2<T> ag() const { return nsvec2<T>(a, g); }
	inline nsvec2<T> ab() const { return nsvec2<T>(a, b); }
	inline nsvec2<T> aa() const { return nsvec2<T>(a, a); }

	inline nsvec2<T> ss() const { return nsvec2<T>(s, s); }
	inline nsvec2<T> st() const { return nsvec2<T>(s, t); }
	inline nsvec2<T> sp() const { return nsvec2<T>(s, p); }
	inline nsvec2<T> sq() const { return nsvec2<T>(s, q); }
	inline nsvec2<T> ts() const { return nsvec2<T>(t, s); }
	inline nsvec2<T> tt() const { return nsvec2<T>(t, t); }
	inline nsvec2<T> tp() const { return nsvec2<T>(t, p); }
	inline nsvec2<T> tq() const { return nsvec2<T>(t, q); }
	inline nsvec2<T> ps() const { return nsvec2<T>(p, s); }
	inline nsvec2<T> pt() const { return nsvec2<T>(p, t); }
	inline nsvec2<T> pp() const { return nsvec2<T>(p, p); }
	inline nsvec2<T> pq() const { return nsvec2<T>(p, q); }
	inline nsvec2<T> qs() const { return nsvec2<T>(q, s); }
	inline nsvec2<T> qt() const { return nsvec2<T>(q, t); }
	inline nsvec2<T> qp() const { return nsvec2<T>(q, p); }
	inline nsvec2<T> qq() const { return nsvec2<T>(q, q); }

	union
	{
		T data[4];

		struct
		{
			T x;
			T y;
			T z;
			T w;
		};

		struct
		{
			T r;
			T g;
			T b;
			T a;
		};

		struct
		{
			T s;
			T t;
			T p;
			T q;
		};
	};
};

template <class T>
nsvec4<T> operator*(const int32 & lhs_, const nsvec4<T> & rhs_)
{
	return rhs_ * static_cast<T>(lhs_);
}

template <class T>
nsvec4<T> operator*(const float & lhs_, const nsvec4<T> & rhs_)
{
	return rhs_ * static_cast<T>(lhs_);
}

template <class T>
nsvec4<T> operator*(const double & lhs_, const nsvec4<T> & rhs_)
{
	return rhs_ * static_cast<T>(lhs_);
}

template<class T>
nsvec4<T> operator/(const int32 & lhs_, const nsvec4<T> & rhs_)
{
	return nsvec4<T>(lhs_ / rhs_.x, lhs_ / rhs_.y, lhs_ / rhs_.z, lhs_ / rhs_.w);
}

template <class T>
nsvec4<T> operator/(const float & lhs_, const nsvec4<T> & rhs_)
{
	return nsvec4<T>(lhs_ / rhs_.x, lhs_ / rhs_.y, lhs_ / rhs_.z, lhs_ / rhs_.w);
}

template <class T>
nsvec4<T> operator/(const double & lhs_, const nsvec4<T> & rhs_)
{
	return nsvec4<T>(lhs_ / rhs_.x, lhs_ / rhs_.y, lhs_ / rhs_.z, lhs_ / rhs_.w);
}

template <class T>
nsvec4<T> abs(const nsvec4<T> & vec_)
{
	return nsvec4<T>(vec_).abs();
}

template <class T>
nsvec4<T> axis_angle(const nsvec3<T> & euler_, typename nsvec3<T>::EulerOrder order_, bool rads_)
{
	return nsvec4<T>().axis_angle_from(nsquat<T>().from(euler_, order_, rads_), rads_);
}

template <class T>
nsvec4<T> axis_angle(const nsquat<T> & orientation_, bool rads_)
{
	return nsvec4<T>().axis_angle_from(orientation_, rads_);
}

template <class T>
nsvec4<T> axis_angle(const nsmat3<T> & rotation_mat3_, bool rads_)
{
	return nsvec4<T>().axis_angle_from(nsquat<T>().from(rotation_mat3_), rads_);
}

template <class T>
nsvec4<T> axis_angle(const nsmat4<T> & transform_, bool rads_)
{
	return nsvec4<T>().axis_angle_from(nsquat<T>().from(transform_), rads_);
}

template <class T>
nsvec4<T> axis_angle(const nsvec3<T> & vec_, const nsvec3<T> & to_vec_, bool rads_)
{
	return nsvec4<T>().axis_angle_from(nsquat<T>().from(vec_, to_vec_), rads_);
}

template <class T>
nsvec4<T> ceil(const nsvec4<T> & vec_)
{
	nsvec4<T> ret(vec_);
	ret.ceiling();
	return ret;
}

template <class T>
nsvec4<T> clamp(const nsvec4<T> & vec_, const T & min_, const T & max_)
{
	nsvec4<T> ret(vec_);
	ret.clamp(min_, max_);
	return ret;
}

template <class T>
T distance(const nsvec4<T> & lvec_, const nsvec4<T> & rvec_)
{
	return lvec_.distanceTo(rvec_);
}

template <class T>
T dot(const nsvec4<T> & lhs_, const nsvec4<T> & rhs_)
{
	return lhs_ * rhs_;
}

template <class T>
nsvec4<T> floor(const nsvec4<T> & vec_)
{
	nsvec4<T> ret(vec_);
	ret.floor();
	return ret;
}

template <class T>
nsvec4<T> fract(const nsvec4<T> & vec_)
{
	return nsvec4<T>(vec_).fract();
}

template <class T>
T length(const nsvec4<T> & vec_)
{
	return vec_.length();
}

template <class T, class T2>
nsvec4<T> lerp(const nsvec4<T> & lhs_, const nsvec4<T> & rhs_, T2 scaling_factor_)
{
	nsvec4<T> ret(lhs_);
	ret.lerp(rhs_, scaling_factor_);
	return ret;
}

template <class T>
nsvec4<T> min(const nsvec4<T> & lhs_, const nsvec4<T> & rhs_)
{
	nsvec2<T> ret(lhs_);
	ret.minimize(rhs_);
	return ret;
}

template <class T>
nsvec4<T> max(const nsvec4<T> & lhs_, const nsvec4<T> & rhs_)
{
	nsvec2<T> ret(lhs_);
	ret.maximize(rhs_);
	return ret;
}

template <class T>
nsvec4<T> normalize(const nsvec4<T> & rhs_)
{
	nsvec4<T> ret(rhs_);
	ret.normalize();
	return ret;
}

template <class T>
nsvec4<T> round(const nsvec4<T> & vec_)
{
	nsvec4<T> ret(vec_);
	ret.round();
	return ret;
}

template<class PUPer, class T>
void pup(PUPer & p, nsvec4<T> & v4, const nsstring & var_name_)
{
	pup(p, v4.x, var_name_ + "[0]"); pup(p, v4.y, var_name_ + "[1]"); pup(p, v4.z, var_name_ + "[2]"); pup(p, v4.w, var_name_ + "[3]");
}

#endif
