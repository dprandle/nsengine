#ifndef NSMATH_H
#define NSMATH_H

#define PI 3.14159265359f
#define EPS 0.0001f

#include <cmath>
#include <nsglobal.h>

float clampf(float pVal, const float & pMin, const float & pMax);
double clamp(double pVal, const double & pMin, const double & pMax);
float fractf(const float & num);
double fract(const double & num);

float lerp(int32 low, int32 high, int32 middle);
float lerp(uint32 low, uint32 high, uint32 middle);
float lerp(float low, float high, float middle);
float lerp(float low, float high, float middle);
double lerp(double low, double high, double middle);

template<class T>
T degrees(const T & val);

template<class T>
T radians(const T & val);

float RandomFloat(float pHigh = 1.0f, float pLow = 0.0f);

#include "nsmat4.h"

typedef std::vector<NSVec2<char>> cvec2array;
typedef std::vector<NSVec2<char16>> c16vec2array;
typedef std::vector<NSVec2<char32>> c32vec2array;
typedef std::vector<NSVec2<wchar>> cwvec2array;
typedef std::vector<NSVec2<int16>> sivec2array;
typedef std::vector<NSVec2<int32>> ivec2array;
typedef std::vector<NSVec2<int32>> livec2array;
typedef std::vector<NSVec2<int64>> llivec2array;
typedef std::vector<NSVec2<uint8>> ucvec2array;
typedef std::vector<NSVec2<uint16>> usivec2array;
typedef std::vector<NSVec2<uint32>> uivec2array;
typedef std::vector<NSVec2<uint32>> ulivec2array;
typedef std::vector<NSVec2<uint64>> ullivec2array;
typedef std::vector<NSVec2<float>> fvec2array;
typedef std::vector<NSVec2<double>> vec2array;
typedef std::vector<NSVec2<ldouble>> lvec2array;

typedef std::vector<NSVec3<char>> cvec3array;
typedef std::vector<NSVec3<char16>> c16vec3array;
typedef std::vector<NSVec3<char32>> c32vec3array;
typedef std::vector<NSVec3<wchar>> cwvec3array;
typedef std::vector<NSVec3<int16>> sivec3array;
typedef std::vector<NSVec3<int32>> ivec3array;
typedef std::vector<NSVec3<int32>> livec3array;
typedef std::vector<NSVec3<int64>> llivec3array;
typedef std::vector<NSVec3<uint8>> ucvec3array;
typedef std::vector<NSVec3<uint16>> usivec3array;
typedef std::vector<NSVec3<uint32>> uivec3array;
typedef std::vector<NSVec3<uint32>> ulivec3array;
typedef std::vector<NSVec3<uint64>> ullivec3array;
typedef std::vector<NSVec3<float>> fvec3array;
typedef std::vector<NSVec3<double>> vec3array;
typedef std::vector<NSVec3<ldouble>> lvec3array;

typedef std::vector<NSVec4<char>> cvec4array;
typedef std::vector<NSVec4<char16>> c16vec4array;
typedef std::vector<NSVec4<char32>> c32vec4array;
typedef std::vector<NSVec4<wchar>> cwvec4array;
typedef std::vector<NSVec4<int16>> sivec4array;
typedef std::vector<NSVec4<int32>> ivec4array;
typedef std::vector<NSVec4<int32>> livec4array;
typedef std::vector<NSVec4<int64>> llivec4array;
typedef std::vector<NSVec4<uint8>> ucvec4array;
typedef std::vector<NSVec4<uint16>> usivec4array;
typedef std::vector<NSVec4<uint32>> uivec4array;
typedef std::vector<NSVec4<uint32>> ulivec4array;
typedef std::vector<NSVec4<uint64>> ullivec4array;
typedef std::vector<NSVec4<float>> fvec4array;
typedef std::vector<NSVec4<double>> vec4array;
typedef std::vector<NSVec4<ldouble>> lvec4array;

typedef std::vector<NSQuat<char>> cquatarray;
typedef std::vector<NSQuat<char16>> c16quatarray;
typedef std::vector<NSQuat<char32>> c32quatarray;
typedef std::vector<NSQuat<wchar>> cwquatarray;
typedef std::vector<NSQuat<int16>> siquatarray;
typedef std::vector<NSQuat<int32>> iquatarray;
typedef std::vector<NSQuat<int32>> liquatarray;
typedef std::vector<NSQuat<int64>> lliquatarray;
typedef std::vector<NSQuat<uint8>> ucquatarray;
typedef std::vector<NSQuat<uint16>> usiquatarray;
typedef std::vector<NSQuat<uint32>> uiquatarray;
typedef std::vector<NSQuat<uint32>> uliquatarray;
typedef std::vector<NSQuat<uint64>> ulliquatarray;
typedef std::vector<NSQuat<float>> fquatarray;
typedef std::vector<NSQuat<double>> quatarray;
typedef std::vector<NSQuat<ldouble>> lquatarray;

typedef std::vector<NSMat2<char>> cmat2array;
typedef std::vector<NSMat2<char16>> c16mat2array;
typedef std::vector<NSMat2<char32>> c32mat2array;
typedef std::vector<NSMat2<wchar>> cwmat2array;
typedef std::vector<NSMat2<int16>> simat2array;
typedef std::vector<NSMat2<int32>> imat2array;
typedef std::vector<NSMat2<int32>> limat2array;
typedef std::vector<NSMat2<int64>> llimat2array;
typedef std::vector<NSMat2<uint8>> ucmat2array;
typedef std::vector<NSMat2<uint16>> usimat2array;
typedef std::vector<NSMat2<uint32>> uimat2array;
typedef std::vector<NSMat2<uint32>> ulimat2array;
typedef std::vector<NSMat2<uint64>> ullimat2array;
typedef std::vector<NSMat2<float>> fmat2array;
typedef std::vector<NSMat2<double>> mat2array;
typedef std::vector<NSMat2<ldouble>> lmat2array;

typedef std::vector<NSMat3<char>> cmat3array;
typedef std::vector<NSMat3<char16>> c16mat3array;
typedef std::vector<NSMat3<char32>> c32mat3array;
typedef std::vector<NSMat3<wchar>> cwmat3array;
typedef std::vector<NSMat3<int16>> simat3array;
typedef std::vector<NSMat3<int32>> imat3array;
typedef std::vector<NSMat3<int32>> limat3array;
typedef std::vector<NSMat3<int64>> llimat3array;
typedef std::vector<NSMat3<uint8>> ucmat3array;
typedef std::vector<NSMat3<uint16>> usimat3array;
typedef std::vector<NSMat3<uint32>> uimat3array;
typedef std::vector<NSMat3<uint32>> ulimat3array;
typedef std::vector<NSMat3<uint64>> ullimat3array;
typedef std::vector<NSMat3<float>> fmat3array;
typedef std::vector<NSMat3<double>> mat3array;
typedef std::vector<NSMat3<ldouble>> lmat3array;

typedef std::vector<NSMat4<char>> cmat4array;
typedef std::vector<NSMat4<char16>> c16mat4array;
typedef std::vector<NSMat4<char32>> c32mat4array;
typedef std::vector<NSMat4<wchar>> cwmat4array;
typedef std::vector<NSMat4<int16>> simat4array;
typedef std::vector<NSMat4<int32>> imat4array;
typedef std::vector<NSMat4<int32>> limat4array;
typedef std::vector<NSMat4<int64>> llimat4array;
typedef std::vector<NSMat4<uint8>> ucmat4array;
typedef std::vector<NSMat4<uint16>> usimat4array;
typedef std::vector<NSMat4<uint32>> uimat4array;
typedef std::vector<NSMat4<uint32>> ulimat4array;
typedef std::vector<NSMat4<uint64>> ullimat4array;
typedef std::vector<NSMat4<float>> fmat4array;
typedef std::vector<NSMat4<double>> mat4array;
typedef std::vector<NSMat4<ldouble>> lmat4array;

typedef std::map<uint32,NSVec2<char>> cvec2uimap;
typedef std::map<uint32,NSVec2<char16>> c16vec2uimap;
typedef std::map<uint32,NSVec2<char32>> c32vec2uimap;
typedef std::map<uint32,NSVec2<wchar>> cwvec2uimap;
typedef std::map<uint32,NSVec2<int16>> sivec2uimap;
typedef std::map<uint32,NSVec2<int32>> ivec2uimap;
typedef std::map<uint32,NSVec2<int32>> livec2uimap;
typedef std::map<uint32,NSVec2<int64>> llivec2uimap;
typedef std::map<uint32,NSVec2<uint8>> ucvec2uimap;
typedef std::map<uint32,NSVec2<uint16>> usivec2uimap;
typedef std::map<uint32,NSVec2<uint32>> uivec2uimap;
typedef std::map<uint32,NSVec2<uint32>> ulivec2uimap;
typedef std::map<uint32,NSVec2<uint64>> ullivec2uimap;
typedef std::map<uint32,NSVec2<float>> fvec2uimap;
typedef std::map<uint32,NSVec2<double>> vec2uimap;
typedef std::map<uint32,NSVec2<ldouble>> lvec2uimap;

typedef std::map<uint32,NSVec3<char>> cvec3uimap;
typedef std::map<uint32,NSVec3<char16>> c16vec3uimap;
typedef std::map<uint32,NSVec3<char32>> c32vec3uimap;
typedef std::map<uint32,NSVec3<wchar>> cwvec3uimap;
typedef std::map<uint32,NSVec3<int16>> sivec3uimap;
typedef std::map<uint32,NSVec3<int32>> ivec3uimap;
typedef std::map<uint32,NSVec3<int32>> livec3uimap;
typedef std::map<uint32,NSVec3<int64>> llivec3uimap;
typedef std::map<uint32,NSVec3<uint8>> ucvec3uimap;
typedef std::map<uint32,NSVec3<uint16>> usivec3uimap;
typedef std::map<uint32,NSVec3<uint32>> uivec3uimap;
typedef std::map<uint32,NSVec3<uint32>> ulivec3uimap;
typedef std::map<uint32,NSVec3<uint64>> ullivec3uimap;
typedef std::map<uint32,NSVec3<float>> fvec3uimap;
typedef std::map<uint32,NSVec3<double>> vec3uimap;
typedef std::map<uint32,NSVec3<ldouble>> lvec3uimap;

