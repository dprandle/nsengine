#ifndef NSVECTOR_H
#define NSVECTOR_H

#include <nsmath.h>
#include <vector>

typedef std::vector<char> c_vector;
typedef std::vector<char16> c16_vector;
typedef std::vector<char32> c32_vector;
typedef std::vector<wchar> wc_vector;
typedef std::vector<int8> i8_vector;
typedef std::vector<int16> i16_vector;
typedef std::vector<int32> i_vector;
typedef std::vector<int64> i64_vector;
typedef std::vector<uint8> ui8_vector;
typedef std::vector<uint16> ui16_vector;
typedef std::vector<uint32> ui_vector;
typedef std::vector<uint64> ui64_vector;
typedef std::vector<float> f_vector;
typedef std::vector<double> d_vector;
typedef std::vector<ldouble> ld_vector;
typedef std::vector<nsstring> nsstring_vector;

typedef std::vector<nsvec2<char>> cvec2_vector;
typedef std::vector<nsvec2<char16>> c16vec2_vector;
typedef std::vector<nsvec2<char32>> c32vec2_vector;
typedef std::vector<nsvec2<wchar>> wcvec2_vector;
typedef std::vector<nsvec2<int8>> i8vec2_vector;
typedef std::vector<nsvec2<int16>> i16vec2_vector;
typedef std::vector<nsvec2<int32>> ivec2_vector;
typedef std::vector<nsvec2<int64>> i64vec2_vector;
typedef std::vector<nsvec2<uint8>> ui8vec2_vector;
typedef std::vector<nsvec2<uint16>> ui16vec2_vector;
typedef std::vector<nsvec2<uint32>> uivec2_vector;
typedef std::vector<nsvec2<uint64>> ui64vec2_vector;
typedef std::vector<nsvec2<float>> fvec2_vector;
typedef std::vector<nsvec2<double>> vec2_vector;
typedef std::vector<nsvec2<ldouble>> ldvec2_vector;

typedef std::vector<nsvec3<char>> cvec3_vector;
typedef std::vector<nsvec3<char16>> c16vec3_vector;
typedef std::vector<nsvec3<char32>> c32vec3_vector;
typedef std::vector<nsvec3<wchar>> wcvec3_vector;
typedef std::vector<nsvec3<int8>> i8vec3_vector;
typedef std::vector<nsvec3<int16>> i16vec3_vector;
typedef std::vector<nsvec3<int32>> ivec3_vector;
typedef std::vector<nsvec3<int64>> i64vec3_vector;
typedef std::vector<nsvec3<uint8>> ui8vec3_vector;
typedef std::vector<nsvec3<uint16>> ui16vec3_vector;
typedef std::vector<nsvec3<uint32>> uivec3_vector;
typedef std::vector<nsvec3<uint64>> ui64vec3_vector;
typedef std::vector<nsvec3<float>> fvec3_vector;
typedef std::vector<nsvec3<double>> vec3_vector;
typedef std::vector<nsvec3<ldouble>> ldvec3_vector;

typedef std::vector<nsvec4<char>> cvec4_vector;
typedef std::vector<nsvec4<char16>> c16vec4_vector;
typedef std::vector<nsvec4<char32>> c32vec4_vector;
typedef std::vector<nsvec4<wchar>> wcvec4_vector;
typedef std::vector<nsvec4<int8>> i8vec4_vector;
typedef std::vector<nsvec4<int16>> i16vec4_vector;
typedef std::vector<nsvec4<int32>> ivec4_vector;
typedef std::vector<nsvec4<int64>> i64vec4_vector;
typedef std::vector<nsvec4<uint8>> ui8vec4_vector;
typedef std::vector<nsvec4<uint16>> ui16vec4_vector;
typedef std::vector<nsvec4<uint32>> uivec4_vector;
typedef std::vector<nsvec4<uint64>> ui64vec4_vector;
typedef std::vector<nsvec4<float>> fvec4_vector;
typedef std::vector<nsvec4<double>> vec4_vector;
typedef std::vector<nsvec4<ldouble>> ldvec4_vector;

