#ifndef NSMAT3_H
#define NSMAT3_H

#include <math/nsmat2.h>
#include <iomanip>

template <class T>
nsvec3<T> operator*(const nsvec3<T> & lhs_, const nsmat3<T> & rhs_);

template <class T>
nsvec3<T> operator/(const nsvec3<T> & lhs_, const nsmat3<T> & rhs_);

template <class T>
nsmat3<T> operator%(const nsvec3<T> & lhs_, const nsmat3<T> & rhs_);

template <class T>
nsmat3<T> operator*(const int32 & lhs_, const nsmat3<T> & rhs_);

template <class T>
nsmat3<T> operator*(const float & lhs_, const nsmat3<T> & rhs_);

template <class T>
nsmat3<T> operator*(const double & lhs_, const nsmat3<T> & rhs_);

template<class T>
nsmat3<T> operator/(const int32 & lhs_, const nsmat3<T> & rhs_);

template <class T>
nsmat3<T> operator/(const float & lhs_, const nsmat3<T> & rhs_);

template <class T>
nsmat3<T> operator/(const double & lhs_, const nsmat3<T> & rhs_);

template <class T>
T determinant(const nsmat3<T> & mat_);

template <class T>
nsmat3<T> rotation2d_mat3(const T angle_, bool rads_ = false);

template <class T>
nsmat3<T> rotation2d_mat3(const nsmat3<T> & transform2d_);

template <class T>
nsmat3<T> rotation2d_mat3(const nsmat2<T> & transform2d_);

template<class T>
nsmat3<T> rotation_mat3(const nsvec4<T> & axis_angle_, bool rads_ = false);

template<class T>
nsmat3<T> rotation_mat3(const nsvec3<T> & euler_, typename nsvec3<T>::EulerOrder order_ , bool rads_ = false);

template<class T>
nsmat3<T> rotation_mat3(const nsquat<T> & orientation_);

template<class T>
nsmat3<T> rotation_mat3(const nsvec3<T> & vec_, const nsvec3<T> & to_vec_);

template<class T>
nsmat3<T> rotation_mat3(const nsmat4<T> & transform_);

template<class T>
nsmat3<T> scaling2d_mat3(const nsvec2<T> & scale_);

template<class T>
nsmat3<T> scaling2d_mat3(const nsmat2<T> & transform2d_);

template<class T>
nsmat3<T> scaling2d_mat3(const nsmat3<T> & transform2d_);

template <class T>
nsmat3<T> scaling_mat3(const nsvec3<T> & scale_);

template <class T>
nsmat3<T> scaling_mat3(const nsmat3<T> & transform_);

template <class T>
nsmat3<T> scaling_mat3(const nsmat4<T> & transform_);

template <class T>
nsmat3<T> translation2d_mat3(const nsvec3<T> & m3_);

template <class T>
nsmat3<T> translation2d_mat3(const nsvec2<T> & m2_);

template <class T>
nsmat3<T> transpose(const nsmat3<T> mat_);

template <class T>
nsmat3<T> inverse(const nsmat3<T> mat_);

template<class PUPer, class T>
void pup(PUPer & p, nsmat3<T> & m3);

template <class T>
struct nsmat3
{
	nsmat3()
	{
		set_identity();
	}

	nsmat3(const T & val_)
	{
		set(val_);
	}

	nsmat3(const nsmat3 & copy_)
	{
		set(copy_.data[0], copy_.data[1], copy_.data[2]);
	}

	nsmat3(
		const T & a, const T & b, const T & c,
		const T & d, const T & e, const T & f,
		const T & g, const T & h, const T & i)
	{
		set(a, b, c, d, e, f, g, h, i);
	}

	nsmat3(const nsvec3<T> & row1, const nsvec3<T> & row2, const nsvec3<T> & row3)
	{
		set(row1, row2, row3);
	}

	nsmat3(const nsmat2<T> & basis_)
	{
		set(basis_);
		data[0][2] = static_cast<T>(0); data[1][2] = static_cast<T>(0);
		data[2][0] = static_cast<T>(0); data[2][1] = static_cast<T>(0); data[2][2] = static_cast<T>(1);
	}