typedef std::map<uint32,NSVec4<char>> cvec4uimap;
typedef std::map<uint32,NSVec4<char16>> c16vec4uimap;
typedef std::map<uint32,NSVec4<char32>> c32vec4uimap;
typedef std::map<uint32,NSVec4<wchar>> cwvec4uimap;
typedef std::map<uint32,NSVec4<int16>> sivec4uimap;
typedef std::map<uint32,NSVec4<int32>> ivec4uimap;
typedef std::map<uint32,NSVec4<int32>> livec4uimap;
typedef std::map<uint32,NSVec4<int64>> llivec4uimap;
typedef std::map<uint32,NSVec4<uint8>> ucvec4uimap;
typedef std::map<uint32,NSVec4<uint16>> usivec4uimap;
typedef std::map<uint32,NSVec4<uint32>> uivec4uimap;
typedef std::map<uint32,NSVec4<uint32>> ulivec4uimap;
typedef std::map<uint32,NSVec4<uint64>> ullivec4uimap;
typedef std::map<uint32,NSVec4<float>> fvec4uimap;
typedef std::map<uint32,NSVec4<double>> vec4uimap;
typedef std::map<uint32,NSVec4<ldouble>> lvec4uimap;

typedef std::map<uint32,NSQuat<char>> cquatuimap;
typedef std::map<uint32,NSQuat<char16>> c16quatuimap;
typedef std::map<uint32,NSQuat<char32>> c32quatuimap;
typedef std::map<uint32,NSQuat<wchar>> cwquatuimap;
typedef std::map<uint32,NSQuat<int16>> siquatuimap;
typedef std::map<uint32,NSQuat<int32>> iquatuimap;
typedef std::map<uint32,NSQuat<int32>> liquatuimap;
typedef std::map<uint32,NSQuat<int64>> lliquatuimap;
typedef std::map<uint32,NSQuat<uint8>> ucquatuimap;
typedef std::map<uint32,NSQuat<uint16>> usiquatuimap;
typedef std::map<uint32,NSQuat<uint32>> uiquatuimap;
typedef std::map<uint32,NSQuat<uint32>> uliquatuimap;
typedef std::map<uint32,NSQuat<uint64>> ulliquatuimap;
typedef std::map<uint32,NSQuat<float>> fquatuimap;
typedef std::map<uint32,NSQuat<double>> quatuimap;
typedef std::map<uint32,NSQuat<ldouble>> lquatuimap;

typedef std::map<uint32,NSMat2<char>> cmat2uimap;
typedef std::map<uint32,NSMat2<char16>> c16mat2uimap;
typedef std::map<uint32,NSMat2<char32>> c32mat2uimap;
typedef std::map<uint32,NSMat2<wchar>> cwmat2uimap;
typedef std::map<uint32,NSMat2<int16>> simat2uimap;
typedef std::map<uint32,NSMat2<int32>> imat2uimap;
typedef std::map<uint32,NSMat2<int32>> limat2uimap;
typedef std::map<uint32,NSMat2<int64>> llimat2uimap;
typedef std::map<uint32,NSMat2<uint8>> ucmat2uimap;
typedef std::map<uint32,NSMat2<uint16>> usimat2uimap;
typedef std::map<uint32,NSMat2<uint32>> uimat2uimap;
typedef std::map<uint32,NSMat2<uint32>> ulimat2uimap;
typedef std::map<uint32,NSMat2<uint64>> ullimat2uimap;
typedef std::map<uint32,NSMat2<float>> fmat2uimap;
typedef std::map<uint32,NSMat2<double>> mat2uimap;
typedef std::map<uint32,NSMat2<ldouble>> lmat2uimap;

typedef std::map<uint32,NSMat3<char>> cmat3uimap;
typedef std::map<uint32,NSMat3<char16>> c16mat3uimap;
typedef std::map<uint32,NSMat3<char32>> c32mat3uimap;
typedef std::map<uint32,NSMat3<wchar>> cwmat3uimap;
typedef std::map<uint32,NSMat3<int16>> simat3uimap;
typedef std::map<uint32,NSMat3<int32>> imat3uimap;
typedef std::map<uint32,NSMat3<int32>> limat3uimap;
typedef std::map<uint32,NSMat3<int64>> llimat3uimap;
typedef std::map<uint32,NSMat3<uint8>> ucmat3uimap;
typedef std::map<uint32,NSMat3<uint16>> usimat3uimap;
typedef std::map<uint32,NSMat3<uint32>> uimat3uimap;
typedef std::map<uint32,NSMat3<uint32>> ulimat3uimap;
typedef std::map<uint32,NSMat3<uint64>> ullimat3uimap;
typedef std::map<uint32,NSMat3<float>> fmat3uimap;
typedef std::map<uint32,NSMat3<double>> mat3uimap;
typedef std::map<uint32,NSMat3<ldouble>> lmat3uimap;

typedef std::map<uint32,NSMat4<char>> cmat4uimap;
typedef std::map<uint32,NSMat4<char16>> c16mat4uimap;
typedef std::map<uint32,NSMat4<char32>> c32mat4uimap;
typedef std::map<uint32,NSMat4<wchar>> cwmat4uimap;
typedef std::map<uint32,NSMat4<int16>> simat4uimap;
typedef std::map<uint32,NSMat4<int32>> imat4uimap;
typedef std::map<uint32,NSMat4<int32>> limat4uimap;
typedef std::map<uint32,NSMat4<int64>> llimat4uimap;
typedef std::map<uint32,NSMat4<uint8>> ucmat4uimap;
typedef std::map<uint32,NSMat4<uint16>> usimat4uimap;
typedef std::map<uint32,NSMat4<uint32>> uimat4uimap;
typedef std::map<uint32,NSMat4<uint32>> ulimat4uimap;
typedef std::map<uint32,NSMat4<uint64>> ullimat4uimap;
typedef std::map<uint32,NSMat4<float>> fmat4uimap;
typedef std::map<uint32,NSMat4<double>> mat4uimap;
typedef std::map<uint32,NSMat4<ldouble>> lmat4uimap;

typedef std::map<int32, NSVec2<char>> cvec2imap;
typedef std::map<int32, NSVec2<char16>> c16vec2imap;
typedef std::map<int32, NSVec2<char32>> c32vec2imap;
typedef std::map<int32, NSVec2<wchar>> cwvec2imap;
typedef std::map<int32, NSVec2<int16>> sivec2imap;
typedef std::map<int32, NSVec2<int32>> ivec2imap;
typedef std::map<int32, NSVec2<int32>> livec2imap;
typedef std::map<int32, NSVec2<int64>> llivec2imap;
typedef std::map<int32, NSVec2<uint8>> ucvec2imap;
typedef std::map<int32, NSVec2<uint16>> usivec2imap;
typedef std::map<int32, NSVec2<uint32>> uivec2imap;
typedef std::map<int32, NSVec2<uint32>> ulivec2imap;
typedef std::map<int32, NSVec2<uint64>> ullivec2imap;
typedef std::map<int32, NSVec2<float>> fvec2imap;
typedef std::map<int32, NSVec2<double>> vec2imap;
typedef std::map<int32, NSVec2<ldouble>> lvec2imap;

typedef std::map<int32, NSVec3<char>> cvec3imap;
typedef std::map<int32, NSVec3<char16>> c16vec3imap;
typedef std::map<int32, NSVec3<char32>> c32vec3imap;
typedef std::map<int32, NSVec3<wchar>> cwvec3imap;
typedef std::map<int32, NSVec3<int16>> sivec3imap;
typedef std::map<int32, NSVec3<int32>> ivec3imap;
typedef std::map<int32, NSVec3<int32>> livec3imap;
typedef std::map<int32, NSVec3<int64>> llivec3imap;
typedef std::map<int32, NSVec3<uint8>> ucvec3imap;
typedef std::map<int32, NSVec3<uint16>> usivec3imap;
typedef std::map<int32, NSVec3<uint32>> uivec3imap;
typedef std::map<int32, NSVec3<uint32>> ulivec3imap;
typedef std::map<int32, NSVec3<uint64>> ullivec3imap;
typedef std::map<int32, NSVec3<float>> fvec3imap;
typedef std::map<int32, NSVec3<double>> vec3imap;
typedef std::map<int32, NSVec3<ldouble>> lvec3imap;

typedef std::map<int32, NSVec4<char>> cvec4imap;
typedef std::map<int32, NSVec4<char16>> c16vec4imap;
typedef std::map<int32, NSVec4<char32>> c32vec4imap;
typedef std::map<int32, NSVec4<wchar>> cwvec4imap;
typedef std::map<int32, NSVec4<int16>> sivec4imap;
typedef std::map<int32, NSVec4<int32>> ivec4imap;
typedef std::map<int32, NSVec4<int32>> livec4imap;
typedef std::map<int32, NSVec4<int64>> llivec4imap;
typedef std::map<int32, NSVec4<uint8>> ucvec4imap;
typedef std::map<int32, NSVec4<uint16>> usivec4imap;
typedef std::map<int32, NSVec4<uint32>> uivec4imap;
typedef std::map<int32, NSVec4<uint32>> ulivec4imap;
typedef std::map<int32, NSVec4<uint64>> ullivec4imap;
typedef std::map<int32, NSVec4<float>> fvec4imap;
typedef std::map<int32, NSVec4<double>> vec4imap;
typedef std::map<int32, NSVec4<ldouble>> lvec4imap;

typedef std::map<int32, NSQuat<char>> cquatimap;
typedef std::map<int32, NSQuat<char16>> c16quatimap;
typedef std::map<int32, NSQuat<char32>> c32quatimap;
typedef std::map<int32, NSQuat<wchar>> cwquatimap;
typedef std::map<int32, NSQuat<int16>> siquatimap;
typedef std::map<int32, NSQuat<int32>> iquatimap;
typedef std::map<int32, NSQuat<int32>> liquatimap;
typedef std::map<int32, NSQuat<int64>> lliquatimap;
typedef std::map<int32, NSQuat<uint8>> ucquatimap;
typedef std::map<int32, NSQuat<uint16>> usiquatimap;
typedef std::map<int32, NSQuat<uint32>> uiquatimap;
typedef std::map<int32, NSQuat<uint32>> uliquatimap;
typedef std::map<int32, NSQuat<uint64>> ulliquatimap;
typedef std::map<int32, NSQuat<float>> fquatimap;
typedef std::map<int32, NSQuat<double>> quatimap;
typedef std::map<int32, NSQuat<ldouble>> lquatimap;

