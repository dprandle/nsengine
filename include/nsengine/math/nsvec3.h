#ifndef NSVEC3_H
#define NSVEC3_H

#include "nsvec2.h"

template <class T>
nsvec3<T> operator*(const int32 & lhs_, const nsvec3<T> & rhs_);

template <class T>
nsvec3<T> operator*(const float & lhs_, const nsvec3<T> & rhs_);

template <class T>
nsvec3<T> operator*(const double & lhs_, const nsvec3<T> & rhs_);

template <class T>
nsvec3<T> operator/(const int32 & lhs_, const nsvec3<T> & rhs_);

template <class T>
nsvec3<T> operator/(const float & lhs_, const nsvec3<T> & rhs_);

template <class T>
nsvec3<T> operator/(const double & lhs_, const nsvec3<T> & rhs_);

template <class T>
nsvec3<T> abs(const nsvec3<T> & vec_);

template <class T>
nsvec3<T> ceil(const nsvec3<T> & vec_);

template <class T>
nsvec3<T> clamp(const nsvec3<T> & vec_, const T & min_, const T & max_);

template <class T>
nsvec3<T> cross(const nsvec3<T> & lhs_, const nsvec3<T> & rhs_);

template <class T>
T distance(const nsvec3<T> & lvec_, const nsvec3<T> & rvec_);

template <class T>
T dot(const nsvec3<T> & lhs_, const nsvec3<T> & rhs_);

template <class T>
nsvec3<T> euler(const nsvec4<T> & axis_angle_, typename nsvec3<T>::EulerOrder order_ , bool rads_ = false);

template <class T>
nsvec3<T> euler(const nsquat<T> & orientation_, typename nsvec3<T>::EulerOrder order_ , bool rads_ = false);

template <class T>
nsvec3<T> euler(const nsmat3<T> & rotation_mat3_, typename nsvec3<T>::EulerOrder order_ , bool rads_ = false);

template <class T>
nsvec3<T> euler(const nsmat4<T> & transform_, typename nsvec3<T>::EulerOrder order_ , bool rads_ = false);

template <class T>
nsvec3<T> euler(const nsvec3<T> & vec_, const nsvec3<T> & to_vec_, typename nsvec3<T>::EulerOrder order_ , bool rads_ = false);

template <class T>
nsvec3<T> floor(const nsvec3<T> & vec_);

template <class T>
nsvec3<T> fract(const nsvec3<T> & vec_);

template <class T>
T length(const nsvec3<T> & vec_);

template <class T, class T2>
nsvec3<T> lerp(const nsvec3<T> & lhs_, const nsvec3<T> & rhs_, T2 scaling_factor_);

template <class T>
nsvec3<T> min(const nsvec3<T> & lhs_, const nsvec3<T> & rhs_);

template <class T>
nsvec3<T> max(const nsvec3<T> & lhs_, const nsvec3<T> & rhs_);

template <class T>
nsvec3<T> normalize(const nsvec3<T> & rhs_);

template <class T>
nsvec3<T> project(const nsvec3<T> & a, const nsvec3<T> & b);

template <class T>
nsvec3<T> project_plane(const nsvec3<T> & a, const nsvec3<T> & normal_);

template <class T>
nsvec3<T> reflect(const nsvec3<T> & incident_, const nsvec3<T> & normal_);

template <class T>
nsvec3<T> round(const nsvec3<T> & vec_);

template <class T>
nsvec3<T> scaling_vec(const nsmat3<T> & transform_);

template <class T>
nsvec3<T> scaling_vec(const nsmat4<T> & transform_);

template <class T>
nsvec3<T> translation_vec(const nsmat4<T> & transform_);

template<class PUPer, class T>
void pup(PUPer & p, nsvec3<T> & v3);

template <class T>
struct nsvec3
{
	enum CoordSys
	{
		Cartesian,
		Cylindrical,
		Spherical
	};

	enum EulerOrder
	{
		XYZ,
		XZY,
		YXZ,
		YZX,
		ZXY,
		ZYX
	};

	nsvec3(const nsvec3<T> & copy_) : x(copy_.x), y(copy_.y), z(copy_.z) {}
	nsvec3(const T & val_ = static_cast<T>(0)) : x(val_), y(val_), z(val_) {}
	nsvec3(const nsvec2<T> & xy, const T & z_) : x(xy.x), y(xy.y), z(z_) {}
	nsvec3(const T & x_, const nsvec2<T> & yz) : x(x_), y(yz.y), z(yz.z) {}
	nsvec3(const T & x_, const T & y_, const T & z_ = static_cast<T>(0)) : x(x_), y(y_), z(z_) {}

	nsvec3<T> & abs()
	{
		x = static_cast<T>(std::abs(x));
		y = static_cast<T>(std::abs(y));
		z = static_cast<T>(std::abs(z));
		return *this;
	}

