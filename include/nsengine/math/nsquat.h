#ifndef NSQUAT_H
#define NSQUAT_H

#include "nsvec4.h"
#include <cmath>

template <class T>
nsquat<T> operator*(const int32 & lhs_, const nsquat<T> & rhs_);

template <class T>
nsquat<T> operator*(const float & lhs_, const nsquat<T> & rhs_);

template <class T>
nsquat<T> operator*(const double & lhs_, const nsquat<T> & rhs_);

template <class T>
nsquat<T> operator/(const int32 & lhs_, const nsquat<T> & rhs_);

template <class T>
nsquat<T> operator/(const float & lhs_, const nsquat<T> & rhs_);

template <class T>
nsquat<T> operator/(const double & lhs_, const nsquat<T> & rhs_);

template<class T>
nsquat<T> orientation(const nsmat3<T> & rotation_mat3_);

template<class T>
nsquat<T> orientation(const nsvec4<T> & axis_angle_, bool rads_ = false);

template<class T>
nsquat<T> orientation(const nsvec3<T> & euler_, typename nsvec3<T>::EulerOrder order_ , bool rads_ = false);

template<class T>
nsquat<T> orientation(const nsvec3<T> & vec_, const nsvec3<T> & to_vec_);

template<class T>
nsquat<T> orientation(const nsmat4<T> & transform_);

template<class T>
nsquat<T> conjugate(const nsquat<T> & quat_);

template <class T>
T dot(const nsquat<T> & pLeft, const nsquat<T> & pRight);

template<class T>
nsquat<T> inverse(const nsquat<T> & quat_);

template <class T>
nsquat<T> normalize(const nsquat<T> & quat_);

template <class T>
nsquat<T> slerp(const nsquat<T> & lhs_, const nsquat<T> & rhs_, const T & scaling_factor_);

template<class PUPer, class T>
void pup(PUPer & p, nsquat<T> & quat_);

template <class T>
struct nsquat
{
	nsquat(const nsquat<T> & copy_) : x(copy_.x), y(copy_.y), z(copy_.z), w(copy_.w) {}
	nsquat() : x(static_cast<T>(0)), y(static_cast<T>(0)), z(static_cast<T>(0)), w(static_cast<T>(1)) {}
	nsquat(const T & x_, const T & y_, const T & z_, const T & w_) : x(x_), y(y_), z(z_), w(w_) {}

	nsquat<T> & conjugate()
	{
		x *= static_cast<T>(-1);
		y *= static_cast<T>(-1);
		z *= static_cast<T>(-1);
		return *this;
	}

	nsquat<T> & from(const nsvec4<T> & axis_angle_, bool rads_ = false)
	{
		T ang = axis_angle_.w;
		ang /= static_cast<T>(2);

		if (!rads_)
			ang = ::radians(ang);

		T sAng = static_cast<T>(std::sin(ang));
		x = axis_angle_.x * sAng;
		y = axis_angle_.y * sAng;
		z = axis_angle_.z * sAng;
		w = static_cast<T>(std::cos(ang));
		return *this;
	}

