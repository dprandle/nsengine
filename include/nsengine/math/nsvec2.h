#ifndef NSVEC2_H
#define NSVEC2_H


#define EPS_VEC_0 0.000001f
#include <nsstring.h>
#include <cmath>
#include <stdexcept>
#include <nstypes.h>

template <class T>
struct nsvec3;

template <class T>
struct nsvec4;

template <class T>
struct nsquat;

template <class T>
struct nsmat2;

template <class T>
struct nsmat3;

template <class T>
struct nsmat4;

template <class T>
struct nsvec2;

template <class T>
nsvec2<T> operator*(const int32 & lhs_, const nsvec2<T> & rhs_);

template <class T>
nsvec2<T> operator*(const float & lhs_, const nsvec2<T> & rhs_);

template <class T>
nsvec2<T> operator*(const double & lhs_, const nsvec2<T> & rhs_);

template <class T>
nsvec2<T> operator/(const int32 & lhs_, const nsvec2<T> & rhs_);

template <class T>
nsvec2<T> operator/(const float & lhs_, const nsvec2<T> & rhs_);

template <class T>
nsvec2<T> operator/(const double & lhs_, const nsvec2<T> & rhs_);

template <class T>
nsvec2<T> abs(const nsvec2<T> & vec_);

template <class T>
nsvec2<T> ceil(const nsvec2<T> & vec_);

template <class T>
nsvec2<T> clamp(const nsvec2<T> & vec_, const T & min_, const T & max_);

template <class T>
T distance(const nsvec2<T> & lvec_, const nsvec2<T> & rvec_);

template <class T>
T dot(const nsvec2<T> & lhs_, const nsvec2<T> & rhs_);

template <class T>
nsvec2<T> floor(const nsvec2<T> & vec_);

template <class T>
nsvec2<T> fract(const nsvec2<T> & vec_);

template <class T>
T length(const nsvec2<T> & vec_);

template <class T, class T2>
nsvec2<T> lerp(const nsvec2<T> & lhs_, const nsvec2<T> & rhs_, T2 scaling_factor_);

template <class T>
nsvec2<T> min(const nsvec2<T> & lhs_, const nsvec2<T> & rhs_);

template <class T>
nsvec2<T> max(const nsvec2<T> & lhs_, const nsvec2<T> & rhs_);

template <class T>
nsvec2<T> normalize(const nsvec2<T> & rhs_);

template <class T>
nsvec2<T> project(const nsvec2<T> & a, const nsvec2<T> & b);

template <class T>
nsvec2<T> project_plane(const nsvec2<T> & a, const nsvec2<T> & normal_);

template <class T>
nsvec2<T> reflect(const nsvec2<T> & incident_, const nsvec2<T> & normal_);

template <class T>
nsvec2<T> round(const nsvec2<T> & vec_);

template <class T>
nsvec2<T> scaling2d_vec(const nsmat2<T> & transform2d_);

template <class T>
nsvec2<T> scaling2d_vec(const nsmat3<T> & transform2d_);

template <class T>
nsvec2<T> translation2d_vec(const nsmat3<T> & transform2d_);

template<class PUPer, class T>
void pup(PUPer & p, nsvec2<T> & v2);

template <class T>
struct nsvec2
{
	nsvec2(const nsvec2<T> & copy_) : x(copy_.x), y(copy_.y) {}

	nsvec2(const T & init_=static_cast<T>(0)) : x(init_), y(init_) {}

	nsvec2(const T & x_,const T & y_) : x(x_), y(y_) {}

	nsvec2<T> & abs()
	{
		x = static_cast<T>(std::abs(x));
		y = static_cast<T>(std::abs(y));
		return *this;
	}

	T angle(bool rads_ = false) const
	{
		T ret = angle_to(nsvec2<T>(1, 0),rads_);
		if (y < 0)
			ret *= -1;
		return ret;
	}

	T angle_to(const nsvec2<T> & vec_, bool rads_ = false) const
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