	T angle_to(const nsvec3<T> vec_, bool rads_ = false) const
	{
		T dotP = (*this) * vec_;
		T l = length() * vec_.length();

		if (l == static_cast<T>(0))
			return static_cast<T>(0);

		dotP /= l;

		if (rads_)
			return T(acos(dotP));
		else
			return T(degrees(acos(dotP)));
	}

	nsvec3<T> & ceil()
	{
		x = static_cast<T>(std::ceil(x));
		y = static_cast<T>(std::ceil(y));
		z = static_cast<T>(std::ceil(z));
		return *this;
	}

	nsvec3<T> & clamp(const T & min_ = static_cast<T>(0), const T & max_ = static_cast<T>(0))
	{
		if (x < min_)
			x = min_;
		if (y < min_)
			y = min_;
		if (z < min_)
			z = min_;
		if (x > max_)
			x = max_;
		if (y > max_)
			y = max_;
		if (z > max_)
			z = max_;
		return *this;
	}

	nsvec3<T> & cross(const nsvec3<T> & cross_with_)
	{
		T tmpx = x, tmpy = y;
		x = y * cross_with_.z - z * cross_with_.y;
		y = z * cross_with_.x - tmpx * cross_with_.z;
		z = tmpx * cross_with_.y - tmpy * cross_with_.x;
		return *this;
	}

	nsvec3<T> cylindrical(bool rads_ = false) const
	{
		nsvec3<T> ret;
		ret.x = sqrt(x*x + y*y);

		if (x == static_cast<T>(0))
		{
			ret.y = static_cast<T>(PI / 2);
			if (y == static_cast<T>(0))
				ret.y = static_cast<T>(0);
		}
		else
			ret.y = static_cast<T>(atan2(y,x));

		ret.z = z;

		if (!rads_)
			ret.y = degrees(ret.y);

		return ret;
	}

	T distance_to(const nsvec3<T> & vec_) const
	{
		return ((vec_ - *this).length());
	}

	nsvec3<T> & euler_from(const nsvec4<T> & axis_angle_, EulerOrder order_ = XYZ, bool rads_ = false)
	{
		return euler_from(nsquat<T>().from(axis_angle_, rads_), order_, rads_);
	}

	nsvec3<T> & euler_from(const nsquat<T> & orientation_, EulerOrder order_, bool rads_)
	{
		return euler_from(nsmat3<T>().rotationFrom(orientation_), order_, rads_);
	}

	nsvec3<T> & euler_from(const nsmat3<T> & rotation_mat3_, EulerOrder order_ = XYZ, bool rads_ = false)
	{
		// https://github.com/mrdoob/three.js/blob/master/src/math/Euler.js
		T ep = static_cast<T>(1) - EPS;
		switch (order_)
		{
		case(XYZ) :
			y = std::asin(rotation_mat3_[0][2]);
			if (std::abs(rotation_mat3_[0][2]) < ep)
			{
				x = std::atan2(-rotation_mat3_[1][2], rotation_mat3_[2][2]);
				z = std::atan2(-rotation_mat3_[0][1], rotation_mat3_[0][0]);
			}
			else
			{
				x = std::atan2(rotation_mat3_[2][1], rotation_mat3_[1][1]);
				z = 0;
			}
			break;
		case(XZY) :
			z = std::asin(rotation_mat3_[0][1]);
			if (std::abs(rotation_mat3_[0][1]) < ep)
			{
				x = std::atan2(rotation_mat3_[2][1], rotation_mat3_[1][1]);
				y = std::atan2(rotation_mat3_[0][2], rotation_mat3_[0][0]);
			}
			else
			{
				x = std::atan2(-rotation_mat3_[1][2], rotation_mat3_[2][2]);
				y = 0;
			}
			break;
		case(YXZ) :
			x = std::asin(rotation_mat3_[1][2]);
			if (std::abs(rotation_mat3_[1][2]) < ep)
			{
				y = std::atan2(rotation_mat3_[0][2], rotation_mat3_[2][2]);
				z = std::atan2(rotation_mat3_[1][0], rotation_mat3_[1][1]);
			}
			else
			{
				y = std::atan2(-rotation_mat3_[2][0], rotation_mat3_[0][0]);
				z = 0;
			}
			break;
		case(YZX) :
			z = std::asin(rotation_mat3_[1][0]);
			if (std::abs(rotation_mat3_[1][0]) < ep)
			{
				x = std::atan2(-rotation_mat3_[1][2], rotation_mat3_[1][1]);
				y = std::atan2(-rotation_mat3_[2][0], rotation_mat3_[0][0]);
			}
			else
			{
				x = 0;
				y = std::atan2(rotation_mat3_[0][2], rotation_mat3_[2][2]);
			}
			break;
		case(ZXY) :
			x = std::asin(rotation_mat3_[2][1]);
			if (std::abs(rotation_mat3_[2][1]) < ep)
			{
				y = std::atan2(-rotation_mat3_[2][0], rotation_mat3_[2][2]);
				z = std::atan2(-rotation_mat3_[0][1], rotation_mat3_[1][1]);
			}
			else
			{
				y = 0;
				z = std::atan2(rotation_mat3_[1][0], rotation_mat3_[0][0]);
			}
			break;
		case(ZYX) :
			y = std::asin(rotation_mat3_[2][0]);
			if (std::abs(rotation_mat3_[2][0]) < ep)
			{
				x = std::atan2(rotation_mat3_[2][1], rotation_mat3_[2][2]);
				z = std::atan2(rotation_mat3_[1][0], rotation_mat3_[0][0]);
			}
			else
			{
				x = 0;
				z = std::atan2(-rotation_mat3_[0][1], rotation_mat3_[1][1]);
			}
			break;
		}
		if (!rads_)
			*this = degrees(*this);
		return *this;
	}