	nsmat2<T> basis() const
	{
		return nsmat2<T>(data[0][0], data[0][1], data[1][0], data[1][1]);
	}

	T * data_ptr()
	{
		return &data[0][0];
	}

	T determinant() const
	{
		return
			data[0][0] * data[1][1] * data[2][2] - data[0][0] * data[1][2] * data[2][1] +
			data[0][1] * data[1][2] * data[2][0] - data[0][1] * data[1][0] * data[2][2] +
			data[0][2] * data[1][0] * data[2][1] - data[0][2] * data[1][1] * data[2][0];
	}

	nsmat3<T> & invert()
	{
		T det = determinant();

		if (det == static_cast<T>(0))
			return set(0);

		nsmat3 ret;
		ret[0].set(
			data[1][1] * data[2][2] - data[1][2] * data[2][1],
			data[0][2] * data[2][1] - data[0][1] * data[2][2],
			data[0][1] * data[1][2] - data[0][2] * data[1][1]);

		ret[1].set(
			data[1][2] * data[2][0] - data[1][0] * data[2][2],
			data[0][0] * data[2][2] - data[0][2] * data[2][0],
			data[0][2] * data[1][0] - data[0][0] * data[1][2]);

		ret[2].set(
			data[1][0] * data[2][1] - data[1][1] * data[2][0],
			data[0][1] * data[2][0] - data[0][0] * data[2][1],
			data[0][0] * data[1][1] - data[0][1] * data[1][0]);

		ret /= determinant();
		return (*this = ret);
	}

	nsvec3<T> right() const
	{
		return normalize((*this)(0));
	}

	nsmat3<T> & rotation2d_from(T angle_, bool rads_ = false)
	{
		if (!rads_)
			angle_ = radians(angle_);

		data[0][0] = static_cast<T>(std::cos(angle_)); data[0][1] = static_cast<T>(std::sin(angle_));
		data[1][0] = static_cast<T>(-std::sin(angle_)); data[1][1] = static_cast<T>(std::cos(angle_));
		data[0][2] = data[1][2] = data[2][0] = data[2][1] = static_cast<T>(0); data[2][2] = static_cast<T>(1);
		return *this;
	}

	nsmat3<T> & rotation2d_from(const nsmat3<T> & transform2d_)
	{
		*this = transform2d_;
		data[0][2] = data[1][2] = data[2][0] = data[2][1] = static_cast<T>(0); data[2][2] = static_cast<T>(1);
		data[0].normalize();
		data[1].normalize();
		return *this;
	}

	nsmat3<T> & rotation2d_from(const nsmat2<T> & transform2d_)
	{
		set(transform2d_);
		data[0][2] = data[1][2] = data[2][0] = data[2][1] = static_cast<T>(0); data[2][2] = static_cast<T>(1);
		data[0].normalize();
		data[1].normalize();
		return *this;
	}

	nsmat3<T> & rotation_from(const nsvec4<T> & axis_angle_, bool rads_ = false)
	{
		// http://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToMatrix/index.htm

		T angle_ = axis_angle_.a;

		if (!rads_)
			angle_ = radians(angle_);

		T c = static_cast<T>(std::cos(angle_));
		T s = static_cast<T>(std::sin(angle_));
		T t = static_cast<T>(1) - c;

		data[0][0] = c + axis_angle_.x * axis_angle_.x * t;
		data[1][1] = c + axis_angle_.y * axis_angle_.y * t;
		data[2][2] = c + axis_angle_.z * axis_angle_.z * t;

		T tmp1 = axis_angle_.x * axis_angle_.y * t;
		T tmp2 = axis_angle_.z * s;
		data[1][0] = tmp1 + tmp2;
		data[0][1] = tmp1 - tmp2;

		tmp1 = axis_angle_.x * axis_angle_.z * t;
		tmp2 = axis_angle_.y * s;
		data[2][0] = tmp1 - tmp2;
		data[0][2] = tmp1 + tmp2;

		tmp1 = axis_angle_.y * axis_angle_.z * t;
		tmp2 = axis_angle_.x * s;
		data[2][1] = tmp1 + tmp2;
		data[1][2] = tmp1 - tmp2;

		return *this;
	}