typedef std::map<int32, NSMat2<char>> cmat2imap;
typedef std::map<int32, NSMat2<char16>> c16mat2imap;
typedef std::map<int32, NSMat2<char32>> c32mat2imap;
typedef std::map<int32, NSMat2<wchar>> cwmat2imap;
typedef std::map<int32, NSMat2<int16>> simat2imap;
typedef std::map<int32, NSMat2<int32>> imat2imap;
typedef std::map<int32, NSMat2<int32>> limat2imap;
typedef std::map<int32, NSMat2<int64>> llimat2imap;
typedef std::map<int32, NSMat2<uint8>> ucmat2imap;
typedef std::map<int32, NSMat2<uint16>> usimat2imap;
typedef std::map<int32, NSMat2<uint32>> uimat2imap;
typedef std::map<int32, NSMat2<uint32>> ulimat2imap;
typedef std::map<int32, NSMat2<uint64>> ullimat2imap;
typedef std::map<int32, NSMat2<float>> fmat2imap;
typedef std::map<int32, NSMat2<double>> mat2imap;
typedef std::map<int32, NSMat2<ldouble>> lmat2imap;

typedef std::map<int32, NSMat3<char>> cmat3imap;
typedef std::map<int32, NSMat3<char16>> c16mat3imap;
typedef std::map<int32, NSMat3<char32>> c32mat3imap;
typedef std::map<int32, NSMat3<wchar>> cwmat3imap;
typedef std::map<int32, NSMat3<int16>> simat3imap;
typedef std::map<int32, NSMat3<int32>> imat3imap;
typedef std::map<int32, NSMat3<int32>> limat3imap;
typedef std::map<int32, NSMat3<int64>> llimat3imap;
typedef std::map<int32, NSMat3<uint8>> ucmat3imap;
typedef std::map<int32, NSMat3<uint16>> usimat3imap;
typedef std::map<int32, NSMat3<uint32>> uimat3imap;
typedef std::map<int32, NSMat3<uint32>> ulimat3imap;
typedef std::map<int32, NSMat3<uint64>> ullimat3imap;
typedef std::map<int32, NSMat3<float>> fmat3imap;
typedef std::map<int32, NSMat3<double>> mat3imap;
typedef std::map<int32, NSMat3<ldouble>> lmat3imap;

typedef std::map<int32, NSMat4<char>> cmat4imap;
typedef std::map<int32, NSMat4<char16>> c16mat4imap;
typedef std::map<int32, NSMat4<char32>> c32mat4imap;
typedef std::map<int32, NSMat4<wchar>> cwmat4imap;
typedef std::map<int32, NSMat4<int16>> simat4imap;
typedef std::map<int32, NSMat4<int32>> imat4imap;
typedef std::map<int32, NSMat4<int32>> limat4imap;
typedef std::map<int32, NSMat4<int64>> llimat4imap;
typedef std::map<int32, NSMat4<uint8>> ucmat4imap;
typedef std::map<int32, NSMat4<uint16>> usimat4imap;
typedef std::map<int32, NSMat4<uint32>> uimat4imap;
typedef std::map<int32, NSMat4<uint32>> ulimat4imap;
typedef std::map<int32, NSMat4<uint64>> ullimat4imap;
typedef std::map<int32, NSMat4<float>> fmat4imap;
typedef std::map<int32, NSMat4<double>> mat4imap;
typedef std::map<int32, NSMat4<ldouble>> lmat4imap;

typedef std::map<float, NSVec2<char>> cvec2fmap;
typedef std::map<float, NSVec2<char16>> c16vec2fmap;
typedef std::map<float, NSVec2<char32>> c32vec2fmap;
typedef std::map<float, NSVec2<wchar>> cwvec2fmap;
typedef std::map<float, NSVec2<int16>> sivec2fmap;
typedef std::map<float, NSVec2<int32>> ivec2fmap;
typedef std::map<float, NSVec2<int32>> livec2fmap;
typedef std::map<float, NSVec2<int64>> llivec2fmap;
typedef std::map<float, NSVec2<uint8>> ucvec2fmap;
typedef std::map<float, NSVec2<uint16>> usivec2fmap;
typedef std::map<float, NSVec2<uint32>> uivec2fmap;
typedef std::map<float, NSVec2<uint32>> ulivec2fmap;
typedef std::map<float, NSVec2<uint64>> ullivec2fmap;
typedef std::map<float, NSVec2<float>> fvec2fmap;
typedef std::map<float, NSVec2<double>> vec2fmap;
typedef std::map<float, NSVec2<ldouble>> lvec2fmap;

typedef std::map<float, NSVec3<char>> cvec3fmap;
typedef std::map<float, NSVec3<char16>> c16vec3fmap;
typedef std::map<float, NSVec3<char32>> c32vec3fmap;
typedef std::map<float, NSVec3<wchar>> cwvec3fmap;
typedef std::map<float, NSVec3<int16>> sivec3fmap;
typedef std::map<float, NSVec3<int32>> ivec3fmap;
typedef std::map<float, NSVec3<int32>> livec3fmap;
typedef std::map<float, NSVec3<int64>> llivec3fmap;
typedef std::map<float, NSVec3<uint8>> ucvec3fmap;
typedef std::map<float, NSVec3<uint16>> usivec3fmap;
typedef std::map<float, NSVec3<uint32>> uivec3fmap;
typedef std::map<float, NSVec3<uint32>> ulivec3fmap;
typedef std::map<float, NSVec3<uint64>> ullivec3fmap;
typedef std::map<float, NSVec3<float>> fvec3fmap;
typedef std::map<float, NSVec3<double>> vec3fmap;
typedef std::map<float, NSVec3<ldouble>> lvec3fmap;

typedef std::map<float, NSVec4<char>> cvec4fmap;
typedef std::map<float, NSVec4<char16>> c16vec4fmap;
typedef std::map<float, NSVec4<char32>> c32vec4fmap;
typedef std::map<float, NSVec4<wchar>> cwvec4fmap;
typedef std::map<float, NSVec4<int16>> sivec4fmap;
typedef std::map<float, NSVec4<int32>> ivec4fmap;
typedef std::map<float, NSVec4<int32>> livec4fmap;
typedef std::map<float, NSVec4<int64>> llivec4fmap;
typedef std::map<float, NSVec4<uint8>> ucvec4fmap;
typedef std::map<float, NSVec4<uint16>> usivec4fmap;
typedef std::map<float, NSVec4<uint32>> uivec4fmap;
typedef std::map<float, NSVec4<uint32>> ulivec4fmap;
typedef std::map<float, NSVec4<uint64>> ullivec4fmap;
typedef std::map<float, NSVec4<float>> fvec4fmap;
typedef std::map<float, NSVec4<double>> vec4fmap;
typedef std::map<float, NSVec4<ldouble>> lvec4fmap;

typedef std::map<float, NSQuat<char>> cquatfmap;
typedef std::map<float, NSQuat<char16>> c16quatfmap;
typedef std::map<float, NSQuat<char32>> c32quatfmap;
typedef std::map<float, NSQuat<wchar>> cwquatfmap;
typedef std::map<float, NSQuat<int16>> siquatfmap;
typedef std::map<float, NSQuat<int32>> iquatfmap;
typedef std::map<float, NSQuat<int32>> liquatfmap;
typedef std::map<float, NSQuat<int64>> lliquatfmap;
typedef std::map<float, NSQuat<uint8>> ucquatfmap;
typedef std::map<float, NSQuat<uint16>> usiquatfmap;
typedef std::map<float, NSQuat<uint32>> uiquatfmap;
typedef std::map<float, NSQuat<uint32>> uliquatfmap;
typedef std::map<float, NSQuat<uint64>> ulliquatfmap;
typedef std::map<float, NSQuat<float>> fquatfmap;
typedef std::map<float, NSQuat<double>> quatfmap;
typedef std::map<float, NSQuat<ldouble>> lquatfmap;

typedef std::map<float, NSMat2<char>> cmat2fmap;
typedef std::map<float, NSMat2<char16>> c16mat2fmap;
typedef std::map<float, NSMat2<char32>> c32mat2fmap;
typedef std::map<float, NSMat2<wchar>> cwmat2fmap;
typedef std::map<float, NSMat2<int16>> simat2fmap;
typedef std::map<float, NSMat2<int32>> imat2fmap;
typedef std::map<float, NSMat2<int32>> limat2fmap;
typedef std::map<float, NSMat2<int64>> llimat2fmap;
typedef std::map<float, NSMat2<uint8>> ucmat2fmap;
typedef std::map<float, NSMat2<uint16>> usimat2fmap;
typedef std::map<float, NSMat2<uint32>> uimat2fmap;
typedef std::map<float, NSMat2<uint32>> ulimat2fmap;
typedef std::map<float, NSMat2<uint64>> ullimat2fmap;
typedef std::map<float, NSMat2<float>> fmat2fmap;
typedef std::map<float, NSMat2<double>> mat2fmap;
typedef std::map<float, NSMat2<ldouble>> lmat2fmap;

typedef std::map<float, NSMat3<char>> cmat3fmap;
typedef std::map<float, NSMat3<char16>> c16mat3fmap;
typedef std::map<float, NSMat3<char32>> c32mat3fmap;
typedef std::map<float, NSMat3<wchar>> cwmat3fmap;
typedef std::map<float, NSMat3<int16>> simat3fmap;
typedef std::map<float, NSMat3<int32>> imat3fmap;
typedef std::map<float, NSMat3<int32>> limat3fmap;
typedef std::map<float, NSMat3<int64>> llimat3fmap;
typedef std::map<float, NSMat3<uint8>> ucmat3fmap;
typedef std::map<float, NSMat3<uint16>> usimat3fmap;
typedef std::map<float, NSMat3<uint32>> uimat3fmap;
typedef std::map<float, NSMat3<uint32>> ulimat3fmap;
typedef std::map<float, NSMat3<uint64>> ullimat3fmap;
typedef std::map<float, NSMat3<float>> fmat3fmap;
typedef std::map<float, NSMat3<double>> mat3fmap;
typedef std::map<float, NSMat3<ldouble>> lmat3fmap;