	nsvec3<T> & euler_from(const nsmat4<T> & transform_, EulerOrder order_ = XYZ, bool rads_ = false)
	{
		return euler_from(rotation_mat3_(transform_), order_, rads_);
	}

	nsvec3<T> & euler_from(const nsvec3<T> & vec_, const nsvec3<T> & to_vec_, EulerOrder order_ = XYZ, bool rads_ = false)
	{
		return euler_from(nsquat<T>().from(vec_, to_vec_), order_, rads_);
	}

	nsvec3<T> & floor()
	{
		x = static_cast<T>(std::floor(x));
		y = static_cast<T>(std::floor(y));
		z = static_cast<T>(std::floor(z));
		return *this;
	}

	nsvec3<T> & fract()
	{
		for (uint32 i = 0; i < 3; ++i)
		{
			if (data[i] >= 0)
				data[i] -= static_cast<T>(std::floor(data[i]));
			else
				data[i] -= static_cast<T>(std::ceil(data[i]));
		}
		return *this;
	}

	nsvec3<T> & from(CoordSys coord_sys_, const nsvec3<T> & vec_, bool rads_ = false)
	{
		switch (coord_sys_)
		{
		case(Cylindrical) :
		{
			T theta = vec_.y;
			if (!rads_)
				theta = radians(theta);
			x = vec_.x*cos(theta);
			y = vec_.x*sin(theta);
			z = vec_.z;
			break;
		}
		case(Spherical) :
		{
			T theta = vec_.y; T phi = vec_.z;
			if (!rads_)
			{
				theta = radians(theta);
				phi = radians(phi);
			}
			x = vec_.x*cos(theta)*sin(phi);
			y = vec_.x*sin(theta)*sin(phi);
			z = vec_.x*cos(phi);
			break;
		}
		default:
			*this = vec_;
		}
		return *this;
	}

	T length() const
	{
		return static_cast<T>(sqrt(x*x + y*y + z*z));
	}

	T length_sq() const
	{
		return x*x + y*y + z*z;
	}

	template<class T2 >
	nsvec3<T> & lerp(const nsvec3<T> & vec_, const T2 & scaling_factor_)
	{
		x += static_cast<T>((vec_.x - x)*scaling_factor_);
		y += static_cast<T>((vec_.y - y)*scaling_factor_);
		z += static_cast<T>((vec_.z - z)*scaling_factor_);
		return *this;
	}

	T min()
	{
		if (x < y)
		{
			if (x < z)
				return x;
			else
				return z;
		}
		else
		{
			if (y < z)
				return y;
			else
				return z;
		}
	}

	nsvec3<T> & minimize(const nsvec3<T> & rhs_)
	{
		if (x < rhs_.x)
			x = rhs_.x;
		if (y < rhs_.y)
			y = rhs_.y;
		if (z < rhs_.z)
			z = rhs_.z;
		return *this;
	}

	T max()
	{
		if (x > y)
		{
			if (x > z)
				return x;
			else
				return z;
		}
		else
		{
			if (y > z)
				return y;
			else
				return z;
		}
	}

	nsvec3<T> & maximize(const nsvec3<T> & rhs_)
	{
		if (x > rhs_.x)
			x = rhs_.x;
		if (y > rhs_.y)
			y = rhs_.y;
		if (z > rhs_.z)
			z = rhs_.z;
		return *this;
	}

	nsvec3<T> & normalize()
	{
		T l = length();
		if (l <= EPS)
			return *this;
		return *this *= (1 / l);
	}

	nsvec3<T> & project(const nsvec3<T> & vec_)
	{
		T denom = vec_ * vec_;
		if (denom == static_cast<T>(0))
			return *this;
		(*this) = ((*this * vec_) / denom) * vec_;
		return *this;
	}