	nsquat<T> & from(const nsvec3<T> & euler_, typename nsvec3<T>::EulerOrder order_, bool rads_ = false)
	{
		T c1, c2, c3, s1, s2, s3;
		if (!rads_)
		{
			nsvec3<T> eul(::radians(euler_));
			c1 = static_cast<T>(std::cos(eul.x / 2));
			c2 = static_cast<T>(std::cos(eul.y / 2));
			c3 = static_cast<T>(std::cos(eul.z / 2));
			s1 = static_cast<T>(std::sin(eul.x / 2));
			s2 = static_cast<T>(std::sin(eul.y / 2));
			s3 = static_cast<T>(std::sin(eul.z / 2));
		}
		else
		{
			c1 = static_cast<T>(std::cos(euler_.x / 2));
			c2 = static_cast<T>(std::cos(euler_.y / 2));
			c3 = static_cast<T>(std::cos(euler_.z / 2));
			s1 = static_cast<T>(std::sin(euler_.x / 2));
			s2 = static_cast<T>(std::sin(euler_.y / 2));
			s3 = static_cast<T>(std::sin(euler_.z / 2));
		}

		switch (order_)
		{
		case(nsvec3<T>::XYZ) :
		{
			x = s1 * c2 * c3 + c1 * s2 * s3;
			y = c1 * s2 * c3 - s1 * c2 * s3;
			z = c1 * c2 * s3 + s1 * s2 * c3;
			w = c1 * c2 * c3 - s1 * s2 * s3;
			break;
		}
		case(nsvec3<T>::XZY) :
		{
			x = s1 * c2 * c3 - c1 * s2 * s3;
			y = c1 * s2 * c3 - s1 * c2 * s3;
			z = c1 * c2 * s3 + s1 * s2 * c3;
			w = c1 * c2 * c3 + s1 * s2 * s3;
			break;
		}
		case(nsvec3<T>::YXZ) :
		{
			x = s1 * c2 * c3 + c1 * s2 * s3;
			y = c1 * s2 * c3 - s1 * c2 * s3;
			z = c1 * c2 * s3 - s1 * s2 * c3;
			w = c1 * c2 * c3 + s1 * s2 * s3;
			break;
		}
		case(nsvec3<T>::YZX) :
		{
			x = s1 * c2 * c3 + c1 * s2 * s3;
			y = c1 * s2 * c3 + s1 * c2 * s3;
			z = c1 * c2 * s3 - s1 * s2 * c3;
			w = c1 * c2 * c3 - s1 * s2 * s3;
			break;
		}
		case(nsvec3<T>::ZXY) :
		{
			x = s1 * c2 * c3 - c1 * s2 * s3;
			y = c1 * s2 * c3 + s1 * c2 * s3;
			z = c1 * c2 * s3 + s1 * s2 * c3;
			w = c1 * c2 * c3 - s1 * s2 * s3;
			break;
		}
		case(nsvec3<T>::ZYX) :
		{
			x = s1 * c2 * c3 - c1 * s2 * s3;
			y = c1 * s2 * c3 + s1 * c2 * s3;
			z = c1 * c2 * s3 - s1 * s2 * c3;
			w = c1 * c2 * c3 + s1 * s2 * s3;
			break;
		}
		}
		return *this;
	}

	nsquat<T> & from(const nsmat3<T> & rotation_mat3_)
	{
		T tr = rotation_mat3_[0][0] + rotation_mat3_[1][1] + rotation_mat3_[2][2], s;

		if (tr > 0)
		{
			s = sqrt(tr + 1.0) * 2;
			w = 0.25 * s;
			x = (rotation_mat3_[2][1] - rotation_mat3_[1][2]) / s;
			y = (rotation_mat3_[0][2] - rotation_mat3_[2][0]) / s;
			z = (rotation_mat3_[1][0] - rotation_mat3_[0][1]) / s;
		}
		else if ((rotation_mat3_[0][0] > rotation_mat3_[1][1])&(rotation_mat3_[0][0] > rotation_mat3_[2][2]))
		{
			s = sqrt(1.0 + rotation_mat3_[0][0] - rotation_mat3_[1][1] - rotation_mat3_[2][2]) * 2;
			w = (rotation_mat3_[2][1] - rotation_mat3_[1][2]) / s;
			x = 0.25 * s;
			y = (rotation_mat3_[0][1] + rotation_mat3_[1][0]) / s;
			z = (rotation_mat3_[0][2] + rotation_mat3_[2][0]) / s;
		}
		else if (rotation_mat3_[1][1] > rotation_mat3_[2][2])
		{
			s = sqrt(1.0 + rotation_mat3_[1][1] - rotation_mat3_[0][0] - rotation_mat3_[2][2]) * 2;
			w = (rotation_mat3_[0][2] - rotation_mat3_[2][0]) / s;
			x = (rotation_mat3_[0][1] + rotation_mat3_[1][0]) / s;
			y = 0.25 * s;
			z = (rotation_mat3_[1][2] + rotation_mat3_[2][1]) / s;
		}
		else
		{
			s = sqrt(1.0 + rotation_mat3_[2][2] - rotation_mat3_[0][0] - rotation_mat3_[1][1]) * 2;
			w = (rotation_mat3_[1][0] - rotation_mat3_[0][1]) / s;
			x = (rotation_mat3_[0][2] + rotation_mat3_[2][0]) / s;
			y = (rotation_mat3_[1][2] + rotation_mat3_[2][1]) / s;
			z = 0.25 * s;
		}
		return *this;
	}