	nsvec2<T> & ceil()
	{
		x = static_cast<T>(std::ceil(x));
		y = static_cast<T>(std::ceil(y));
		return *this;
	}

	nsvec2<T> & clamp(const T & min_ = static_cast<T>(0), const T & max_ = static_cast<T>(1))
	{
		if (x < min_)
			x = min_;
		if (y < min_)
			y = min_;
		if (x > max_)
			x = max_;
		if (y > max_)
			y = max_;
		return *this;
	}

	T distance_to(const nsvec2<T> & vec_) const
	{
		return ((vec_ - *this).length());
	}

	nsvec2<T> & floor()
	{
		x = static_cast<T>(std::floor(x));
		y = static_cast<T>(std::floor(y));
		return *this;
	}

	nsvec2<T> & fract()
	{
		for (uint32 i = 0; i < 2; ++i)
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
		return static_cast<T>(sqrt(x*x + y*y));
	}

	T length_sq() const
	{
		return x*x + y*y;
	}

	template<class T2>
	nsvec2<T> & lerp(const nsvec2<T> & vec_, const T2 & scaling_factor_)
	{
		x += static_cast<T>((vec_.x - x)*scaling_factor_);
		y += static_cast<T>((vec_.y - y)*scaling_factor_);
		return *this;
	}

	T min() const
	{
		if (x < y)
			return x;
		else
			return y;
	}

	nsvec2<T> & minimize(const nsvec2<T> & rhs_)
	{
		if (x < rhs_.x)
			x = rhs_.x;
		if (y < rhs_.y)
			y = rhs_.y;
		return *this;
	}

	T max() const
	{
		if (x > y)
			return x;
		else
			return y;
	}

	nsvec2<T> & maximize(const nsvec2<T> & rhs_)
	{
		if (x > rhs_.x)
			x = rhs_.x;
		if (y > rhs_.y)
			y = rhs_.y;
		return *this;
	}

	nsvec2<T> & normalize()
	{
		T l = length();
		if (l == 0)
			return *this;
		return *this *= (1 / l);
	}

	nsvec2<T> polar(bool rads_ = false) const
	{
		return nsvec2<T>(length(), angle(rads_));
	}
	
	nsvec2<T> & project(const nsvec2<T> & vec_)
	{
		T denom = vec_ * vec_;
		if (denom == static_cast<T>(0))
			return *this;
		(*this) = ((*this * vec_) / denom) * vec_;
		return *this;
	}

	nsvec2<T> & project_plane(const nsvec2<T> & normal_)
	{
		nsvec2<T> aonb(*this);
		aonb.project(normal_);
		(*this) -= aonb;
		return *this;
	}
	
	nsvec2<T> & reflect(const nsvec2<T> & normal_)
	{
		(*this) = (*this) - (static_cast<T>(2) * (normal_ * *this)) * normal_;
		return *this;
	}

	template<class T2>
	nsvec2<T> & rotate(const T2 & angle_)
	{
		T2 newangle = angle_ + static_cast<T2>(angle());
		x = length()*static_cast<T>(std::cos(angle_ + angle()));
		y = length()*static_cast<T>(std::sin(angle_ + angle()));
		return *this;
	}

	nsvec2<T> & round()
	{
		x = static_cast<T>(std::round(x));
		y = static_cast<T>(std::round(y));
		return *this;
	}

	nsvec2<T> & round_to_zero()
	{
		if (std::abs(x) < EPS_VEC_0)
			x = 0;
		if (std::abs(y) < EPS_VEC_0)
			y = 0;
		return *this;
	}

	nsvec2<T> & scaling_from(const nsmat2<T> & transform2d_)
	{
		x = transform2d_[0].length();
		y = transform2d_[1].length();
		return *this;
	}

	nsvec2<T> & scaling_from(const nsmat3<T> & transform2d_)
	{
		x = sqrt(transform2d_[0][0] * transform2d_[0][0] + transform2d_[0][1] * transform2d_[0][1]);
		y = sqrt(transform2d_[1][0] * transform2d_[1][0] + transform2d_[1][1] * transform2d_[1][1]);
		return *this;
	}