	nsvec3<T> & project_plane(const nsvec3<T> & plane_normal_)
	{
		nsvec3<T> aonb(*this);
		aonb.project(plane_normal_);
		(*this) -= aonb;
		return *this;
	}

	nsvec3<T> & reflect(const nsvec3<T> & normal_)
	{

		(*this) = (*this) - (static_cast<T>(2) * (normal_ * *this)) * normal_;
		return *this;
	}

	nsvec3<T> & round()
	{
		x = static_cast<T>(std::round(x));
		y = static_cast<T>(std::round(y));
		z = static_cast<T>(std::round(z));
		return *this;
	}

	nsvec3<T> & round_to_zero()
	{
		if (std::abs(x) < EPS_VEC_0)
			x = 0;
		if (std::abs(y) < EPS_VEC_0)
			y = 0;
		if (std::abs(z) < EPS_VEC_0)
			z = 0;
		return *this;
	}

	nsvec3<T> & scaling_from(const nsmat3<T> & transform_)
	{
		x = transform_[0].length();
		y = transform_[1].length();
		z = transform_[2].length();
		return *this;
	}

	nsvec3<T> & scaling_from(const nsmat4<T> & transform_)
	{
		x = sqrt(transform_[0][0] * transform_[0][0] + transform_[0][1] * transform_[0][1] + transform_[0][2] * transform_[0][2]);
		y = sqrt(transform_[1][0] * transform_[1][0] + transform_[1][1] * transform_[1][1] + transform_[1][2] * transform_[1][2]);
		z = sqrt(transform_[2][0] * transform_[2][0] + transform_[2][1] * transform_[2][1] + transform_[2][2] * transform_[2][2]);
		return *this;
	}

	nsvec3<T> & translation_from(const nsmat4<T> & transform_)
	{
		return *this = transform_(3).xyz();
	}

	nsvec3<T> & set(const T & val_)
	{
		x = y = z = val_;
		return *this;
	}

	nsvec3<T> & set(const T & x_, const T & y_, const T & z_)
	{
		x = x_; y = y_; z = z_;
		return *this;
	}

	nsvec3<T> & set(const nsvec2<T> & xy, const T & z_)
	{
		x = xy.x; y = xy.y; z = z_;
		return *this;
	}

	nsvec3<T> & set(const T & x_, const nsvec2<T> & yz)
	{
		x = x_; y = yz.x; z = yz.y;
		return *this;
	}

	nsvec3<T> & set_length(const T & len_)
	{
		T l = length();

		if (l == static_cast<T>(0))
			return *this;

		T mult = len_ / l;
		(*this) *= mult;
		return *this;
	}

	nsvec3<T> spherical(bool rads_ = false) const
	{
		nsvec3<T> ret;
		ret.x = length();

		if (x == static_cast<T>(0))
		{
			ret.y = static_cast<T>(PI / 2);
			if (y == static_cast<T>(0))
				ret.y = static_cast<T>(0);
		}
		else
			ret.y = static_cast<T>(atan2(y,x));

		if (ret.x == static_cast<T>(0))
			ret.z = static_cast<T>(0);
		else
			ret.z = acos(z / ret.x);

		if (!rads_)
		{
			ret.y = degrees(ret.y);
			ret.z = degrees(ret.z);
		}

		return ret;
	}

	nsstring to_string(CoordSys disp_ = Cartesian) const
	{
		nsstringstream ss;
		if (disp_ == Cartesian)
			ss  << "[" << x << " " << y << " " << z << "]";
		else if (disp_ == Cylindrical)
		{
			nsvec3<T> cyl = cylindrical();
			ss << "[" << cyl.x << " " << cyl.y << " " << cyl.z << "]";
		}
		else
		{
			nsvec3<T> sph = spherical();
			ss << "[" << sph.x << " " << sph.y << " " << sph.z << "]";
		}
		return ss.str();
	}

	// overloaded operators
	nsvec3<T> operator+(const nsvec3<T> & rhs_) const
	{
		return nsvec3<T>(x + rhs_.x, y + rhs_.y, z + rhs_.z);
	}

	nsvec3<T> operator-(const nsvec3<T> & rhs_) const
	{
		return nsvec3<T>(x - rhs_.x, y - rhs_.y, z - rhs_.z);
	}

	T operator*(const nsvec3<T> & rhs_) const // dot product
	{
		return x*rhs_.x + y*rhs_.y + z*rhs_.z;
	}

	nsmat3<T> operator^(const nsvec3<T> & rhs_) const
	{
		nsmat3<T> ret;
		ret[0] = x * rhs_;
		ret[1] = y * rhs_;
		ret[2] = z * rhs_;
		return ret;
	}