	nsmat3<T> & rotation_from(const nsvec3<T> & euler_, typename nsvec3<T>::EulerOrder order_ , bool rads_ = false)
	{
		T cb, sb, ch, sh, ca, sa;
		if (!rads_)
		{
			nsvec3<T> eulRads(radians(euler_));
			cb = static_cast<T>(std::cos(eulRads.x));
			sb = static_cast<T>(std::sin(eulRads.x));
			ch = static_cast<T>(std::cos(eulRads.y));
			sh = static_cast<T>(std::sin(eulRads.y));
			ca = static_cast<T>(std::cos(eulRads.z));
			sa = static_cast<T>(std::sin(eulRads.z));
		}
		else
		{
			cb = static_cast<T>(std::cos(euler_.x));
			sb = static_cast<T>(std::sin(euler_.x));
			ch = static_cast<T>(std::cos(euler_.y));
			sh = static_cast<T>(std::sin(euler_.y));
			ca = static_cast<T>(std::cos(euler_.z));
			sa = static_cast<T>(std::sin(euler_.z));
		}

		switch (order_)
		{
		case(nsvec3<T>::XYZ) :
		{
			data[0][0] = ch * ca;
			data[0][1] = -ch * sa;
			data[0][2] = sh;

			data[1][0] = cb * sa + sb * ca * sh;
			data[1][1] = cb * ca - sb * sa * sh;
			data[1][2] = -sb * ch;

			data[2][0] = sb * sa - cb * ca * sh;
			data[2][1] = sb * ca + cb * sa * sh;
			data[2][2] = cb * ch;
			break;
		}
		case(nsvec3<T>::XZY) :
		{
			data[0][0] = ch * ca;
			data[0][1] = -sa;
			data[0][2] = sh * ca;

			data[1][0] = cb * ch * sa + sb * sh;
			data[1][1] = cb * ca;
			data[1][2] = cb * sh * sa - sb * ch;

			data[2][0] = sb * ch * sa - cb * sh;
			data[2][1] = sb * ca;
			data[2][2] = sb * sh * sa + cb * ch;
			break;
		}
		case(nsvec3<T>::YXZ) :
		{
			data[0][0] = ch * ca + sh * sa * sb;
			data[0][1] = sh * ca * sb - ch * sa;
			data[0][2] = cb * sh;

			data[1][0] = cb * sa;
			data[1][1] = cb * ca;
			data[1][2] = -sb;

			data[2][0] = ch * sa * sb - sh * ca;
			data[2][1] = sh * sa + ch * ca * sb;
			data[2][2] = cb * ch;
			break;
		}
		case(nsvec3<T>::YZX) :
		{
			data[0][0] = ch * ca;
			data[0][1] = sb * sh - cb * ch * sa;
			data[0][2] = sb * ch * sa + cb * sh;

			data[1][0] = sa;
			data[1][1] = cb * ca;
			data[1][2] = -sb * ca;

			data[2][0] = -sh * ca;
			data[2][1] = cb * sh * sa + sb * ch;
			data[2][2] = cb * ch - sb * sh * sa;
			break;
		}
		case(nsvec3<T>::ZXY) :
		{
			data[0][0] = ch * ca - sh * sa * sb;
			data[0][1] = -cb * sa;
			data[0][2] = sh * ca + ch * sa * sb;

			data[1][0] = ch * sa + sh * ca * sb;
			data[1][1] = cb * ca;
			data[1][2] = sh * sa - ch * ca * sb;

			data[2][0] = -cb * sh;
			data[2][1] = sb;
			data[2][2] = cb * ch;
			break;
		}
		case(nsvec3<T>::ZYX) :
		{
			data[0][0] = ch * ca;
			data[0][1] = sb * ca * sh - cb * sa;
			data[0][2] = cb * ca * sh + sb * sa;

			data[1][0] = ch * sa;
			data[1][1] = sb * sa * sh + cb * ca;
			data[1][2] = cb * sa * sh - sb * ca;

			data[2][0] = -sh;
			data[2][1] = sb * ch;
			data[2][2] = cb * ch;
			break;
		}
		}
		return round_to_zero();
	}