	nsquat<T> & from(const nsmat4<T> & transform_)
	{
		return from(transform_.basis());
	}

	nsquat<T> & from(const nsvec3<T> & vec_, const nsvec3<T> & to_vec_)
	{
		/* http://www.euclideanspace.com/maths/algebra/vectors/angleBetween/index.htm */
		T real = 1 + vec_ * to_vec_;
		nsvec3<T> imag = cross(vec_, to_vec_);
		if (real < 0.0000001)
		{
			w = 0; x = -vec_.z; y = vec_.y; z = vec_.x;
			return (*this).normalize();
		}
		w = real; x = imag.x; y = imag.y; z = imag.z;
		return (*this).normalize();
	}

	nsquat<T> & invert()
	{
		return conjugate().normalize();
	}

	T length() const
	{
		return static_cast<T>(sqrt(x*x + y*y + z*z + w*w));
	}
	
	T length_sq() const
	{
		return x*x + y*y + z*z + w*w;
	}

	nsquat<T> & normalize()
	{
		T l = length();
		if (l == static_cast<T>(0))
			return set_indentity();
		return *this *= static_cast<T>(1) / l;
	}

	nsvec3<T> right() const
	{
		return nsvec3<T>(1 - 2*(y*y + z*z), 2*(x*y + z*w), 2*(x*z - y*w));
	}

	nsquat<T> & round_to_zero()
	{
		if (std::abs(x) < EPS_VEC_0)
			x = 0;
		if (std::abs(y) < EPS_VEC_0)
			y = 0;
		if (std::abs(x) < EPS_VEC_0)
			z = 0;
		if (std::abs(w) < EPS_VEC_0)
			w = 0;
		return *this;
	}

	nsquat<T> & set(const T & x_, const T & y_, const T & z_, const T & w_)
	{
		x = x_; y = y_; z = z_; w = w_;
		return *this;
	}

	nsquat<T> & set_indentity()
	{
		x = 0; y = 0; z = 0; z = 1;
		return *this;
	}

	nsquat<T> & slerp(nsquat<T> second_, const T & scaling_factor_)
	{
		if (scaling_factor_ == 0 )
			return *this;
		
		if ( scaling_factor_ == 1 )
			return (*this = second_);
		
		T cos_half_theta = w * second_.w + x * second_.x + y * second_.y + z * second_.z;

		if ( cos_half_theta < 0 )
		{
			second_ *= -1;
			cos_half_theta = -cos_half_theta;
		}
		
		if ( cos_half_theta >= 1.0 )
			return *this;
		
        T half_theta = static_cast<T>(acos(cos_half_theta));
        T sin_half_theta = static_cast<T>(sqrt( 1.0 - cos_half_theta * cos_half_theta ));

		if (std::abs(sin_half_theta) < EPS)
		{
			*this = 0.5 * (*this + second_);
			return *this;
		}
		
		T ratio_a = sin((1 - scaling_factor_) * half_theta) / sin_half_theta;
		T ratio_b = sin(scaling_factor_ * half_theta) / sin_half_theta;

		*this = *this * ratio_a + second_ * ratio_b;
		normalize();
		return *this;
	}

	nsvec3<T> target() const
	{
		return nsvec3<T>(2*(x*z + y*w), 2*(y*z - x*w), 1 - 2*(x*x + y*y));
	}

	nsstring to_string() const
	{
		nsstringstream ss;
		ss << "[" << w << " " << x << "i " << y << "j " << z << "k]";
		return ss.str();
	}

	nsvec3<T> up() const
	{
		return nsvec3<T>(2*(x*y - z*w), 1 - 2*(x*x + z*z), 2*(y*z + x*w));
	}

	// overloaded operators
	nsquat<T> operator+(const nsquat<T> & rhs_) const
	{
		return nsquat<T>(x + rhs_.x, y + rhs_.y, z + rhs_.z, w + rhs_.w);
	}