	nsvec3<T> operator%(const nsvec3<T> & rhs_) const // component wise scalar product
	{
		return nsvec3<T>(x*rhs_.x, y*rhs_.y, z*rhs_.z);
	}

	nsvec3<T> operator/(const nsvec3<T> & rhs_) const
	{
		return nsvec3<T>(x/rhs_.x, y/rhs_.y, z/rhs_.z);
	}

	nsvec3<T> operator*(const T & rhs_) const
	{
		return nsvec3<T>(x * rhs_, y * rhs_, z * rhs_);
	}

	nsvec3<T> operator/(const T & rhs_) const
	{
		return nsvec3<T>(x / rhs_, y / rhs_, z / rhs_);
	}

	nsvec3<T> & operator=(const nsvec3<T> & rhs_)
	{
		if (this == &rhs_)
			return *this;
		x = rhs_.x;
		y = rhs_.y;
		z = rhs_.z;
		return *this;
	}

	nsvec3<T> operator++(int32)
	{
		nsvec3<T> ret(*this);
		++(*this);
		return ret;
	}

	nsvec3<T> operator--(int32)
	{
		nsvec3<T> ret(*this);
		--(*this);
		return ret;
	}

	nsvec3<T> & operator++()
	{
		++x; ++y; ++z;
		return *this;
	}

	nsvec3<T> & operator--()
	{
		--x; --y; --z;
		return *this;
	}

	nsvec3<T> & operator+=(const nsvec3<T> & rhs_)
	{
		x += rhs_.x; y += rhs_.y; z += rhs_.z;
		return *this;
	}

	nsvec3<T> & operator-=(const nsvec3<T> & rhs_)
	{
		x -= rhs_.x; y -= rhs_.y; z -= rhs_.z;
		return *this;
	}

	nsvec3<T> & operator%=(const nsvec3<T> & rhs_)
	{
		x *= rhs_.x; y *= rhs_.y; z *= rhs_.z;
		return *this;
	}

	nsvec3<T> & operator/=(const nsvec3<T> & rhs_)
	{
		x /= rhs_.x; y /= rhs_.y; z /= rhs_.z;
		return *this;
	}

	nsvec3<T> & operator*=(const T & rhs_)
	{
		x *= rhs_; y *= rhs_; z *= rhs_;
		return *this;
	}

	nsvec3<T> & operator/=(const T & rhs_)
	{
		x /= rhs_; y /= rhs_; z /= rhs_;
		return *this;
	}

	bool operator==(const nsvec3<T> & rhs_) const
	{
		return ((x == rhs_.x) && (y == rhs_.y) && (z == rhs_.z));
	}

	bool operator!=(const nsvec3<T> & rhs_) const
	{
		return !(*this == rhs_);
	}

	bool operator==(const T & rhs_) const
	{
		return ((x == rhs_) && (y == rhs_) && (z == rhs_));
	}

	bool operator!=(const T & rhs_) const
	{
		return !(*this == rhs_);
	}

	const T & operator[](const uint32 & val_) const
	{
		if (val_ > 2)
			throw(std::out_of_range("vec3 index out of range"));
		return data[val_];
	}

	T & operator[](const uint32 & val_)
	{
		if (val_ > 2)
			throw(std::out_of_range("vec3 index out of range"));
		return data[val_];
	}


	// Swizzle operations
	inline nsvec3<T> xxx() const { return nsvec3<T>(x, x, x); }
	inline nsvec3<T> xxy() const { return nsvec3<T>(x, x, y); }
	inline nsvec3<T> xxz() const { return nsvec3<T>(x, x, z); }
	inline nsvec3<T> xyx() const { return nsvec3<T>(x, y, x); }
	inline nsvec3<T> xyy() const { return nsvec3<T>(x, y, y); }
	inline nsvec3<T> xzx() const { return nsvec3<T>(x, z, x); }
	inline nsvec3<T> xzy() const { return nsvec3<T>(x, z, y); }
	inline nsvec3<T> xzz() const { return nsvec3<T>(x, z, z); }

	inline nsvec3<T> yxx() const { return nsvec3<T>(y, x, x); }
	inline nsvec3<T> yxy() const { return nsvec3<T>(y, x, y); }
	inline nsvec3<T> yxz() const { return nsvec3<T>(y, x, z); }
	inline nsvec3<T> yyx() const { return nsvec3<T>(y, y, x); }
	inline nsvec3<T> yyy() const { return nsvec3<T>(y, y, y); }
	inline nsvec3<T> yyz() const { return nsvec3<T>(y, y, z); }
	inline nsvec3<T> yzx() const { return nsvec3<T>(y, z, x); }
	inline nsvec3<T> yzy() const { return nsvec3<T>(y, z, y); }
	inline nsvec3<T> yzz() const { return nsvec3<T>(y, z, z); }