	nsmat3<T> & rotation_from(const nsquat<T> & ornt_)
	{
		data[0][0] = 1 - 2*(ornt_.y*ornt_.y + ornt_.z*ornt_.z);
		data[0][1] = 2*(ornt_.x*ornt_.y - ornt_.z*ornt_.w);
		data[0][2] = 2*(ornt_.x*ornt_.z + ornt_.y*ornt_.w);

		data[1][0] = 2*(ornt_.x*ornt_.y + ornt_.z*ornt_.w);
		data[1][1] = 1 - 2*(ornt_.x*ornt_.x + ornt_.z*ornt_.z);
		data[1][2] = 2*(ornt_.y*ornt_.z - ornt_.x*ornt_.w);

		data[2][0] = 2*(ornt_.x*ornt_.z - ornt_.y*ornt_.w);
		data[2][1] = 2*(ornt_.y*ornt_.z + ornt_.x*ornt_.w);
		data[2][2] = 1 - 2*(ornt_.x*ornt_.x + ornt_.y*ornt_.y);
		return *this;
	}

	nsmat3<T> & rotation_from(const nsmat3<T> & transform_)
	{
		*this = transform_;
		data[0].normalize();
		data[1].normalize();
		data[2].normalize();
		return *this;
	}

	nsmat3<T> & rotation_from(const nsmat4<T> & transform_)
	{
		*this = transform_.basis();
		data[0].normalize();
		data[1].normalize();
		data[2].normalize();
		return *this;
	}

	nsmat3<T> & rotation_from(const nsvec3<T> & vec_, const nsvec3<T> & to_vec_)
	{
		/* http://www.euclideanspace.com/maths/algebra/vectors/angleBetween/index.htm */
		nsvec3<T> vs = cross(vec_, to_vec_);
		T ca = vec_ * to_vec_;
		nsvec3<T> v = normalize(vs);
		nsvec3<T> vt = v * (static_cast<T>(1) - ca);

		data[0][0] = vt.x * v.x + ca;
		data[1][1] = vt.y * v.y + ca;
		data[2][2] = vt.z * v.z + ca;
		vt.x *= v.y;
		vt.z *= v.x;
		vt.y *= v.z;
		data[0][1] = vt.x - vs.z;
		data[0][2] = vt.z + vs.y;
		data[1][0] = vt.x + vs.z;
		data[1][2] = vt.y - vs.x;
		data[2][0] = vt.z - vs.y;
		data[2][1] = vt.y + vs.x;
		return *this;
	}

	nsmat3<T> & round_to_zero()
	{
		data[0].round_to_zero();
		data[1].round_to_zero();
		data[2].round_to_zero();
		return *this;
	}

	nsmat3<T> & scaling2d_from(const nsvec2<T> & scale_)
	{
		set_identity();
		data[0][0] = scale_.x; data[1][1] = scale_.y;
		return *this;
	}

	nsmat3<T> & scaling2d_from(const nsmat2<T> & transform2d_)
	{
		nsvec2<T> scalingVec(transform2d_[0].length(), transform2d_[1].length());
		return scaling_from(scalingVec);
	}

	nsmat3<T> & scaling2d_from(const nsmat3<T> & transform2d_)
	{
		nsvec2<T> scalingVec;
		scalingVec.x = sqrt(transform2d_[0][0] * transform2d_[0][0] + transform2d_[0][1] * transform2d_[0][1]);
		scalingVec.y = sqrt(transform2d_[1][0] * transform2d_[1][0] + transform2d_[1][1] * transform2d_[1][1]);
		return scaling_from(scalingVec);
	}

	nsmat3<T> & scaling_from(const nsvec3<T> & scale_)
	{
		set_identity();
		data[0][0] = scale_.x; data[1][1] = scale_.y; data[2][2] = scale_.z;
		return *this;
	}

	nsmat3<T> & scaling_from(const nsmat3<T> & transform_)
	{
		nsvec3<T> scalingVec(transform_[0].length(), transform_[1].length(), transform_[2].length());		
		return scaling_from(scalingVec);
	}