typedef std::map<float, NSMat4<char>> cmat4fmap;
typedef std::map<float, NSMat4<char16>> c16mat4fmap;
typedef std::map<float, NSMat4<char32>> c32mat4fmap;
typedef std::map<float, NSMat4<wchar>> cwmat4fmap;
typedef std::map<float, NSMat4<int16>> simat4fmap;
typedef std::map<float, NSMat4<int32>> imat4fmap;
typedef std::map<float, NSMat4<int32>> limat4fmap;
typedef std::map<float, NSMat4<int64>> llimat4fmap;
typedef std::map<float, NSMat4<uint8>> ucmat4fmap;
typedef std::map<float, NSMat4<uint16>> usimat4fmap;
typedef std::map<float, NSMat4<uint32>> uimat4fmap;
typedef std::map<float, NSMat4<uint32>> ulimat4fmap;
typedef std::map<float, NSMat4<uint64>> ullimat4fmap;
typedef std::map<float, NSMat4<float>> fmat4fmap;
typedef std::map<float, NSMat4<double>> mat4fmap;
typedef std::map<float, NSMat4<ldouble>> lmat4fmap;

typedef std::map<double, NSVec2<char>> cvec2dmap;
typedef std::map<double, NSVec2<char16>> c16vec2dmap;
typedef std::map<double, NSVec2<char32>> c32vec2dmap;
typedef std::map<double, NSVec2<wchar>> cwvec2dmap;
typedef std::map<double, NSVec2<int16>> sivec2dmap;
typedef std::map<double, NSVec2<int32>> ivec2dmap;
typedef std::map<double, NSVec2<int32>> livec2dmap;
typedef std::map<double, NSVec2<int64>> llivec2dmap;
typedef std::map<double, NSVec2<uint8>> ucvec2dmap;
typedef std::map<double, NSVec2<uint16>> usivec2dmap;
typedef std::map<double, NSVec2<uint32>> uivec2dmap;
typedef std::map<double, NSVec2<uint32>> ulivec2dmap;
typedef std::map<double, NSVec2<uint64>> ullivec2dmap;
typedef std::map<double, NSVec2<float>> fvec2dmap;
typedef std::map<double, NSVec2<double>> vec2dmap;
typedef std::map<double, NSVec2<ldouble>> lvec2dmap;

typedef std::map<double, NSVec3<char>> cvec3dmap;
typedef std::map<double, NSVec3<char16>> c16vec3dmap;
typedef std::map<double, NSVec3<char32>> c32vec3dmap;
typedef std::map<double, NSVec3<wchar>> cwvec3dmap;
typedef std::map<double, NSVec3<int16>> sivec3dmap;
typedef std::map<double, NSVec3<int32>> ivec3dmap;
typedef std::map<double, NSVec3<int32>> livec3dmap;
typedef std::map<double, NSVec3<int64>> llivec3dmap;
typedef std::map<double, NSVec3<uint8>> ucvec3dmap;
typedef std::map<double, NSVec3<uint16>> usivec3dmap;
typedef std::map<double, NSVec3<uint32>> uivec3dmap;
typedef std::map<double, NSVec3<uint32>> ulivec3dmap;
typedef std::map<double, NSVec3<uint64>> ullivec3dmap;
typedef std::map<double, NSVec3<float>> fvec3dmap;
typedef std::map<double, NSVec3<double>> vec3dmap;
typedef std::map<double, NSVec3<ldouble>> lvec3dmap;

typedef std::map<double, NSVec4<char>> cvec4dmap;
typedef std::map<double, NSVec4<char16>> c16vec4dmap;
typedef std::map<double, NSVec4<char32>> c32vec4dmap;
typedef std::map<double, NSVec4<wchar>> cwvec4dmap;
typedef std::map<double, NSVec4<int16>> sivec4dmap;
typedef std::map<double, NSVec4<int32>> ivec4dmap;
typedef std::map<double, NSVec4<int32>> livec4dmap;
typedef std::map<double, NSVec4<int64>> llivec4dmap;
typedef std::map<double, NSVec4<uint8>> ucvec4dmap;
typedef std::map<double, NSVec4<uint16>> usivec4dmap;
typedef std::map<double, NSVec4<uint32>> uivec4dmap;
typedef std::map<double, NSVec4<uint32>> ulivec4dmap;
typedef std::map<double, NSVec4<uint64>> ullivec4dmap;
typedef std::map<double, NSVec4<float>> fvec4dmap;
typedef std::map<double, NSVec4<double>> vec4dmap;
typedef std::map<double, NSVec4<ldouble>> lvec4dmap;

typedef std::map<double, NSQuat<char>> cquatdmap;
typedef std::map<double, NSQuat<char16>> c16quatdmap;
typedef std::map<double, NSQuat<char32>> c32quatdmap;
typedef std::map<double, NSQuat<wchar>> cwquatdmap;
typedef std::map<double, NSQuat<int16>> siquatdmap;
typedef std::map<double, NSQuat<int32>> iquatdmap;
typedef std::map<double, NSQuat<int32>> liquatdmap;
typedef std::map<double, NSQuat<int64>> lliquatdmap;
typedef std::map<double, NSQuat<uint8>> ucquatdmap;
typedef std::map<double, NSQuat<uint16>> usiquatdmap;
typedef std::map<double, NSQuat<uint32>> uiquatdmap;
typedef std::map<double, NSQuat<uint32>> uliquatdmap;
typedef std::map<double, NSQuat<uint64>> ulliquatdmap;
typedef std::map<double, NSQuat<float>> fquatdmap;
typedef std::map<double, NSQuat<double>> quatdmap;
typedef std::map<double, NSQuat<ldouble>> lquatdmap;

typedef std::map<double, NSMat2<char>> cmat2dmap;
typedef std::map<double, NSMat2<char16>> c16mat2dmap;
typedef std::map<double, NSMat2<char32>> c32mat2dmap;
typedef std::map<double, NSMat2<wchar>> cwmat2dmap;
typedef std::map<double, NSMat2<int16>> simat2dmap;
typedef std::map<double, NSMat2<int32>> imat2dmap;
typedef std::map<double, NSMat2<int32>> limat2dmap;
typedef std::map<double, NSMat2<int64>> llimat2dmap;
typedef std::map<double, NSMat2<uint8>> ucmat2dmap;
typedef std::map<double, NSMat2<uint16>> usimat2dmap;
typedef std::map<double, NSMat2<uint32>> uimat2dmap;
typedef std::map<double, NSMat2<uint32>> ulimat2dmap;
typedef std::map<double, NSMat2<uint64>> ullimat2dmap;
typedef std::map<double, NSMat2<float>> fmat2dmap;
typedef std::map<double, NSMat2<double>> mat2dmap;
typedef std::map<double, NSMat2<ldouble>> lmat2dmap;

typedef std::map<double, NSMat4<char>> cmat4dmap;
typedef std::map<double, NSMat4<char16>> c16mat4dmap;
typedef std::map<double, NSMat4<char32>> c32mat4dmap;
typedef std::map<double, NSMat4<wchar>> cwmat4dmap;
typedef std::map<double, NSMat4<int16>> simat4dmap;
typedef std::map<double, NSMat4<int32>> imat4dmap;
typedef std::map<double, NSMat4<int32>> limat4dmap;
typedef std::map<double, NSMat4<int64>> llimat4dmap;
typedef std::map<double, NSMat4<uint8>> ucmat4dmap;
typedef std::map<double, NSMat4<uint16>> usimat4dmap;
typedef std::map<double, NSMat4<uint32>> uimat4dmap;
typedef std::map<double, NSMat4<uint32>> ulimat4dmap;
typedef std::map<double, NSMat4<uint64>> ullimat4dmap;
typedef std::map<double, NSMat4<float>> fmat4dmap;
typedef std::map<double, NSMat4<double>> mat4dmap;
typedef std::map<double, NSMat4<ldouble>> lmat4dmap;

typedef std::unordered_map<uint32, NSVec2<char>> cvec2uiu_map;
typedef std::unordered_map<uint32, NSVec2<char16>> c16vec2uiu_map;
typedef std::unordered_map<uint32, NSVec2<char32>> c32vec2uiu_map;
typedef std::unordered_map<uint32, NSVec2<wchar>> cwvec2uiu_map;
typedef std::unordered_map<uint32, NSVec2<int16>> sivec2uiu_map;
typedef std::unordered_map<uint32, NSVec2<int32>> ivec2uiu_map;
typedef std::unordered_map<uint32, NSVec2<int32>> livec2uiu_map;
typedef std::unordered_map<uint32, NSVec2<int64>> llivec2uiu_map;
typedef std::unordered_map<uint32, NSVec2<uint8>> ucvec2uiu_map;
typedef std::unordered_map<uint32, NSVec2<uint16>> usivec2uiu_map;
typedef std::unordered_map<uint32, NSVec2<uint32>> uivec2uiu_map;
typedef std::unordered_map<uint32, NSVec2<uint32>> ulivec2uiu_map;
typedef std::unordered_map<uint32, NSVec2<uint64>> ullivec2uiu_map;
typedef std::unordered_map<uint32, NSVec2<float>> fvec2uiu_map;
typedef std::unordered_map<uint32, NSVec2<double>> vec2uiu_map;
typedef std::unordered_map<uint32, NSVec2<ldouble>> lvec2uiu_map;

typedef std::unordered_map<uint32, NSVec3<char>> cvec3uiu_map;
typedef std::unordered_map<uint32, NSVec3<char16>> c16vec3uiu_map;
typedef std::unordered_map<uint32, NSVec3<char32>> c32vec3uiu_map;
typedef std::unordered_map<uint32, NSVec3<wchar>> cwvec3uiu_map;
typedef std::unordered_map<uint32, NSVec3<int16>> sivec3uiu_map;
typedef std::unordered_map<uint32, NSVec3<int32>> ivec3uiu_map;
typedef std::unordered_map<uint32, NSVec3<int32>> livec3uiu_map;
typedef std::unordered_map<uint32, NSVec3<int64>> llivec3uiu_map;
typedef std::unordered_map<uint32, NSVec3<uint8>> ucvec3uiu_map;
typedef std::unordered_map<uint32, NSVec3<uint16>> usivec3uiu_map;
typedef std::unordered_map<uint32, NSVec3<uint32>> uivec3uiu_map;
typedef std::unordered_map<uint32, NSVec3<uint32>> ulivec3uiu_map;
typedef std::unordered_map<uint32, NSVec3<uint64>> ullivec3uiu_map;
typedef std::unordered_map<uint32, NSVec3<float>> fvec3uiu_map;
typedef std::unordered_map<uint32, NSVec3<double>> vec3uiu_map;
typedef std::unordered_map<uint32, NSVec3<ldouble>> lvec3uiu_map;