	inline nsvec3<T> zxx() const { return nsvec3<T>(z, x, x); }
	inline nsvec3<T> zxy() const { return nsvec3<T>(z, x, y); }
	inline nsvec3<T> zxz() const { return nsvec3<T>(z, x, z); }
	inline nsvec3<T> zyx() const { return nsvec3<T>(z, y, x); }
	inline nsvec3<T> zyy() const { return nsvec3<T>(z, y, y); }
	inline nsvec3<T> zyz() const { return nsvec3<T>(z, y, z); }
	inline nsvec3<T> zzx() const { return nsvec3<T>(z, z, x); }
	inline nsvec3<T> zzy() const { return nsvec3<T>(z, z, y); }
	inline nsvec3<T> zzz() const { return nsvec3<T>(z, z, z); }


	inline nsvec3<T> rrr() const { return nsvec3<T>(x, x, x); }
	inline nsvec3<T> rrg() const { return nsvec3<T>(x, x, y); }
	inline nsvec3<T> rrb() const { return nsvec3<T>(x, x, z); }
	inline nsvec3<T> rgr() const { return nsvec3<T>(x, y, x); }
	inline nsvec3<T> rgg() const { return nsvec3<T>(x, y, y); }
	inline nsvec3<T> rbr() const { return nsvec3<T>(x, z, x); }
	inline nsvec3<T> rbg() const { return nsvec3<T>(x, z, y); }
	inline nsvec3<T> rbb() const { return nsvec3<T>(x, z, z); }

	inline nsvec3<T> grr() const { return nsvec3<T>(y, x, x); }
	inline nsvec3<T> grg() const { return nsvec3<T>(y, x, y); }
	inline nsvec3<T> grb() const { return nsvec3<T>(y, x, z); }
	inline nsvec3<T> ggr() const { return nsvec3<T>(y, y, x); }
	inline nsvec3<T> ggg() const { return nsvec3<T>(y, y, y); }
	inline nsvec3<T> ggb() const { return nsvec3<T>(y, y, z); }
	inline nsvec3<T> gbr() const { return nsvec3<T>(y, z, x); }
	inline nsvec3<T> gbg() const { return nsvec3<T>(y, z, y); }
	inline nsvec3<T> gbb() const { return nsvec3<T>(y, z, z); }

	inline nsvec3<T> brr() const { return nsvec3<T>(z, x, x); }
	inline nsvec3<T> brg() const { return nsvec3<T>(z, x, y); }
	inline nsvec3<T> brb() const { return nsvec3<T>(z, x, z); }
	inline nsvec3<T> bgr() const { return nsvec3<T>(z, y, x); }
	inline nsvec3<T> bgg() const { return nsvec3<T>(z, y, y); }
	inline nsvec3<T> bgb() const { return nsvec3<T>(z, y, z); }
	inline nsvec3<T> bbr() const { return nsvec3<T>(z, z, x); }
	inline nsvec3<T> bbg() const { return nsvec3<T>(z, z, y); }
	inline nsvec3<T> bbb() const { return nsvec3<T>(z, z, z); }


	inline nsvec3<T> sss() const { return nsvec3<T>(x, x, x); }
	inline nsvec3<T> sst() const { return nsvec3<T>(x, x, y); }
	inline nsvec3<T> ssp() const { return nsvec3<T>(x, x, z); }
	inline nsvec3<T> sts() const { return nsvec3<T>(x, y, x); }
	inline nsvec3<T> stt() const { return nsvec3<T>(x, y, y); }
	inline nsvec3<T> sps() const { return nsvec3<T>(x, z, x); }
	inline nsvec3<T> spt() const { return nsvec3<T>(x, z, y); }
	inline nsvec3<T> spp() const { return nsvec3<T>(x, z, z); }

	inline nsvec3<T> tss() const { return nsvec3<T>(y, x, x); }
	inline nsvec3<T> tst() const { return nsvec3<T>(y, x, y); }
	inline nsvec3<T> tsp() const { return nsvec3<T>(y, x, z); }
	inline nsvec3<T> tts() const { return nsvec3<T>(y, y, x); }
	inline nsvec3<T> ttt() const { return nsvec3<T>(y, y, y); }
	inline nsvec3<T> ttp() const { return nsvec3<T>(y, y, z); }
	inline nsvec3<T> tps() const { return nsvec3<T>(y, z, x); }
	inline nsvec3<T> tpt() const { return nsvec3<T>(y, z, y); }
	inline nsvec3<T> tpp() const { return nsvec3<T>(y, z, z); }