	nsmat3<T> & scaling_from(const nsmat4<T> & transform_)
	{
		nsvec3<T> scalingVec;
		scalingVec.x = sqrt(transform_[0][0] * transform_[0][0] + transform_[0][1] * transform_[0][1] + transform_[0][2] * transform_[0][2]);
		scalingVec.y = sqrt(transform_[1][0] * transform_[1][0] + transform_[1][1] * transform_[1][1] + transform_[1][2] * transform_[1][2]);
		scalingVec.z = sqrt(transform_[2][0] * transform_[2][0] + transform_[2][1] * transform_[2][1] + transform_[2][2] * transform_[2][2]);
		return scaling_from(scalingVec);
	}

	nsmat3<T> & set(
		const T & a, const T & b, const T & c,
		const T & d, const T & e, const T & f,
		const T & g, const T & h, const T & i)
	{
		data[0][0] = a; data[0][1] = b; data[0][2] = c;
		data[1][0] = d; data[1][1] = e; data[1][2] = f;
		data[2][0] = g; data[2][1] = h; data[2][2] = i;
		return *this;
	}

	nsmat3<T> & set(const T & val_)
	{
		data[0].set(val_);
		data[1].set(val_);
		data[2].set(val_);
		return *this;
	}

	nsmat3<T> & set(const nsvec3<T> & row1, const nsvec3<T> & row2, const nsvec3<T> & row3)
	{
		data[0] = row1;
		data[1] = row2;
		data[2] = row3;
		return *this;
	}

	nsmat3<T> & set(const nsmat2<T> & basis_)
	{
		data[0][0] = basis_[0][0]; data[0][1] = basis_[0][1];
		data[1][0] = basis_[1][0]; data[1][1] = basis_[1][1];
		return *this;
	}

	nsmat3<T> & set_column(const uint32 & i, const T & x, const T & y,const T & z)
	{
		(*this)[0][i] = x; (*this)[1][i] = y; (*this)[2][i] = z;
		return *this;
	}

	nsmat3<T> & set_column(const uint32 & i, const nsvec3<T> & col)
	{
		(*this)[0][i] = col.x; (*this)[1][i] = col.y; (*this)[2][i] = col.z;
		return *this;
	}