	nsvec2<T> & set(const T & val_)
	{
		x = y = val_;
		return *this;
	}

	nsvec2<T> & set(const T & x_, const T & y_)
	{
		x = x_; y = y_;
		return *this;
	}

	nsvec2<T> & from_polar(const T & pMag, const T & angle_, bool rads_ = false)
	{
		if (!rads_)
			angle_ = radians(angle_);

		x = static_cast<T>(pMag*std::cos(angle_));
		y = static_cast<T>(pMag*std::sin(angle_));
		return *this;
	}

	nsvec2<T> & from_polar(const nsvec2<T> & vec_, bool rads_ = false)
	{
		T ang = vec_.y;

		if (!rads_)
			ang = radians(ang);

		x = static_cast<T>(vec_.x*std::cos(ang));
		y = static_cast<T>(vec_.x*std::sin(ang));
		return *this;
	}

	nsvec2<T> & set_length(const T & len_)
	{
		T l = length();

		if (l == static_cast<T>(0))
			return *this;

		T mult = len_ / l;
		(*this) *= mult;
		return *this;
	}

	nsvec2<T> & translation_from(const nsmat3<T> & transform2d_)
	{
		return *this = transform2d_(2).xy();
	}

	nsstring to_string(bool polar_ = false)
	{
		nsstringstream ss;
		if (polar_)
			ss << "[" << length() << " " << angle() << "]";
		else
			ss << "[" << x << " " << y << "]";
		return ss.str();
	}

	nsvec2<T> operator+(const nsvec2<T> & rhs_) const
	{
		return nsvec2<T>(x + rhs_.x, y + rhs_.y);
	}

	nsvec2<T> operator-(const nsvec2<T> & rhs_) const
	{
		return nsvec2<T>(x - rhs_.x, y - rhs_.y);
	}

	T operator*(const nsvec2<T> & rhs_) const
	{
		return x*rhs_.x + y*rhs_.y;
	}

	nsmat2<T> operator^(const nsvec2<T> & rhs_) const
	{
		nsmat2<T> ret;
		ret[0] = x * rhs_;
		ret[1] = y * rhs_;
		return ret;
	}

	nsvec2<T> operator%(const nsvec2<T> & rhs_) const
	{
		return nsvec2<T>(x*rhs_.x, y*rhs_.y);
	}

	nsvec2<T> operator/(const nsvec2<T> & rhs_) const
	{
		return nsvec2<T>(x / rhs_.x, y / rhs_.y);
	}

	nsvec2<T> operator*(const T & rhs_) const
	{
		return nsvec2<T>(x * rhs_, y * rhs_);
	}

	nsvec2<T> operator/(const T & rhs_) const
	{
		return nsvec2<T>(x / rhs_, y / rhs_);
	}

	nsvec2<T> & operator=(const nsvec2<T> & rhs_)
	{
		if (this == &rhs_)
			return *this;
		x = rhs_.x;
		y = rhs_.y;
		return *this;
	}

	nsvec2<T> operator++(int32)
	{
		nsvec2<T> ret(*this);
		++(*this);
		return ret;
	}

	nsvec2<T> operator--(int32)
	{
		nsvec2<T> ret(*this);
		--(*this);
		return ret;
	}

	nsvec2<T> & operator++()
	{
		++x; ++y;
		return *this;
	}

	nsvec2<T> & operator--()
	{
		--x; --y;
		return *this;
	}

	nsvec2<T> & operator+=(const nsvec2<T> & rhs_)
	{
		x += rhs_.x; y += rhs_.y;
		return *this;
	}

	nsvec2<T> & operator-=(const nsvec2<T> & rhs_)
	{
		x -= rhs_.x; y -= rhs_.y;
		return *this;
	}

	nsvec2<T> & operator%=(const nsvec2<T> & rhs_)
	{
		x *= rhs_.x; y *= rhs_.y;
		return *this;
	}