	inline nsvec3<T> pss() const { return nsvec3<T>(z, x, x); }
	inline nsvec3<T> pst() const { return nsvec3<T>(z, x, y); }
	inline nsvec3<T> psp() const { return nsvec3<T>(z, x, z); }
	inline nsvec3<T> pts() const { return nsvec3<T>(z, y, x); }
	inline nsvec3<T> ptt() const { return nsvec3<T>(z, y, y); }
	inline nsvec3<T> ptp() const { return nsvec3<T>(z, y, z); }
	inline nsvec3<T> pps() const { return nsvec3<T>(z, z, x); }
	inline nsvec3<T> ppt() const { return nsvec3<T>(z, z, y); }
	inline nsvec3<T> ppp() const { return nsvec3<T>(z, z, z); }


	inline nsvec2<T> xx() const { return nsvec2<T>(x, x); }
	inline nsvec2<T> xy() const { return nsvec2<T>(x, y); }
	inline nsvec2<T> xz() const { return nsvec2<T>(x, z); }
	inline nsvec2<T> yx() const { return nsvec2<T>(y, x); }
	inline nsvec2<T> yy() const { return nsvec2<T>(y, y); }
	inline nsvec2<T> yz() const { return nsvec2<T>(y, z); }
	inline nsvec2<T> zx() const { return nsvec2<T>(z, x); }
	inline nsvec2<T> zy() const { return nsvec2<T>(z, y); }
	inline nsvec2<T> zz() const { return nsvec2<T>(z, z); }

	inline nsvec2<T> rr() const { return nsvec2<T>(x, x); }
	inline nsvec2<T> rg() const { return nsvec2<T>(x, y); }
	inline nsvec2<T> rb() const { return nsvec2<T>(x, z); }
	inline nsvec2<T> gr() const { return nsvec2<T>(y, x); }
	inline nsvec2<T> gg() const { return nsvec2<T>(y, y); }
	inline nsvec2<T> gb() const { return nsvec2<T>(y, z); }
	inline nsvec2<T> br() const { return nsvec2<T>(z, x); }
	inline nsvec2<T> bg() const { return nsvec2<T>(z, y); }
	inline nsvec2<T> bb() const { return nsvec2<T>(z, z); }

	inline nsvec2<T> ss() const { return nsvec2<T>(x, x); }
	inline nsvec2<T> st() const { return nsvec2<T>(x, y); }
	inline nsvec2<T> sp() const { return nsvec2<T>(x, z); }
	inline nsvec2<T> ts() const { return nsvec2<T>(y, x); }
	inline nsvec2<T> tt() const { return nsvec2<T>(y, y); }
	inline nsvec2<T> tp() const { return nsvec2<T>(y, z); }
	inline nsvec2<T> ps() const { return nsvec2<T>(z, x); }
	inline nsvec2<T> pt() const { return nsvec2<T>(z, y); }
	inline nsvec2<T> pp() const { return nsvec2<T>(z, z); }


	union
	{
		T data[3];

		struct
		{
			T x;
			T y;
			T z;
		};

		struct
		{
			T r;
			T g;
			T b;
		};

		struct
		{
			T s;
			T t;
			T p;
		};
	};
};

template <class T>
nsvec3<T> operator*(const int32 & lhs_, const nsvec3<T> & rhs_)
{
	return rhs_ * static_cast<T>(lhs_);
}

template <class T>
nsvec3<T> operator*(const float & lhs_, const nsvec3<T> & rhs_)
{
	return rhs_ * static_cast<T>(lhs_);
}

template <class T>
nsvec3<T> operator*(const double & lhs_, const nsvec3<T> & rhs_)
{
	return rhs_ * static_cast<T>(lhs_);
}

template <class T>
nsvec3<T> operator/(const int32 & lhs_, const nsvec3<T> & rhs_)
{
	return nsvec3<T>(lhs_ / rhs_.x, lhs_ / rhs_.y, lhs_ / rhs_.z);
}

template <class T>
nsvec3<T> operator/(const float & lhs_, const nsvec3<T> & rhs_)
{
	return nsvec3<T>(lhs_ / rhs_.x, lhs_ / rhs_.y, lhs_ / rhs_.z);
}

template <class T>
nsvec3<T> operator/(const double & lhs_, const nsvec3<T> & rhs_)
{
	return nsvec3<T>(lhs_ / rhs_.x, lhs_ / rhs_.y, lhs_ / rhs_.z);
}

template <class T>
nsvec3<T> abs(const nsvec3<T> & vec_)
{
	return nsvec3<T>(vec_).abs();
}

template <class T>
nsvec3<T> ceil(const nsvec3<T> & vec_)
{
	nsvec3<T> ret(vec_);
	ret.ceiling();
	return ret;
}

template <class T>
nsvec3<T> clamp(const nsvec3<T> & vec_, const T & min_, const T & max_)
{
	nsvec3<T> ret(vec_);
	ret.clamp(min_, max_);
	return ret;
}