	nsmat3<T> & set_identity()
	{
		data[0].set(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0));
		data[1].set(static_cast<T>(0), static_cast<T>(1), static_cast<T>(0));
		data[2].set(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1));
		return *this;
	}

	nsvec3<T> target() const
	{
		return normalize((*this)(2));			
	}

	nsmat3<T> & translation2d_from(const nsvec3<T> & m3_)
	{
		set_identity();
		return set_column(2, m3_.x, m3_.y, 1);
	}

	nsmat3<T> & translation2d_from(const nsvec2<T> & m2_)
	{
		set_identity();
		return set_column(2, m2_.x, m2_.y, 1);
	}

	nsmat3<T> & transpose()
	{
		T tmp;
		tmp = data[1][0]; data[1][0] = data[0][1]; data[0][1] = tmp;
		tmp = data[2][0]; data[2][0] = data[0][2]; data[0][2] = tmp;
		tmp = data[2][1]; data[2][1] = data[1][2]; data[1][2] = tmp;
		return *this;
	}

	nsstring to_string(bool newline_=true) const
	{
		nsstringstream ss;
		nsstring lc = ";";

		if (newline_)
			lc += '\n';

		ss << std::left
		   << "|" << std::setw(6) << data[0][0]
		   << " " << std::setw(6) << data[0][1]
		   << " " << std::setw(6) << data[0][2] << lc
		   << "|" << std::setw(6) << data[1][0]
		   << " " << std::setw(6) << data[1][1]
		   << " " << std::setw(6) << data[1][2] << lc
		   << "|" << std::setw(6) << data[2][0]
		   << " " << std::setw(6) << data[2][1]
		   << " " << std::setw(6) << data[2][2] << " |";
		return ss.str();
	}

	nsvec3<T> up() const
	{
		return normalize((*this)(1));
	}

	// Overloaded operators
	nsmat3<T> operator*(const nsmat3<T> & rhs_) const
	{
		nsmat3<T> ret;
		
		ret.data[0][0] = data[0][0] * rhs_.data[0][0] + data[0][1] * rhs_.data[1][0] + data[0][2] * rhs_.data[2][0];
		ret.data[0][1] = data[0][0] * rhs_.data[0][1] + data[0][1] * rhs_.data[1][1] + data[0][2] * rhs_.data[2][1];
		ret.data[0][2] = data[0][0] * rhs_.data[0][2] + data[0][1] * rhs_.data[1][2] + data[0][2] * rhs_.data[2][2];

		ret.data[1][0] = data[1][0] * rhs_.data[0][0] + data[1][1] * rhs_.data[1][0] + data[1][2] * rhs_.data[2][0];
		ret.data[1][1] = data[1][0] * rhs_.data[0][1] + data[1][1] * rhs_.data[1][1] + data[1][2] * rhs_.data[2][1];
		ret.data[1][2] = data[1][0] * rhs_.data[0][2] + data[1][1] * rhs_.data[1][2] + data[1][2] * rhs_.data[2][2];

		ret.data[2][0] = data[2][0] * rhs_.data[0][0] + data[2][1] * rhs_.data[1][0] + data[2][2] * rhs_.data[2][0];
		ret.data[2][1] = data[2][0] * rhs_.data[0][1] + data[2][1] * rhs_.data[1][1] + data[2][2] * rhs_.data[2][1];
		ret.data[2][2] = data[2][0] * rhs_.data[0][2] + data[2][1] * rhs_.data[1][2] + data[2][2] * rhs_.data[2][2];

		return ret;
	}

	nsmat3<T> operator/(const nsmat3<T> & rhs_) const
	{
		return *this * inverse(rhs_);
	}

	nsmat3<T> operator%(const nsmat3<T> & rhs_) const
	{
		nsmat3<T> ret;
		ret.data[0] = data[0] % rhs_.data[0];
		ret.data[1] = data[1] % rhs_.data[1];
		ret.data[2] = data[2] % rhs_.data[2];
		return ret;
	}

	nsvec3<T> operator*(const nsvec3<T> & rhs_) const
	{
		return nsvec3<T>(data[0] * rhs_, data[1] * rhs_, data[2] * rhs_);
	}

	nsmat3<T> operator%(const nsvec3<T> & rhs_) const
	{
		return nsmat3<T>(data[0] % rhs_, data[1] % rhs_, data[2] % rhs_);
	}

	nsmat3<T> operator/(const nsvec3<T> & rhs_) const
	{
		nsmat3<T> ret;
		ret.data[0] = data[0] / rhs_[0];
		ret.data[1] = data[1] / rhs_[1];
		ret.data[2] = data[2] / rhs_[2];
		return ret;
	}

	nsmat3<T> operator*(const T & rhs_) const
	{
		nsmat3<T> ret;
		ret.data[0] = data[0] * rhs_;
		ret.data[1] = data[1] * rhs_;
		ret.data[2] = data[2] * rhs_;
		return ret;
	}

	nsmat3<T> operator/(const T & rhs_) const
	{
		nsmat3<T> ret;
		ret.data[0] = data[0] / rhs_;
		ret.data[1] = data[1] / rhs_;
		ret.data[2] = data[2] / rhs_;
		return ret;
	}
	
	nsmat3<T> operator+(const nsmat3<T> & rhs_) const
	{
		nsmat3<T> ret;
		ret.data[0] = data[0] + rhs_.data[0];
		ret.data[1] = data[1] + rhs_.data[1];
		ret.data[2] = data[2] + rhs_.data[2];
		return ret;
	}

	nsmat3<T> operator-(const nsmat3<T> & rhs_) const
	{
		nsmat3<T> ret;
		ret.data[0] = data[0] - rhs_.data[0];
		ret.data[1] = data[1] - rhs_.data[1];
		ret.data[2] = data[2] - rhs_.data[2];
		return ret;
	}

	bool operator==(const nsmat3<T> & rhs_) const
	{
		return (data[0] == rhs_.data[0] && data[1] == rhs_.data[1] && data[2] == rhs_.data[2]);
	}

	bool operator!=(const nsmat3<T> & rhs_) const
	{
		return !(*this == rhs_);
	}

	nsmat3<T> & operator=(const nsmat3<T> & rhs_)
	{
		if (this == &rhs_)
			return *this;

		data[0] = rhs_.data[0];
		data[1] = rhs_.data[1];
		data[2] = rhs_.data[2];
		return *this;
	}

	nsmat3<T> & operator*=(const nsmat3<T> & rhs_)
	{
		*this = *this * rhs_;
		return *this;
	}

	nsmat3<T> & operator/=(const nsmat3<T> & rhs_)
	{
		*this = *this / rhs_;
		return *this;
	}

	nsmat3<T> & operator%=(const nsmat3<T> & rhs_)
	{
		*this = *this % rhs_;
		return *this;
	}

	nsmat3<T> & operator%=(const nsvec3<T> & rhs_)
	{
		*this = *this % rhs_;
		return *this;
	}

	nsmat3<T> & operator/=(const nsvec3<T> & rhs_)
	{
		*this = *this / rhs_;
		return *this;
	}

	nsmat3<T> & operator*=(const T & rhs_)
	{
		*this = *this % rhs_;
		return *this;
	}

	nsmat3<T> & operator/=(const T & rhs_)
	{
		*this = *this / rhs_;
		return *this;
	}

	nsmat3<T> operator++(int32)
	{
		nsmat3<T> ret(*this);
		++(*this);
		return ret;
	}

	nsmat3<T> operator--(int32)
	{
		nsmat3<T> ret(*this);
		--(*this);
		return ret;
	}

	nsmat3<T> & operator++()
	{
		++data[0]; ++data[1]; ++data[2];
		return *this;
	}

	nsmat3<T> & operator--()
	{
		--data[0]; --data[1]; --data[2];
		return *this;
	}

	nsmat3<T> & operator+=(const nsmat3<T> & rhs_)
	{
		*this = *this + rhs_;
		return *this;
	}

	nsmat3<T> & operator-=(const nsmat3<T> & rhs_)
	{
		*this = *this - rhs_;
		return *this;
	}

	const nsvec3<T> & operator[](const uint32 & val_) const
	{
		if (val_ > 2)
			throw(std::out_of_range("mat3 index out of range"));
		return data[val_];
	}

	nsvec3<T> & operator[](const uint32 & val_)
	{
		if (val_ > 2)
			throw(std::out_of_range("mat3 index out of range"));
		return data[val_];
	}

	nsvec3<T> operator()(uint32 val_) const
	{
		return nsvec3<T>(data[0][val_], data[1][val_], data[2][val_]);
	}