typedef std::vector<nsquat<char>> cquat_vector;
typedef std::vector<nsquat<char16>> c16quat_vector;
typedef std::vector<nsquat<char32>> c32quat_vector;
typedef std::vector<nsquat<wchar>> wcquat_vector;
typedef std::vector<nsquat<int8>> i8quat_vector;
typedef std::vector<nsquat<int16>> i16quat_vector;
typedef std::vector<nsquat<int32>> iquat_vector;
typedef std::vector<nsquat<int64>> i64quat_vector;
typedef std::vector<nsquat<uint8>> ui8quat_vector;
typedef std::vector<nsquat<uint16>> ui16quat_vector;
typedef std::vector<nsquat<uint32>> uiquat_vector;
typedef std::vector<nsquat<uint64>> ui64quat_vector;
typedef std::vector<nsquat<float>> fquat_vector;
typedef std::vector<nsquat<double>> quat_vector;
typedef std::vector<nsquat<ldouble>> ldquat_vector;

typedef std::vector<nsmat2<char>> cmat2_vector;
typedef std::vector<nsmat2<char16>> c16mat2_vector;
typedef std::vector<nsmat2<char32>> c32mat2_vector;
typedef std::vector<nsmat2<wchar>> wcmat2_vector;
typedef std::vector<nsmat2<int8>> i8mat2_vector;
typedef std::vector<nsmat2<int16>> i16mat2_vector;
typedef std::vector<nsmat2<int32>> imat2_vector;
typedef std::vector<nsmat2<int64>> i64mat2_vector;
typedef std::vector<nsmat2<uint8>> ui8mat2_vector;
typedef std::vector<nsmat2<uint16>> ui16mat2_vector;
typedef std::vector<nsmat2<uint32>> uimat2_vector;
typedef std::vector<nsmat2<uint64>> ui64mat2_vector;
typedef std::vector<nsmat2<float>> fmat2_vector;
typedef std::vector<nsmat2<double>> mat2_vector;
typedef std::vector<nsmat2<ldouble>> ldmat2_vector;

typedef std::vector<nsmat3<char>> cmat3_vector;
typedef std::vector<nsmat3<char16>> c16mat3_vector;
typedef std::vector<nsmat3<char32>> c32mat3_vector;
typedef std::vector<nsmat3<wchar>> wcmat3_vector;
typedef std::vector<nsmat3<int8>> i8mat3_vector;
typedef std::vector<nsmat3<int16>> i16mat3_vector;
typedef std::vector<nsmat3<int32>> imat3_vector;
typedef std::vector<nsmat3<int64>> i64mat3_vector;
typedef std::vector<nsmat3<uint8>> ui8mat3_vector;
typedef std::vector<nsmat3<uint16>> ui16mat3_vector;
typedef std::vector<nsmat3<uint32>> uimat3_vector;
typedef std::vector<nsmat3<uint64>> ui64mat3_vector;
typedef std::vector<nsmat3<float>> fmat3_vector;
typedef std::vector<nsmat3<double>> mat3_vector;
typedef std::vector<nsmat3<ldouble>> ldmat3_vector;

typedef std::vector<nsmat4<char>> cmat4_vector;
typedef std::vector<nsmat4<char16>> c16mat4_vector;
typedef std::vector<nsmat4<char32>> c32mat4_vector;
typedef std::vector<nsmat4<wchar>> wcmat4_vector;
typedef std::vector<nsmat4<int8>> i8mat4_vector;
typedef std::vector<nsmat4<int16>> i16mat4_vector;
typedef std::vector<nsmat4<int32>> imat4_vector;
typedef std::vector<nsmat4<int64>> i64mat4_vector;
typedef std::vector<nsmat4<uint8>> ui8mat4_vector;
typedef std::vector<nsmat4<uint16>> ui16mat4_vector;
typedef std::vector<nsmat4<uint32>> uimat4_vector;
typedef std::vector<nsmat4<uint64>> ui64mat4_vector;
typedef std::vector<nsmat4<float>> fmat4_vector;
typedef std::vector<nsmat4<double>> mat4_vector;
typedef std::vector<nsmat4<ldouble>> ldmat4_vector;

#endif