	nsquat<T> operator-(const nsquat<T> & rhs_) const
	{
		return nsquat<T>(x - rhs_.x, y - rhs_.y, z - rhs_.z, w - rhs_.w);
	}

	nsquat<T> operator*(const nsquat<T> & rhs_) const
	{
		nsquat<T> ret;
		ret.x = w*rhs_.x + x*rhs_.w + y*rhs_.z - z*rhs_.y;
		ret.y = w*rhs_.y - x*rhs_.z + y*rhs_.w + z*rhs_.x;
		ret.z = w*rhs_.z + x*rhs_.y - y*rhs_.x + z*rhs_.w;
		ret.w = w*rhs_.w - x*rhs_.x - y*rhs_.y - z*rhs_.z;
		return ret;
	}

	nsquat<T> operator/(const nsquat<T> & rhs_) const
	{
		return *this * inverse(rhs_);
	}

	nsquat<T> operator%(const nsquat<T> & rhs_) const // component wise scalar product
	{
		return nsquat<T>(x*rhs_.x, y*rhs_.y, z*rhs_.z, w*rhs_.w);
	}

	nsvec3<T> operator*(const nsvec3<T> & rhs_) const
	{
		nsvec3<T> ret;
		// quat_ * vec3
		T ix = w * rhs_.x + y * rhs_.z - z * rhs_.y;
		T iy = w * rhs_.y + z * rhs_.x - x * rhs_.z;
		T iz = w * rhs_.z + x * rhs_.y - y * rhs_.x;
		T iw = -x * rhs_.x - y * rhs_.y - z * rhs_.z;
		// vec3 * inverse(quat_)
		ret.x = ix * w + iw * -x + iy * -z - iz * -y;
		ret.y = iy * w + iw * -y + iz * -x - ix * -z;
		ret.z = iz * w + iw * -z + ix * -y - iy * -x;
		return ret;
	}

	nsquat<T> operator*(const T & rhs_) const
	{
		return nsquat<T>(x * rhs_, y * rhs_, z * rhs_, w * rhs_);
	}

	nsquat<T> operator/(const T & rhs_) const
	{
		return nsquat<T>(x / rhs_, y / rhs_, z / rhs_, w / rhs_);
	}

	nsquat<T> & operator=(const nsquat<T> & rhs_)
	{
		if (this == &rhs_)
			return *this;
		x = rhs_.x;
		y = rhs_.y;
		z = rhs_.z;
		w = rhs_.w;
		return *this;
	}

	nsquat<T> operator++(int32)
	{
		nsquat<T> ret(*this);
		++(*this);
		return ret;
	}

	nsquat<T> operator--(int32)
	{
		nsquat<T> ret(*this);
		--(*this);
		return ret;
	}

	nsquat<T> & operator++()
	{
		++x; ++y; ++z; ++w;
		return *this;
	}

	nsquat<T> & operator--()
	{
		--x; --y; --z; --w;
		return *this;
	}

	nsquat<T> & operator+=(const nsquat<T> & rhs_)
	{
		x += rhs_.x; y += rhs_.y; z += rhs_.z; w += rhs_.w;
		return *this;
	}

	nsquat<T> & operator-=(const nsquat<T> & rhs_)
	{
		x -= rhs_.x; y -= rhs_.y; z -= rhs_.z; w -= rhs_.w;
		return *this;
	}

	nsquat<T> & operator*=(const nsquat<T> & rhs_)
	{
		return *this = *this * rhs_;
	}

	nsquat<T> & operator/=(const nsquat<T> & rhs_)
	{
		return *this = *this * inverse(rhs_);
	}

	nsquat<T> & operator%=(const nsquat<T> & rhs_)
	{
		x *= rhs_.x; y *= rhs_.y; z *= rhs_.z; w *= rhs_.w;
		return *this;
	}

	nsquat<T> & operator*=(const T & rhs_)
	{
		x *= rhs_; y *= rhs_; z *= rhs_; w *= rhs_;
		return *this;
	}

	nsquat<T> & operator/=(const T & rhs_)
	{
		x /= rhs_; y /= rhs_; z /= rhs_; w /= rhs_;
		return *this;
	}