private:
	nsvec3<T> data[3];
};

template <class T>
nsmat3<T> operator*(const int32 & lhs_, const nsmat3<T> & rhs_)
{
	return rhs_ * static_cast<T>(lhs_);
}

template <class T>
nsmat3<T> operator*(const float & lhs_, const nsmat3<T> & rhs_)
{
	return rhs_ * static_cast<T>(lhs_);
}

template <class T>
nsmat3<T> operator*(const double & lhs_, const nsmat3<T> & rhs_)
{
	return rhs_ * static_cast<T>(lhs_);
}

template<class T>
nsmat3<T> operator/(const int32 & lhs_, const nsmat3<T> & rhs_)
{
	return nsmat3<T>(lhs_ / rhs_[0], lhs_ / rhs_[1], lhs_ / rhs_[2]);
}

template <class T>
nsmat3<T> operator/(const float & lhs_, const nsmat3<T> & rhs_)
{
	return nsmat3<T>(lhs_ / rhs_[0], lhs_ / rhs_[1], lhs_ / rhs_[2]);
}

template <class T>
nsmat3<T> operator/(const double & lhs_, const nsmat3<T> & rhs_)
{
	return nsmat3<T>(lhs_ / rhs_[0], lhs_ / rhs_[1], lhs_ / rhs_[2]);
}

template <class T>
nsvec3<T> operator*(const nsvec3<T> & lhs_, const nsmat3<T> & rhs_)
{
	return nsvec3<T>(
		lhs_[0] * rhs_[0][0] + lhs_[1] * rhs_[1][0] + lhs_[2] * rhs_[2][0],
		lhs_[0] * rhs_[0][1] + lhs_[1] * rhs_[1][1] + lhs_[2] * rhs_[2][1],
		lhs_[0] * rhs_[0][2] + lhs_[1] * rhs_[1][2] + lhs_[2] * rhs_[2][2]);
}