typedef std::unordered_map<uint32, NSVec4<char>> cvec4uiu_map;
typedef std::unordered_map<uint32, NSVec4<char16>> c16vec4uiu_map;
typedef std::unordered_map<uint32, NSVec4<char32>> c32vec4uiu_map;
typedef std::unordered_map<uint32, NSVec4<wchar>> cwvec4uiu_map;
typedef std::unordered_map<uint32, NSVec4<int16>> sivec4uiu_map;
typedef std::unordered_map<uint32, NSVec4<int32>> ivec4uiu_map;
typedef std::unordered_map<uint32, NSVec4<int32>> livec4uiu_map;
typedef std::unordered_map<uint32, NSVec4<int64>> llivec4uiu_map;
typedef std::unordered_map<uint32, NSVec4<uint8>> ucvec4uiu_map;
typedef std::unordered_map<uint32, NSVec4<uint16>> usivec4uiu_map;
typedef std::unordered_map<uint32, NSVec4<uint32>> uivec4uiu_map;
typedef std::unordered_map<uint32, NSVec4<uint32>> ulivec4uiu_map;
typedef std::unordered_map<uint32, NSVec4<uint64>> ullivec4uiu_map;
typedef std::unordered_map<uint32, NSVec4<float>> fvec4uiu_map;
typedef std::unordered_map<uint32, NSVec4<double>> vec4uiu_map;
typedef std::unordered_map<uint32, NSVec4<ldouble>> lvec4uiu_map;

typedef std::unordered_map<uint32, NSQuat<char>> cquatuiu_map;
typedef std::unordered_map<uint32, NSQuat<char16>> c16quatuiu_map;
typedef std::unordered_map<uint32, NSQuat<char32>> c32quatuiu_map;
typedef std::unordered_map<uint32, NSQuat<wchar>> cwquatuiu_map;
typedef std::unordered_map<uint32, NSQuat<int16>> siquatuiu_map;
typedef std::unordered_map<uint32, NSQuat<int32>> iquatuiu_map;
typedef std::unordered_map<uint32, NSQuat<int32>> liquatuiu_map;
typedef std::unordered_map<uint32, NSQuat<int64>> lliquatuiu_map;
typedef std::unordered_map<uint32, NSQuat<uint8>> ucquatuiu_map;
typedef std::unordered_map<uint32, NSQuat<uint16>> usiquatuiu_map;
typedef std::unordered_map<uint32, NSQuat<uint32>> uiquatuiu_map;
typedef std::unordered_map<uint32, NSQuat<uint32>> uliquatuiu_map;
typedef std::unordered_map<uint32, NSQuat<uint64>> ulliquatuiu_map;
typedef std::unordered_map<uint32, NSQuat<float>> fquatuiu_map;
typedef std::unordered_map<uint32, NSQuat<double>> quatuiu_map;
typedef std::unordered_map<uint32, NSQuat<ldouble>> lquatuiu_map;

typedef std::unordered_map<uint32, NSMat2<char>> cmat2uiu_map;
typedef std::unordered_map<uint32, NSMat2<char16>> c16mat2uiu_map;
typedef std::unordered_map<uint32, NSMat2<char32>> c32mat2uiu_map;
typedef std::unordered_map<uint32, NSMat2<wchar>> cwmat2uiu_map;
typedef std::unordered_map<uint32, NSMat2<int16>> simat2uiu_map;
typedef std::unordered_map<uint32, NSMat2<int32>> imat2uiu_map;
typedef std::unordered_map<uint32, NSMat2<int32>> limat2uiu_map;
typedef std::unordered_map<uint32, NSMat2<int64>> llimat2uiu_map;
typedef std::unordered_map<uint32, NSMat2<uint8>> ucmat2uiu_map;
typedef std::unordered_map<uint32, NSMat2<uint16>> usimat2uiu_map;
typedef std::unordered_map<uint32, NSMat2<uint32>> uimat2uiu_map;
typedef std::unordered_map<uint32, NSMat2<uint32>> ulimat2uiu_map;
typedef std::unordered_map<uint32, NSMat2<uint64>> ullimat2uiu_map;
typedef std::unordered_map<uint32, NSMat2<float>> fmat2uiu_map;
typedef std::unordered_map<uint32, NSMat2<double>> mat2uiu_map;
typedef std::unordered_map<uint32, NSMat2<ldouble>> lmat2uiu_map;

typedef std::unordered_map<uint32, NSMat3<char>> cmat3uiu_map;
typedef std::unordered_map<uint32, NSMat3<char16>> c16mat3uiu_map;
typedef std::unordered_map<uint32, NSMat3<char32>> c32mat3uiu_map;
typedef std::unordered_map<uint32, NSMat3<wchar>> cwmat3uiu_map;
typedef std::unordered_map<uint32, NSMat3<int16>> simat3uiu_map;
typedef std::unordered_map<uint32, NSMat3<int32>> imat3uiu_map;
typedef std::unordered_map<uint32, NSMat3<int32>> limat3uiu_map;
typedef std::unordered_map<uint32, NSMat3<int64>> llimat3uiu_map;
typedef std::unordered_map<uint32, NSMat3<uint8>> ucmat3uiu_map;
typedef std::unordered_map<uint32, NSMat3<uint16>> usimat3uiu_map;
typedef std::unordered_map<uint32, NSMat3<uint32>> uimat3uiu_map;
typedef std::unordered_map<uint32, NSMat3<uint32>> ulimat3uiu_map;
typedef std::unordered_map<uint32, NSMat3<uint64>> ullimat3uiu_map;
typedef std::unordered_map<uint32, NSMat3<float>> fmat3uiu_map;
typedef std::unordered_map<uint32, NSMat3<double>> mat3uiu_map;
typedef std::unordered_map<uint32, NSMat3<ldouble>> lmat3uiu_map;

typedef std::unordered_map<uint32, NSMat4<char>> cmat4uiu_map;
typedef std::unordered_map<uint32, NSMat4<char16>> c16mat4uiu_map;
typedef std::unordered_map<uint32, NSMat4<char32>> c32mat4uiu_map;
typedef std::unordered_map<uint32, NSMat4<wchar>> cwmat4uiu_map;
typedef std::unordered_map<uint32, NSMat4<int16>> simat4uiu_map;
typedef std::unordered_map<uint32, NSMat4<int32>> imat4uiu_map;
typedef std::unordered_map<uint32, NSMat4<int32>> limat4uiu_map;
typedef std::unordered_map<uint32, NSMat4<int64>> llimat4uiu_map;
typedef std::unordered_map<uint32, NSMat4<uint8>> ucmat4uiu_map;
typedef std::unordered_map<uint32, NSMat4<uint16>> usimat4uiu_map;
typedef std::unordered_map<uint32, NSMat4<uint32>> uimat4uiu_map;
typedef std::unordered_map<uint32, NSMat4<uint32>> ulimat4uiu_map;
typedef std::unordered_map<uint32, NSMat4<uint64>> ullimat4uiu_map;
typedef std::unordered_map<uint32, NSMat4<float>> fmat4uiu_map;
typedef std::unordered_map<uint32, NSMat4<double>> mat4uiu_map;
typedef std::unordered_map<uint32, NSMat4<ldouble>> lmat4uiu_map;

typedef std::unordered_map<int32, NSVec2<char>> cvec2iu_map;
typedef std::unordered_map<int32, NSVec2<char16>> c16vec2iu_map;
typedef std::unordered_map<int32, NSVec2<char32>> c32vec2iu_map;
typedef std::unordered_map<int32, NSVec2<wchar>> cwvec2iu_map;
typedef std::unordered_map<int32, NSVec2<int16>> sivec2iu_map;
typedef std::unordered_map<int32, NSVec2<int32>> ivec2iu_map;
typedef std::unordered_map<int32, NSVec2<int32>> livec2iu_map;
typedef std::unordered_map<int32, NSVec2<int64>> llivec2iu_map;
typedef std::unordered_map<int32, NSVec2<uint8>> ucvec2iu_map;
typedef std::unordered_map<int32, NSVec2<uint16>> usivec2iu_map;
typedef std::unordered_map<int32, NSVec2<uint32>> uivec2iu_map;
typedef std::unordered_map<int32, NSVec2<uint32>> ulivec2iu_map;
typedef std::unordered_map<int32, NSVec2<uint64>> ullivec2iu_map;
typedef std::unordered_map<int32, NSVec2<float>> fvec2iu_map;
typedef std::unordered_map<int32, NSVec2<double>> vec2iu_map;
typedef std::unordered_map<int32, NSVec2<ldouble>> lvec2iu_map;

typedef std::unordered_map<int32, NSVec3<char>> cvec3iu_map;
typedef std::unordered_map<int32, NSVec3<char16>> c16vec3iu_map;
typedef std::unordered_map<int32, NSVec3<char32>> c32vec3iu_map;
typedef std::unordered_map<int32, NSVec3<wchar>> cwvec3iu_map;
typedef std::unordered_map<int32, NSVec3<int16>> sivec3iu_map;
typedef std::unordered_map<int32, NSVec3<int32>> ivec3iu_map;
typedef std::unordered_map<int32, NSVec3<int32>> livec3iu_map;
typedef std::unordered_map<int32, NSVec3<int64>> llivec3iu_map;
typedef std::unordered_map<int32, NSVec3<uint8>> ucvec3iu_map;
typedef std::unordered_map<int32, NSVec3<uint16>> usivec3iu_map;
typedef std::unordered_map<int32, NSVec3<uint32>> uivec3iu_map;
typedef std::unordered_map<int32, NSVec3<uint32>> ulivec3iu_map;
typedef std::unordered_map<int32, NSVec3<uint64>> ullivec3iu_map;
typedef std::unordered_map<int32, NSVec3<float>> fvec3iu_map;
typedef std::unordered_map<int32, NSVec3<double>> vec3iu_map;
typedef std::unordered_map<int32, NSVec3<ldouble>> lvec3iu_map;