	nsvec2<T> & operator/=(const nsvec2<T> & rhs_)
	{
		x /= rhs_.x; y /= rhs_.y;
		return *this;
	}

	nsvec2<T> & operator*=(const T & rhs_)
	{
		x *= rhs_; y *= rhs_;
		return *this;
	}

	nsvec2<T> & operator/=(const T & rhs_)
	{
		x /= rhs_; y /= rhs_;
		return *this;
	}

	bool operator==(const nsvec2<T> & rhs_) const
	{
		return ((x == rhs_.x) && (y == rhs_.y));
	}

	bool operator<(const nsvec2<T> & rhs_) const
	{
		return (x < rhs_.x && y < rhs_.y);
	}

	bool operator<=(const nsvec2<T> & rhs_) const
	{
		return (*this == rhs_ || *this < rhs_);
	}

	bool operator>(const nsvec2<T> & rhs_) const
	{
		return (x > rhs_.x && y > rhs_.y);
	}

	bool operator>=(const nsvec2<T> & rhs_) const
	{
		return !(*this == rhs_ || *this > rhs_);
	}

	bool operator!=(const nsvec2<T> & rhs_) const
	{
		return !(*this == rhs_);
	}

	bool operator==(const T & rhs_) const
	{
		return ((x == rhs_) && (y == rhs_));
	}

	bool operator!=(const T & rhs_) const
	{
		return !(*this == rhs_);
	}

	const T & operator[](const uint32 & val_) const
	{
		if (val_ > 1)
			throw(std::out_of_range("vec2 index out of range"));
		return data[val_];
	}

	T & operator[](const uint32 & val_)
	{
		if (val_ > 1)
			throw(std::out_of_range("vec2 index out of range"));
		return data[val_];
	}


	// Swizzle operations
	inline nsvec2<T> xx() const { return nsvec2<T>(x, x); }
	inline nsvec2<T> yx() const { return nsvec2<T>(y, x); }
	inline nsvec2<T> yy() const { return nsvec2<T>(y, y); }

	inline nsvec2<T> ss() const { return nsvec2<T>(x, x); }
	inline nsvec2<T> ts() const { return nsvec2<T>(y, x); }
	inline nsvec2<T> tt() const { return nsvec2<T>(y, y); }

	inline nsvec2<T> uu() const { return nsvec2<T>(x, x); }
	inline nsvec2<T> vu() const { return nsvec2<T>(y, x); }
	inline nsvec2<T> vv() const { return nsvec2<T>(y, y); }

	inline nsvec2<T> ww() const { return nsvec2<T>(x, x); }
	inline nsvec2<T> hw() const { return nsvec2<T>(y, x); }
	inline nsvec2<T> hh() const { return nsvec2<T>(y, y); }

	union
	{
		T data[2];

		struct 
		{
			T x;
			T y;
		};

		struct 
		{
			T w;
			T h;
		};

		struct 
		{
			T s;
			T t;
		};

		struct 
		{
			T u;
			T v;
		};
	};
};

template <class T>
nsvec2<T> operator*(const int32 & lhs_, const nsvec2<T> & rhs_)
{
	return rhs_ * static_cast<T>(lhs_);
}

template <class T>
nsvec2<T> operator*(const float & lhs_, const nsvec2<T> & rhs_)
{
	return rhs_ * static_cast<T>(lhs_);
}

template <class T>
nsvec2<T> operator*(const double & lhs_, const nsvec2<T> & rhs_)
{
	return rhs_ * static_cast<T>(lhs_);
}

template <class T>
nsvec2<T> operator/(const int32 & lhs_, const nsvec2<T> & rhs_)
{
	return nsvec2<T>(lhs_ / rhs_.x, lhs_ / rhs_.y);
}

template <class T>
nsvec2<T> operator/(const float & lhs_, const nsvec2<T> & rhs_)
{
	return nsvec2<T>(lhs_ / rhs_.x, lhs_ / rhs_.y);
}