template <class T>
nsvec3<T> cross(const nsvec3<T> & lhs_, const nsvec3<T> & rhs_)
{
	nsvec3<T> ret;
	ret.x = lhs_.y * rhs_.z - lhs_.z * rhs_.y;
	ret.y = lhs_.z * rhs_.x - lhs_.x * rhs_.z;
	ret.z = lhs_.x * rhs_.y - lhs_.y * rhs_.x;
	return ret;
}

template <class T>
T distance(const nsvec3<T> & lvec_, const nsvec3<T> & rvec_)
{
	return lvec_.distanceTo(rvec_);
}

template <class T>
T dot(const nsvec3<T> & lhs_, const nsvec3<T> & rhs_)
{
	return lhs_ * rhs_;
}

template <class T>
nsvec3<T> euler(const nsvec4<T> & axis_angle_, typename nsvec3<T>::EulerOrder order_, bool rads_)
{
	return nsvec3<T>().euler_from(nsquat<T>().from(axis_angle_, rads_), order_, rads_);
}

template <class T>
nsvec3<T> euler(const nsquat<T> & orientation_, typename nsvec3<T>::EulerOrder order_, bool rads_)
{
	return nsvec3<T>().euler_from(nsmat3<T>().rotation_from(orientation_), order_, rads_);
}

template <class T>
nsvec3<T> euler(const nsmat3<T> & rotation_mat3_, typename nsvec3<T>::EulerOrder order_, bool rads_)
{
	return nsvec3<T>().euler_from(rotation_mat3_, order_, rads_);
}

template <class T>
nsvec3<T> euler(const nsmat4<T> & transform_, typename nsvec3<T>::EulerOrder order_, bool rads_)
{
	return nsvec3<T>().euler_from(transform_, order_, rads_);
}

template <class T>
nsvec3<T> euler(const nsvec3<T> & vec_, const nsvec3<T> & to_vec_, typename nsvec3<T>::EulerOrder order_, bool rads_)
{
	return nsvec3<T>().euler_from(nsquat<T>().from(vec_, to_vec_), order_, rads_);
}

template <class T>
nsvec3<T> floor(const nsvec3<T> & vec_)
{
	nsvec3<T> ret(vec_);
	ret.floor();
	return ret;
}

template <class T>
nsvec3<T> fract(const nsvec3<T> & vec_)
{
	return nsvec3<T>(vec_).fract();
}

template <class T>
T length(const nsvec3<T> & vec_)
{
	return vec_.length();
}

template <class T, class T2>
nsvec3<T> lerp(const nsvec3<T> & lhs_, const nsvec3<T> & rhs_, T2 scaling_factor_)
{
	nsvec3<T> ret(lhs_);
	ret.lerp(rhs_, scaling_factor_);
	return ret;
}

template <class T>
nsvec3<T> min(const nsvec3<T> & lhs_, const nsvec3<T> & rhs_)
{
	nsvec2<T> ret(lhs_);
	ret.minimize(rhs_);
	return ret;
}

template <class T>
nsvec3<T> max(const nsvec3<T> & lhs_, const nsvec3<T> & rhs_)
{
	nsvec2<T> ret(lhs_);
	ret.maximize(rhs_);
	return ret;
}

template <class T>
nsvec3<T> normalize(const nsvec3<T> & rhs_)
{
	nsvec3<T> ret(rhs_);
	ret.normalize();
	return ret;
}

template <class T>
nsvec3<T> project(const nsvec3<T> & a, const nsvec3<T> & b)
{
	nsvec3<T> ret(a);
	ret.project(b);
	return ret;
}

template <class T>
nsvec3<T> project_plane(const nsvec3<T> & a, const nsvec3<T> & normal_)
{
	nsvec3<T> ret(a);
	ret.project_plane(normal_);
	return ret;
}

template <class T>
nsvec3<T> reflect(const nsvec3<T> & incident_, const nsvec3<T> & normal_)
{
	nsvec3<T> ret(incident_);
	ret.reflect(normal_);
	return ret;
}

template <class T>
nsvec3<T> round(const nsvec3<T> & vec_)
{
	nsvec3<T> ret(vec_);
	ret.round();
	return ret;
}

template <class T>
nsvec3<T> scaling_vec(const nsmat3<T> & transform_)
{
	return nsvec3<T>().scaling_from(transform_);
}

template <class T>
nsvec3<T> scaling_vec(const nsmat4<T> & transform_)
{
	return nsvec3<T>().scaling_from(transform_);
}

template <class T>
nsvec3<T> translation_vec(const nsmat4<T> & transform_)
{
	return nsvec3<T>().translation_from(transform_);
}

template<class PUPer, class T>
void pup(PUPer & p, nsvec3<T> & v3, const nsstring & var_name_)
{
	pup(p, v3.x, var_name_ + "[0]"); pup(p, v3.y, var_name_ + "[1]"); pup(p, v3.z, var_name_ + "[2]");
}

#endif