typedef std::unordered_map<int32, NSVec4<char>> cvec4iu_map;
typedef std::unordered_map<int32, NSVec4<char16>> c16vec4iu_map;
typedef std::unordered_map<int32, NSVec4<char32>> c32vec4iu_map;
typedef std::unordered_map<int32, NSVec4<wchar>> cwvec4iu_map;
typedef std::unordered_map<int32, NSVec4<int16>> sivec4iu_map;
typedef std::unordered_map<int32, NSVec4<int32>> ivec4iu_map;
typedef std::unordered_map<int32, NSVec4<int32>> livec4iu_map;
typedef std::unordered_map<int32, NSVec4<int64>> llivec4iu_map;
typedef std::unordered_map<int32, NSVec4<uint8>> ucvec4iu_map;
typedef std::unordered_map<int32, NSVec4<uint16>> usivec4iu_map;
typedef std::unordered_map<int32, NSVec4<uint32>> uivec4iu_map;
typedef std::unordered_map<int32, NSVec4<uint32>> ulivec4iu_map;
typedef std::unordered_map<int32, NSVec4<uint64>> ullivec4iu_map;
typedef std::unordered_map<int32, NSVec4<float>> fvec4iu_map;
typedef std::unordered_map<int32, NSVec4<double>> vec4iu_map;
typedef std::unordered_map<int32, NSVec4<ldouble>> lvec4iu_map;

typedef std::unordered_map<int32, NSQuat<char>> cquatiu_map;
typedef std::unordered_map<int32, NSQuat<char16>> c16quatiu_map;
typedef std::unordered_map<int32, NSQuat<char32>> c32quatiu_map;
typedef std::unordered_map<int32, NSQuat<wchar>> cwquatiu_map;
typedef std::unordered_map<int32, NSQuat<int16>> siquatiu_map;
typedef std::unordered_map<int32, NSQuat<int32>> iquatiu_map;
typedef std::unordered_map<int32, NSQuat<int32>> liquatiu_map;
typedef std::unordered_map<int32, NSQuat<int64>> lliquatiu_map;
typedef std::unordered_map<int32, NSQuat<uint8>> ucquatiu_map;
typedef std::unordered_map<int32, NSQuat<uint16>> usiquatiu_map;
typedef std::unordered_map<int32, NSQuat<uint32>> uiquatiu_map;
typedef std::unordered_map<int32, NSQuat<uint32>> uliquatiu_map;
typedef std::unordered_map<int32, NSQuat<uint64>> ulliquatiu_map;
typedef std::unordered_map<int32, NSQuat<float>> fquatiu_map;
typedef std::unordered_map<int32, NSQuat<double>> quatiu_map;
typedef std::unordered_map<int32, NSQuat<ldouble>> lquatiu_map;

typedef std::unordered_map<int32, NSMat2<char>> cmat2iu_map;
typedef std::unordered_map<int32, NSMat2<char16>> c16mat2iu_map;
typedef std::unordered_map<int32, NSMat2<char32>> c32mat2iu_map;
typedef std::unordered_map<int32, NSMat2<wchar>> cwmat2iu_map;
typedef std::unordered_map<int32, NSMat2<int16>> simat2iu_map;
typedef std::unordered_map<int32, NSMat2<int32>> imat2iu_map;
typedef std::unordered_map<int32, NSMat2<int32>> limat2iu_map;
typedef std::unordered_map<int32, NSMat2<int64>> llimat2iu_map;
typedef std::unordered_map<int32, NSMat2<uint8>> ucmat2iu_map;
typedef std::unordered_map<int32, NSMat2<uint16>> usimat2iu_map;
typedef std::unordered_map<int32, NSMat2<uint32>> uimat2iu_map;
typedef std::unordered_map<int32, NSMat2<uint32>> ulimat2iu_map;
typedef std::unordered_map<int32, NSMat2<uint64>> ullimat2iu_map;
typedef std::unordered_map<int32, NSMat2<float>> fmat2iu_map;
typedef std::unordered_map<int32, NSMat2<double>> mat2iu_map;
typedef std::unordered_map<int32, NSMat2<ldouble>> lmat2iu_map;

typedef std::unordered_map<int32, NSMat3<char>> cmat3iu_map;
typedef std::unordered_map<int32, NSMat3<char16>> c16mat3iu_map;
typedef std::unordered_map<int32, NSMat3<char32>> c32mat3iu_map;
typedef std::unordered_map<int32, NSMat3<wchar>> cwmat3iu_map;
typedef std::unordered_map<int32, NSMat3<int16>> simat3iu_map;
typedef std::unordered_map<int32, NSMat3<int32>> imat3iu_map;
typedef std::unordered_map<int32, NSMat3<int32>> limat3iu_map;
typedef std::unordered_map<int32, NSMat3<int64>> llimat3iu_map;
typedef std::unordered_map<int32, NSMat3<uint8>> ucmat3iu_map;
typedef std::unordered_map<int32, NSMat3<uint16>> usimat3iu_map;
typedef std::unordered_map<int32, NSMat3<uint32>> uimat3iu_map;
typedef std::unordered_map<int32, NSMat3<uint32>> ulimat3iu_map;
typedef std::unordered_map<int32, NSMat3<uint64>> ullimat3iu_map;
typedef std::unordered_map<int32, NSMat3<float>> fmat3iu_map;
typedef std::unordered_map<int32, NSMat3<double>> mat3iu_map;
typedef std::unordered_map<int32, NSMat3<ldouble>> lmat3iu_map;

typedef std::unordered_map<int32, NSMat4<char>> cmat4iu_map;
typedef std::unordered_map<int32, NSMat4<char16>> c16mat4iu_map;
typedef std::unordered_map<int32, NSMat4<char32>> c32mat4iu_map;
typedef std::unordered_map<int32, NSMat4<wchar>> cwmat4iu_map;
typedef std::unordered_map<int32, NSMat4<int16>> simat4iu_map;
typedef std::unordered_map<int32, NSMat4<int32>> imat4iu_map;
typedef std::unordered_map<int32, NSMat4<int32>> limat4iu_map;
typedef std::unordered_map<int32, NSMat4<int64>> llimat4iu_map;
typedef std::unordered_map<int32, NSMat4<uint8>> ucmat4iu_map;
typedef std::unordered_map<int32, NSMat4<uint16>> usimat4iu_map;
typedef std::unordered_map<int32, NSMat4<uint32>> uimat4iu_map;
typedef std::unordered_map<int32, NSMat4<uint32>> ulimat4iu_map;
typedef std::unordered_map<int32, NSMat4<uint64>> ullimat4iu_map;
typedef std::unordered_map<int32, NSMat4<float>> fmat4iu_map;
typedef std::unordered_map<int32, NSMat4<double>> mat4iu_map;
typedef std::unordered_map<int32, NSMat4<ldouble>> lmat4iu_map;

typedef std::unordered_map<float, NSVec2<char>> cvec2fu_map;
typedef std::unordered_map<float, NSVec2<char16>> c16vec2fu_map;
typedef std::unordered_map<float, NSVec2<char32>> c32vec2fu_map;
typedef std::unordered_map<float, NSVec2<wchar>> cwvec2fu_map;
typedef std::unordered_map<float, NSVec2<int16>> sivec2fu_map;
typedef std::unordered_map<float, NSVec2<int32>> ivec2fu_map;
typedef std::unordered_map<float, NSVec2<int32>> livec2fu_map;
typedef std::unordered_map<float, NSVec2<int64>> llivec2fu_map;
typedef std::unordered_map<float, NSVec2<uint8>> ucvec2fu_map;
typedef std::unordered_map<float, NSVec2<uint16>> usivec2fu_map;
typedef std::unordered_map<float, NSVec2<uint32>> uivec2fu_map;
typedef std::unordered_map<float, NSVec2<uint32>> ulivec2fu_map;
typedef std::unordered_map<float, NSVec2<uint64>> ullivec2fu_map;
typedef std::unordered_map<float, NSVec2<float>> fvec2fu_map;
typedef std::unordered_map<float, NSVec2<double>> vec2fu_map;
typedef std::unordered_map<float, NSVec2<ldouble>> lvec2fu_map;

typedef std::unordered_map<float, NSVec3<char>> cvec3fu_map;
typedef std::unordered_map<float, NSVec3<char16>> c16vec3fu_map;
typedef std::unordered_map<float, NSVec3<char32>> c32vec3fu_map;
typedef std::unordered_map<float, NSVec3<wchar>> cwvec3fu_map;
typedef std::unordered_map<float, NSVec3<int16>> sivec3fu_map;
typedef std::unordered_map<float, NSVec3<int32>> ivec3fu_map;
typedef std::unordered_map<float, NSVec3<int32>> livec3fu_map;
typedef std::unordered_map<float, NSVec3<int64>> llivec3fu_map;
typedef std::unordered_map<float, NSVec3<uint8>> ucvec3fu_map;
typedef std::unordered_map<float, NSVec3<uint16>> usivec3fu_map;
typedef std::unordered_map<float, NSVec3<uint32>> uivec3fu_map;
typedef std::unordered_map<float, NSVec3<uint32>> ulivec3fu_map;
typedef std::unordered_map<float, NSVec3<uint64>> ullivec3fu_map;
typedef std::unordered_map<float, NSVec3<float>> fvec3fu_map;
typedef std::unordered_map<float, NSVec3<double>> vec3fu_map;
typedef std::unordered_map<float, NSVec3<ldouble>> lvec3fu_map;

typedef std::unordered_map<float, NSVec4<char>> cvec4fu_map;
typedef std::unordered_map<float, NSVec4<char16>> c16vec4fu_map;
typedef std::unordered_map<float, NSVec4<char32>> c32vec4fu_map;
typedef std::unordered_map<float, NSVec4<wchar>> cwvec4fu_map;
typedef std::unordered_map<float, NSVec4<int16>> sivec4fu_map;
typedef std::unordered_map<float, NSVec4<int32>> ivec4fu_map;
typedef std::unordered_map<float, NSVec4<int32>> livec4fu_map;
typedef std::unordered_map<float, NSVec4<int64>> llivec4fu_map;
typedef std::unordered_map<float, NSVec4<uint8>> ucvec4fu_map;
typedef std::unordered_map<float, NSVec4<uint16>> usivec4fu_map;
typedef std::unordered_map<float, NSVec4<uint32>> uivec4fu_map;
typedef std::unordered_map<float, NSVec4<uint32>> ulivec4fu_map;
typedef std::unordered_map<float, NSVec4<uint64>> ullivec4fu_map;
typedef std::unordered_map<float, NSVec4<float>> fvec4fu_map;
typedef std::unordered_map<float, NSVec4<double>> vec4fu_map;
typedef std::unordered_map<float, NSVec4<ldouble>> lvec4fu_map;