	bool operator==(const nsquat<T> & rhs_) const
	{
		return ((x == rhs_.x) && (y == rhs_.y) && (z == rhs_.z) && (w == rhs_.w));
	}

	bool operator!=(const nsquat<T> & rhs_) const
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
			throw(std::out_of_range("quat_ index out of range"));
		return data[val_];
	}

	T & operator[](const uint32 & val_)
	{
		if (val_ > 3)
			throw(std::out_of_range("quat_ index out of range"));
		return data[val_];
	}

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
			T b;
			T c;
			T d;
			T a;
		};

		struct
		{
			T i;
			T j;
			T k;
			T alpha;
		};

	};
};

template <class T>
nsquat<T> operator*(const int32 & lhs_, const nsquat<T> & rhs_)
{
	return rhs_ * static_cast<T>(lhs_);
}

template <class T>
nsquat<T> operator*(const float & lhs_, const nsquat<T> & rhs_)
{
	return rhs_ * static_cast<T>(lhs_);
}

template <class T>
nsquat<T> operator*(const double & lhs_, const nsquat<T> & rhs_)
{
	return rhs_ * static_cast<T>(lhs_);
}

template <class T>
nsquat<T> operator/(const int32 & lhs_, const nsquat<T> & rhs_)
{
	return nsquat<T>(lhs_ / rhs_.x, lhs_ / rhs_.y, lhs_ / rhs_.z, lhs_ / rhs_.w);
}

template <class T>
nsquat<T> operator/(const float & lhs_, const nsquat<T> & rhs_)
{
	return nsquat<T>(lhs_ / rhs_.x, lhs_ / rhs_.y, lhs_ / rhs_.z, lhs_ / rhs_.w);
}

template <class T>
nsquat<T> operator/(const double & lhs_, const nsquat<T> & rhs_)
{
	return nsquat<T>(lhs_ / rhs_.x, lhs_ / rhs_.y, lhs_ / rhs_.z, lhs_ / rhs_.w);
}

template <class T>
T dot(const nsquat<T> & lhs_, const nsquat<T> & rhs_)
{
	return lhs_.a*rhs_.a + lhs_.b*rhs_.b + lhs_.c*rhs_.c + lhs_.d*rhs_.d;
}

template<class T>
nsquat<T> conjugate(const nsquat<T> & quat_)
{
	return nsquat<T>(quat_).conjugate();
}

template<class T>
nsquat<T> inverse(const nsquat<T> & quat_)
{
	return nsquat<T>(quat_).invert();
}

template <class T>
nsquat<T> normalize(const nsquat<T> & quat_)
{
	return nsquat<T>(quat_).normalize();
}

template<class T>
nsquat<T> orientation(const nsmat3<T> & rotation_mat3_)
{
	return nsquat<T>().from(rotation_mat3_);
}

template<class T>
nsquat<T> orientation(const nsvec4<T> & axis_angle_, bool rads_)
{
	return nsquat<T>().from(axis_angle_, rads_);
}

template<class T>
nsquat<T> orientation(const nsvec3<T> & euler_, typename nsvec3<T>::EulerOrder order_, bool rads_)
{
	return nsquat<T>().from(euler_,order_,rads_);
}

template<class T>
nsquat<T> orientation(const nsvec3<T> & vec_, const nsvec3<T> & to_vec_)
{
	return nsquat<T>().from(vec_, to_vec_);
}

template<class T>
nsquat<T> orientation(const nsmat4<T> & transform_)
{
	return nsquat<T>().from(transform_);
}

template <class T>
nsquat<T> slerp(const nsquat<T> & lhs_, const nsquat<T> & rhs_, const T & scaling_factor_)
{
	nsquat<T> ret(lhs_);
	ret.slerp(rhs_, scaling_factor_);
	return ret;
}

template<class PUPer, class T>
void pup(PUPer & p, nsquat<T> & quat_, const nsstring & var_name_)
{
	pup(p, quat_.x, var_name_ + ".x"); pup(p, quat_.y, var_name_ + ".y"); pup(p, quat_.z, var_name_ + ".z"); pup(p, quat_.w, var_name_ + ".w");
}

#endif