template <class T>
nsvec3<T> operator/(const nsvec3<T> & lhs_, const nsmat3<T> & rhs_)
{
	return lhs_ * inverse(rhs_);
}

template <class T>
nsmat3<T> operator%(const nsvec3<T> & lhs_, const nsmat3<T> & rhs_)
{
	return nsmat3<T>(rhs_[0] * lhs_[0], rhs_[1] * lhs_[1], rhs_[2] * lhs_[2]);
}

template <class T>
T determinant(const nsmat3<T> & mat_)
{
	return mat_.determinant();
}

template <class T>
nsmat3<T> rotation2d_mat3(T angle_, bool rads_)
{
	return nsmat3<T>().rotation2d_from(angle_, rads_);
}

template <class T>
nsmat3<T> rotation2d_mat3(const nsmat3<T> & transform2d_)
{
	return nsmat3<T>().rotation2d_from(transform2d_);
}

template <class T>
nsmat3<T> rotation2d_mat3(const nsmat2<T> & transform2d_)
{
	return nsmat3<T>().rotation2d_from(transform2d_);
}

template<class T>
nsmat3<T> rotation_mat3(const nsvec4<T> & axis_angle_, bool rads_)
{
	return nsmat3<T>().rotation_from(axis_angle_, rads_);
}

template<class T>
nsmat3<T> rotation_mat3(const nsvec3<T> & euler_, typename nsvec3<T>::EulerOrder order_, bool rads_)
{
	return nsmat3<T>().rotation_from(euler_,order_, rads_);
}

template<class T>
nsmat3<T> rotation_mat3(const nsquat<T> & orientation_)
{
	return nsmat3<T>().rotation_from(orientation_);
}

template<class T>
nsmat3<T> rotation_mat3(const nsvec3<T> & vec_, const nsvec3<T> & to_vec_)
{
	return nsmat3<T>().rotation_from(vec_, to_vec_);
}

template<class T>
nsmat3<T> rotation_mat3(const nsmat4<T> & transform_)
{
	return nsmat3<T>().rotation_from(transform_);
}

template<class T>
nsmat3<T> rotation_mat3(const nsmat3<T> & transform_)
{
	return nsmat3<T>().rotation_from(transform_);
}

template<class T>
nsmat3<T> scaling2d_mat3(const nsvec2<T> & scale_)
{
	return nsmat3<T>().scaling2d_from(scale_);
}

template<class T>
nsmat3<T> scaling2d_mat3(const nsmat2<T> & transform2d_)
{
	return nsmat3<T>().scaling2d_from(transform2d_);
}

template<class T>
nsmat3<T> scaling2d_mat3(const nsmat3<T> & transform2d_)
{
	return nsmat3<T>().scaling2d_from(transform2d_);
}

template <class T>
nsmat3<T> scaling_mat3(const nsvec3<T> & scale_)
{
	return nsmat3<T>().scaling_from(scale_);
}

template <class T>
nsmat3<T> scaling_mat3(const nsmat3<T> & transform_)
{
	return nsmat3<T>().scaling_from(transform_);
}

template <class T>
nsmat3<T> scaling_mat3(const nsmat4<T> & transform_)
{
	return nsmat3<T>().scaling_from(transform_);
}

template <class T>
nsmat3<T> translation2d_mat3(const nsvec3<T> & m3_)
{
	return nsmat3<T>().translation2d_from(m3_);
}

template <class T>
nsmat3<T> translation2d_mat3(const nsvec2<T> & m2_)
{
	return nsmat3<T>().translation2d_from(m2_);
}

template <class T>
nsmat3<T> transpose(nsmat3<T> mat_)
{
	return mat_.transpose();
}

template <class T>
nsmat3<T> inverse(nsmat3<T> mat_)
{
	return mat_.invert();
}

template<class PUPer, class T>
void pup(PUPer & p, nsmat3<T> & m3, const nsstring & varName)
{
	pup(p, m3[0], varName + "[0]"); pup(p, m3[1], varName + "[1]"); pup(p, m3[2], varName + "[2]");
}

#endif