typedef std::unordered_map<float, NSQuat<char>> cquatfu_map;
typedef std::unordered_map<float, NSQuat<char16>> c16quatfu_map;
typedef std::unordered_map<float, NSQuat<char32>> c32quatfu_map;
typedef std::unordered_map<float, NSQuat<wchar>> cwquatfu_map;
typedef std::unordered_map<float, NSQuat<int16>> siquatfu_map;
typedef std::unordered_map<float, NSQuat<int32>> iquatfu_map;
typedef std::unordered_map<float, NSQuat<int32>> liquatfu_map;
typedef std::unordered_map<float, NSQuat<int64>> lliquatfu_map;
typedef std::unordered_map<float, NSQuat<uint8>> ucquatfu_map;
typedef std::unordered_map<float, NSQuat<uint16>> usiquatfu_map;
typedef std::unordered_map<float, NSQuat<uint32>> uiquatfu_map;
typedef std::unordered_map<float, NSQuat<uint32>> uliquatfu_map;
typedef std::unordered_map<float, NSQuat<uint64>> ulliquatfu_map;
typedef std::unordered_map<float, NSQuat<float>> fquatfu_map;
typedef std::unordered_map<float, NSQuat<double>> quatfu_map;
typedef std::unordered_map<float, NSQuat<ldouble>> lquatfu_map;

typedef std::unordered_map<float, NSMat2<char>> cmat2fu_map;
typedef std::unordered_map<float, NSMat2<char16>> c16mat2fu_map;
typedef std::unordered_map<float, NSMat2<char32>> c32mat2fu_map;
typedef std::unordered_map<float, NSMat2<wchar>> cwmat2fu_map;
typedef std::unordered_map<float, NSMat2<int16>> simat2fu_map;
typedef std::unordered_map<float, NSMat2<int32>> imat2fu_map;
typedef std::unordered_map<float, NSMat2<int32>> limat2fu_map;
typedef std::unordered_map<float, NSMat2<int64>> llimat2fu_map;
typedef std::unordered_map<float, NSMat2<uint8>> ucmat2fu_map;
typedef std::unordered_map<float, NSMat2<uint16>> usimat2fu_map;
typedef std::unordered_map<float, NSMat2<uint32>> uimat2fu_map;
typedef std::unordered_map<float, NSMat2<uint32>> ulimat2fu_map;
typedef std::unordered_map<float, NSMat2<uint64>> ullimat2fu_map;
typedef std::unordered_map<float, NSMat2<float>> fmat2fu_map;
typedef std::unordered_map<float, NSMat2<double>> mat2fu_map;
typedef std::unordered_map<float, NSMat2<ldouble>> lmat2fu_map;

typedef std::unordered_map<float, NSMat3<char>> cmat3fu_map;
typedef std::unordered_map<float, NSMat3<char16>> c16mat3fu_map;
typedef std::unordered_map<float, NSMat3<char32>> c32mat3fu_map;
typedef std::unordered_map<float, NSMat3<wchar>> cwmat3fu_map;
typedef std::unordered_map<float, NSMat3<int16>> simat3fu_map;
typedef std::unordered_map<float, NSMat3<int32>> imat3fu_map;
typedef std::unordered_map<float, NSMat3<int32>> limat3fu_map;
typedef std::unordered_map<float, NSMat3<int64>> llimat3fu_map;
typedef std::unordered_map<float, NSMat3<uint8>> ucmat3fu_map;
typedef std::unordered_map<float, NSMat3<uint16>> usimat3fu_map;
typedef std::unordered_map<float, NSMat3<uint32>> uimat3fu_map;
typedef std::unordered_map<float, NSMat3<uint32>> ulimat3fu_map;
typedef std::unordered_map<float, NSMat3<uint64>> ullimat3fu_map;
typedef std::unordered_map<float, NSMat3<float>> fmat3fu_map;
typedef std::unordered_map<float, NSMat3<double>> mat3fu_map;
typedef std::unordered_map<float, NSMat3<ldouble>> lmat3fu_map;

typedef std::unordered_map<float, NSMat4<char>> cmat4fu_map;
typedef std::unordered_map<float, NSMat4<char16>> c16mat4fu_map;
typedef std::unordered_map<float, NSMat4<char32>> c32mat4fu_map;
typedef std::unordered_map<float, NSMat4<wchar>> cwmat4fu_map;
typedef std::unordered_map<float, NSMat4<int16>> simat4fu_map;
typedef std::unordered_map<float, NSMat4<int32>> imat4fu_map;
typedef std::unordered_map<float, NSMat4<int32>> limat4fu_map;
typedef std::unordered_map<float, NSMat4<int64>> llimat4fu_map;
typedef std::unordered_map<float, NSMat4<uint8>> ucmat4fu_map;
typedef std::unordered_map<float, NSMat4<uint16>> usimat4fu_map;
typedef std::unordered_map<float, NSMat4<uint32>> uimat4fu_map;
typedef std::unordered_map<float, NSMat4<uint32>> ulimat4fu_map;
typedef std::unordered_map<float, NSMat4<uint64>> ullimat4fu_map;
typedef std::unordered_map<float, NSMat4<float>> fmat4fu_map;
typedef std::unordered_map<float, NSMat4<double>> mat4fu_map;
typedef std::unordered_map<float, NSMat4<ldouble>> lmat4fu_map;

typedef std::unordered_map<double, NSVec2<char>> cvec2du_map;
typedef std::unordered_map<double, NSVec2<char16>> c16vec2du_map;
typedef std::unordered_map<double, NSVec2<char32>> c32vec2du_map;
typedef std::unordered_map<double, NSVec2<wchar>> cwvec2du_map;
typedef std::unordered_map<double, NSVec2<int16>> sivec2du_map;
typedef std::unordered_map<double, NSVec2<int32>> ivec2du_map;
typedef std::unordered_map<double, NSVec2<int32>> livec2du_map;
typedef std::unordered_map<double, NSVec2<int64>> llivec2du_map;
typedef std::unordered_map<double, NSVec2<uint8>> ucvec2du_map;
typedef std::unordered_map<double, NSVec2<uint16>> usivec2du_map;
typedef std::unordered_map<double, NSVec2<uint32>> uivec2du_map;
typedef std::unordered_map<double, NSVec2<uint32>> ulivec2du_map;
typedef std::unordered_map<double, NSVec2<uint64>> ullivec2du_map;
typedef std::unordered_map<double, NSVec2<float>> fvec2du_map;
typedef std::unordered_map<double, NSVec2<double>> vec2du_map;
typedef std::unordered_map<double, NSVec2<ldouble>> lvec2du_map;

typedef std::unordered_map<double, NSVec3<char>> cvec3du_map;
typedef std::unordered_map<double, NSVec3<char16>> c16vec3du_map;
typedef std::unordered_map<double, NSVec3<char32>> c32vec3du_map;
typedef std::unordered_map<double, NSVec3<wchar>> cwvec3du_map;
typedef std::unordered_map<double, NSVec3<int16>> sivec3du_map;
typedef std::unordered_map<double, NSVec3<int32>> ivec3du_map;
typedef std::unordered_map<double, NSVec3<int32>> livec3du_map;
typedef std::unordered_map<double, NSVec3<int64>> llivec3du_map;
typedef std::unordered_map<double, NSVec3<uint8>> ucvec3du_map;
typedef std::unordered_map<double, NSVec3<uint16>> usivec3du_map;
typedef std::unordered_map<double, NSVec3<uint32>> uivec3du_map;
typedef std::unordered_map<double, NSVec3<uint32>> ulivec3du_map;
typedef std::unordered_map<double, NSVec3<uint64>> ullivec3du_map;
typedef std::unordered_map<double, NSVec3<float>> fvec3du_map;
typedef std::unordered_map<double, NSVec3<double>> vec3du_map;
typedef std::unordered_map<double, NSVec3<ldouble>> lvec3du_map;

typedef std::unordered_map<double, NSVec4<char>> cvec4du_map;
typedef std::unordered_map<double, NSVec4<char16>> c16vec4du_map;
typedef std::unordered_map<double, NSVec4<char32>> c32vec4du_map;
typedef std::unordered_map<double, NSVec4<wchar>> cwvec4du_map;
typedef std::unordered_map<double, NSVec4<int16>> sivec4du_map;
typedef std::unordered_map<double, NSVec4<int32>> ivec4du_map;
typedef std::unordered_map<double, NSVec4<int32>> livec4du_map;
typedef std::unordered_map<double, NSVec4<int64>> llivec4du_map;
typedef std::unordered_map<double, NSVec4<uint8>> ucvec4du_map;
typedef std::unordered_map<double, NSVec4<uint16>> usivec4du_map;
typedef std::unordered_map<double, NSVec4<uint32>> uivec4du_map;
typedef std::unordered_map<double, NSVec4<uint32>> ulivec4du_map;
typedef std::unordered_map<double, NSVec4<uint64>> ullivec4du_map;
typedef std::unordered_map<double, NSVec4<float>> fvec4du_map;
typedef std::unordered_map<double, NSVec4<double>> vec4du_map;
typedef std::unordered_map<double, NSVec4<ldouble>> lvec4du_map;

typedef std::unordered_map<double, NSQuat<char>> cquatdu_map;
typedef std::unordered_map<double, NSQuat<char16>> c16quatdu_map;
typedef std::unordered_map<double, NSQuat<char32>> c32quatdu_map;
typedef std::unordered_map<double, NSQuat<wchar>> cwquatdu_map;
typedef std::unordered_map<double, NSQuat<int16>> siquatdu_map;
typedef std::unordered_map<double, NSQuat<int32>> iquatdu_map;
typedef std::unordered_map<double, NSQuat<int32>> liquatdu_map;
typedef std::unordered_map<double, NSQuat<int64>> lliquatdu_map;
typedef std::unordered_map<double, NSQuat<uint8>> ucquatdu_map;
typedef std::unordered_map<double, NSQuat<uint16>> usiquatdu_map;
typedef std::unordered_map<double, NSQuat<uint32>> uiquatdu_map;
typedef std::unordered_map<double, NSQuat<uint32>> uliquatdu_map;
typedef std::unordered_map<double, NSQuat<uint64>> ulliquatdu_map;
typedef std::unordered_map<double, NSQuat<float>> fquatdu_map;
typedef std::unordered_map<double, NSQuat<double>> quatdu_map;
typedef std::unordered_map<double, NSQuat<ldouble>> lquatdu_map;