template <class T>
nsvec2<T> operator/(const double & lhs_, const nsvec2<T> & rhs_)
{
	return nsvec2<T>(lhs_ / rhs_.x, lhs_ / rhs_.y);
}

template <class T>
nsvec2<T> abs(const nsvec2<T> & vec_)
{
	return nsvec2<T>(vec_).abs();
}

template <class T>
nsvec2<T> ceil(const nsvec2<T> & vec_)
{
	nsvec2<T> ret(vec_);
	ret.ceiling();
	return ret;
}

template <class T>
nsvec2<T> clamp(const nsvec2<T> & vec_, const T & min_, const T & max_)
{
	nsvec2<T> ret(vec_);
	ret.clamp(min_, max_);
	return ret;
}

template <class T>
T distance(const nsvec2<T> & lvec_, const nsvec2<T> & rvec_)
{
	return lvec_.distanceTo(rvec_);
}

template <class T>
T dot(const nsvec2<T> & lhs_, const nsvec2<T> & rhs_)
{
	return lhs_ * rhs_;
}

template <class T>
nsvec2<T> floor(const nsvec2<T> & vec_)
{
	nsvec2<T> ret(vec_);
	ret.floor();
	return ret;
}

template <class T>
nsvec2<T> fract(const nsvec2<T> & vec_)
{
	return nsvec2<T>(vec_).fract();
}

template <class T>
T length(const nsvec2<T> & vec_)
{
	return vec_.length();
}

template <class T, class T2>
nsvec2<T> lerp(const nsvec2<T> & lhs_, const nsvec2<T> & rhs_, T2 scaling_factor_)
{
	nsvec2<T> ret(lhs_);
	ret.lerp(rhs_, scaling_factor_);
	return ret;
}

template <class T>
nsvec2<T> min(const nsvec2<T> & lhs_, const nsvec2<T> & rhs_)
{
	nsvec2<T> ret(lhs_);
	ret.minimize(rhs_);
	return ret;
}

template <class T>
nsvec2<T> max(const nsvec2<T> & lhs_, const nsvec2<T> & rhs_)
{
	nsvec2<T> ret(lhs_);
	ret.maximize(rhs_);
	return ret;
}

template <class T>
nsvec2<T> normalize(const nsvec2<T> & rhs_)
{
	nsvec2<T> ret(rhs_);
	ret.normalize();
	return ret;
}

template <class T>
nsvec2<T> project(const nsvec2<T> & a, const nsvec2<T> & b)
{
	nsvec2<T> ret(a);
	ret.project(b);
	return ret;
}

template <class T>
nsvec2<T> project_plane(const nsvec2<T> & a, const nsvec2<T> & normal_)
{
	nsvec2<T> ret(a);
	ret.project_plane(normal_);
	return ret;
}

template <class T>
nsvec2<T> reflect(const nsvec2<T> & incident_, const nsvec2<T> & normal_)
{
	nsvec2<T> ret(incident_);
	incident_.reflect(normal_);
	return ret;
}

template <class T>
nsvec2<T> round(const nsvec2<T> & vec_)
{
	nsvec2<T> ret(vec_);
	ret.round();
	return ret;
}

template <class T>
nsvec2<T> scaling2d_vec(const nsmat2<T> & transform2d_)
{
	return nsvec2<T>().scaling_from(transform2d_);
}

template <class T>
nsvec2<T> scaling2d_vec(const nsmat3<T> & transform2d_)
{
	return nsvec2<T>().scaling_from(transform2d_);
}

template <class T>
nsvec2<T> translation2d_vec(const nsmat3<T> & transform2d_)
{
	return nsvec2<T>().translation_from(transform2d_);
}

template<class PUPer, class T>
void pup(PUPer & p, nsvec2<T> & v2, const nsstring & var_name_)
{
	pup(p, v2.x, var_name_ + "[0]"); pup(p, v2.y, var_name_ + "[1]");
}
#endif