typedef std::unordered_map<double, NSMat2<char>> cmat2du_map;
typedef std::unordered_map<double, NSMat2<char16>> c16mat2du_map;
typedef std::unordered_map<double, NSMat2<char32>> c32mat2du_map;
typedef std::unordered_map<double, NSMat2<wchar>> cwmat2du_map;
typedef std::unordered_map<double, NSMat2<int16>> simat2du_map;
typedef std::unordered_map<double, NSMat2<int32>> imat2du_map;
typedef std::unordered_map<double, NSMat2<int32>> limat2du_map;
typedef std::unordered_map<double, NSMat2<int64>> llimat2du_map;
typedef std::unordered_map<double, NSMat2<uint8>> ucmat2du_map;
typedef std::unordered_map<double, NSMat2<uint16>> usimat2du_map;
typedef std::unordered_map<double, NSMat2<uint32>> uimat2du_map;
typedef std::unordered_map<double, NSMat2<uint32>> ulimat2du_map;
typedef std::unordered_map<double, NSMat2<uint64>> ullimat2du_map;
typedef std::unordered_map<double, NSMat2<float>> fmat2du_map;
typedef std::unordered_map<double, NSMat2<double>> mat2du_map;
typedef std::unordered_map<double, NSMat2<ldouble>> lmat2du_map;

typedef std::unordered_map<double, NSMat3<char>> cmat3du_map;
typedef std::unordered_map<double, NSMat3<char16>> c16mat3du_map;
typedef std::unordered_map<double, NSMat3<char32>> c32mat3du_map;
typedef std::unordered_map<double, NSMat3<wchar>> cwmat3du_map;
typedef std::unordered_map<double, NSMat3<int16>> simat3du_map;
typedef std::unordered_map<double, NSMat3<int32>> imat3du_map;
typedef std::unordered_map<double, NSMat3<int32>> limat3du_map;
typedef std::unordered_map<double, NSMat3<int64>> llimat3du_map;
typedef std::unordered_map<double, NSMat3<uint8>> ucmat3du_map;
typedef std::unordered_map<double, NSMat3<uint16>> usimat3du_map;
typedef std::unordered_map<double, NSMat3<uint32>> uimat3du_map;
typedef std::unordered_map<double, NSMat3<uint32>> ulimat3du_map;
typedef std::unordered_map<double, NSMat3<uint64>> ullimat3du_map;
typedef std::unordered_map<double, NSMat3<float>> fmat3du_map;
typedef std::unordered_map<double, NSMat3<double>> mat3du_map;
typedef std::unordered_map<double, NSMat3<ldouble>> lmat3du_map;

typedef std::unordered_map<double, NSMat4<char>> cmat4du_map;
typedef std::unordered_map<double, NSMat4<char16>> c16mat4du_map;
typedef std::unordered_map<double, NSMat4<char32>> c32mat4du_map;
typedef std::unordered_map<double, NSMat4<wchar>> cwmat4du_map;
typedef std::unordered_map<double, NSMat4<int16>> simat4du_map;
typedef std::unordered_map<double, NSMat4<int32>> imat4du_map;
typedef std::unordered_map<double, NSMat4<int32>> limat4du_map;
typedef std::unordered_map<double, NSMat4<int64>> llimat4du_map;
typedef std::unordered_map<double, NSMat4<uint8>> ucmat4du_map;
typedef std::unordered_map<double, NSMat4<uint16>> usimat4du_map;
typedef std::unordered_map<double, NSMat4<uint32>> uimat4du_map;
typedef std::unordered_map<double, NSMat4<uint32>> ulimat4du_map;
typedef std::unordered_map<double, NSMat4<uint64>> ullimat4du_map;
typedef std::unordered_map<double, NSMat4<float>> fmat4du_map;
typedef std::unordered_map<double, NSMat4<double>> mat4du_map;
typedef std::unordered_map<double, NSMat4<ldouble>> lmat4du_map;

// Math typedefs
typedef NSVec2<char> cvec2;
typedef NSVec2<char16> c16vec2;
typedef NSVec2<char32> c32vec2;
typedef NSVec2<wchar> cwvec2;
typedef NSVec2<int16> sivec2;
typedef NSVec2<int32> ivec2;
typedef NSVec2<int32> livec2;
typedef NSVec2<int64> liivec2;
typedef NSVec2<uint8> ucvec2;
typedef NSVec2<uint16> usivec2;
typedef NSVec2<uint32> uivec2;
typedef NSVec2<uint32> ulivec2;
typedef NSVec2<uint64> ullivec2;
typedef NSVec2<float> fvec2;
typedef NSVec2<double> vec2;
typedef NSVec2<ldouble> lvec2;
typedef NSVec3<char> cvec3;
typedef NSVec3<char16> c16vec3;
typedef NSVec3<char32> c32vec3;
typedef NSVec3<wchar> cwvec3;
typedef NSVec3<int16> sivec3;
typedef NSVec3<int32> ivec3;
typedef NSVec3<int32> livec3;
typedef NSVec3<int64> liivec3;
typedef NSVec3<uint8> ucvec3;
typedef NSVec3<uint16> usivec3;
typedef NSVec3<uint32> uivec3;
typedef NSVec3<uint32> ulivec3;
typedef NSVec3<uint64> ullivec3;
typedef NSVec3<float> fvec3;
typedef NSVec3<double> vec3;
typedef NSVec3<ldouble> lvec3;
typedef NSVec4<char> cvec4;
typedef NSVec4<char16> c16vec4;
typedef NSVec4<char32> c32vec4;
typedef NSVec4<wchar> cwvec4;
typedef NSVec4<int16> sivec4;
typedef NSVec4<int32> ivec4;
typedef NSVec4<int32> livec4;
typedef NSVec4<int64> liivec4;
typedef NSVec4<uint8> ucvec4;
typedef NSVec4<uint16> usivec4;
typedef NSVec4<uint32> uivec4;
typedef NSVec4<uint32> ulivec4;
typedef NSVec4<uint64> ullivec4;
typedef NSVec4<float> fvec4;
typedef NSVec4<double> vec4;
typedef NSVec4<ldouble> lvec4;
typedef NSQuat<char> cquat;
typedef NSQuat<char16> c16quat;
typedef NSQuat<char32> c32quat;
typedef NSQuat<wchar> cwquat;
typedef NSQuat<int16> siquat;
typedef NSQuat<int32> iquat;
typedef NSQuat<int32> liquat;
typedef NSQuat<int64> liiquat;
typedef NSQuat<uint8> ucquat;
typedef NSQuat<uint16> usiquat;
typedef NSQuat<uint32> uiquat;
typedef NSQuat<uint32> uliquat;
typedef NSQuat<uint64> ulliquat;
typedef NSQuat<float> fquat;
typedef NSQuat<double> quat;
typedef NSQuat<ldouble> lquat;
typedef NSMat2<char> cmat2;
typedef NSMat2<char16> c16mat2;
typedef NSMat2<char32> c32mat2;
typedef NSMat2<wchar> cwmat2;
typedef NSMat2<int16> simat2;
typedef NSMat2<int32> imat2;
typedef NSMat2<int32> limat2;
typedef NSMat2<int64> liimat2;
typedef NSMat2<uint8> ucmat2;
typedef NSMat2<uint16> usimat2;
typedef NSMat2<uint32> uimat2;
typedef NSMat2<uint32> ulimat2;
typedef NSMat2<uint64> ullimat2;
typedef NSMat2<float> fmat2;
typedef NSMat2<double> mat2;
typedef NSMat2<ldouble> lmat2;
typedef NSMat3<char> cmat3;
typedef NSMat3<char16> c16mat3;
typedef NSMat3<char32> c32mat3;
typedef NSMat3<wchar> cwmat3;
typedef NSMat3<int16> simat3;
typedef NSMat3<int32> imat3;
typedef NSMat3<int32> limat3;
typedef NSMat3<int64> liimat3;
typedef NSMat3<uint8> ucmat3;
typedef NSMat3<uint16> usimat3;
typedef NSMat3<uint32> uimat3;
typedef NSMat3<uint32> ulimat3;
typedef NSMat3<uint64> ullimat3;
typedef NSMat3<float> fmat3;
typedef NSMat3<double> mat3;
typedef NSMat3<ldouble> lmat3;
typedef NSMat4<char> cmat4;
typedef NSMat4<char16> c16mat4;
typedef NSMat4<char32> c32mat4;
typedef NSMat4<wchar> cwmat4;
typedef NSMat4<int16> simat4;
typedef NSMat4<int32> imat4;
typedef NSMat4<int32> limat4;
typedef NSMat4<int64> liimat4;
typedef NSMat4<uint8> ucmat4;
typedef NSMat4<uint16> usimat4;
typedef NSMat4<uint32> uimat4;
typedef NSMat4<uint32> ulimat4;
typedef NSMat4<uint64> ullimat4;
typedef NSMat4<float> fmat4;
typedef NSMat4<double> mat4;
typedef NSMat4<ldouble> lmat4;

template<class T>
T degrees(const T & val)
{
	return static_cast<T>(180 / PI) * val;
}

template<class T>
T radians(const T & val)
{
	return static_cast<T>(PI / 180) * val;
}

struct NSBoundingBox
{
	NSBoundingBox(
		const std::vector<fvec3> & pVertices = std::vector<fvec3>()
		);

	enum Face {
		None,
		Bottom,
		Top,
		Left,
		Right,
		Back,
		Front
	};

	void calculate(
		const std::vector<fvec3> & pVertices,
		const fmat4 & pTransform = fmat4()
		);

	fvec3 center(const Face & pFace = None);

	void clear();

	float dx();
	float dy();
	float dz();

	void extend(
		const std::vector<fvec3> & pVertices,
		const fmat4 & pTransform = fmat4()
		);

	void set(
		const fvec3 & pMin,
		const fvec3 pMax
		);

	float volume();

	fvec3 mMin;
	fvec3 mMax;
	fvec3 mVerts[8];

private:
	void _updateVerts();
};

template<class PUPer>
void pup(PUPer & p, NSBoundingBox & box, const nsstring & varName)
{
	pup(p, box.mMin, varName + ".mMin");
	pup(p, box.mMax, varName + ".mMax");
	for (uint32 i = 0; i < 8; ++i)
		pup(p, box.mVerts[i], varName + "vert[" + std::to_string(i) + "]");
}

#endif
