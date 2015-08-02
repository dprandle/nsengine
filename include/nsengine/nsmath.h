#ifndef NSMATH_H
#define NSMATH_H

#define PI 3.14159265359f
#define EPS 0.0001f

#include <cmath>
#include <nsglobal.h>

nsfloat clampf(nsfloat pVal, const nsfloat & pMin, const nsfloat & pMax);
nsdouble clamp(nsdouble pVal, const nsdouble & pMin, const nsdouble & pMax);
nsfloat fractf(const nsfloat & num);
nsdouble fract(const nsdouble & num);

nsfloat lerp(nsint low, nsint high, nsint middle);
nsfloat lerp(nsuint low, nsuint high, nsuint middle);
nsfloat lerp(nsfloat low, nsfloat high, nsfloat middle);
nsfloat lerp(nsfloat low, nsfloat high, nsfloat middle);
nsdouble lerp(nsdouble low, nsdouble high, nsdouble middle);

template<class T>
T degrees(const T & val);

template<class T>
T radians(const T & val);

float RandomFloat(float pHigh = 1.0f, float pLow = 0.0f);

#include "nsmat4.h"

typedef std::vector<NSVec2<nschar>> cvec2array;
typedef std::vector<NSVec2<nschar16>> c16vec2array;
typedef std::vector<NSVec2<nschar32>> c32vec2array;
typedef std::vector<NSVec2<nswchar>> cwvec2array;
typedef std::vector<NSVec2<nssint>> sivec2array;
typedef std::vector<NSVec2<nsint>> ivec2array;
typedef std::vector<NSVec2<nslint>> livec2array;
typedef std::vector<NSVec2<nsllint>> llivec2array;
typedef std::vector<NSVec2<nsuchar>> ucvec2array;
typedef std::vector<NSVec2<nsusint>> usivec2array;
typedef std::vector<NSVec2<nsuint>> uivec2array;
typedef std::vector<NSVec2<nsulint>> ulivec2array;
typedef std::vector<NSVec2<nsullint>> ullivec2array;
typedef std::vector<NSVec2<nsfloat>> fvec2array;
typedef std::vector<NSVec2<nsdouble>> vec2array;
typedef std::vector<NSVec2<nsldouble>> lvec2array;

typedef std::vector<NSVec3<nschar>> cvec3array;
typedef std::vector<NSVec3<nschar16>> c16vec3array;
typedef std::vector<NSVec3<nschar32>> c32vec3array;
typedef std::vector<NSVec3<nswchar>> cwvec3array;
typedef std::vector<NSVec3<nssint>> sivec3array;
typedef std::vector<NSVec3<nsint>> ivec3array;
typedef std::vector<NSVec3<nslint>> livec3array;
typedef std::vector<NSVec3<nsllint>> llivec3array;
typedef std::vector<NSVec3<nsuchar>> ucvec3array;
typedef std::vector<NSVec3<nsusint>> usivec3array;
typedef std::vector<NSVec3<nsuint>> uivec3array;
typedef std::vector<NSVec3<nsulint>> ulivec3array;
typedef std::vector<NSVec3<nsullint>> ullivec3array;
typedef std::vector<NSVec3<nsfloat>> fvec3array;
typedef std::vector<NSVec3<nsdouble>> vec3array;
typedef std::vector<NSVec3<nsldouble>> lvec3array;

typedef std::vector<NSVec4<nschar>> cvec4array;
typedef std::vector<NSVec4<nschar16>> c16vec4array;
typedef std::vector<NSVec4<nschar32>> c32vec4array;
typedef std::vector<NSVec4<nswchar>> cwvec4array;
typedef std::vector<NSVec4<nssint>> sivec4array;
typedef std::vector<NSVec4<nsint>> ivec4array;
typedef std::vector<NSVec4<nslint>> livec4array;
typedef std::vector<NSVec4<nsllint>> llivec4array;
typedef std::vector<NSVec4<nsuchar>> ucvec4array;
typedef std::vector<NSVec4<nsusint>> usivec4array;
typedef std::vector<NSVec4<nsuint>> uivec4array;
typedef std::vector<NSVec4<nsulint>> ulivec4array;
typedef std::vector<NSVec4<nsullint>> ullivec4array;
typedef std::vector<NSVec4<nsfloat>> fvec4array;
typedef std::vector<NSVec4<nsdouble>> vec4array;
typedef std::vector<NSVec4<nsldouble>> lvec4array;

typedef std::vector<NSQuat<nschar>> cquatarray;
typedef std::vector<NSQuat<nschar16>> c16quatarray;
typedef std::vector<NSQuat<nschar32>> c32quatarray;
typedef std::vector<NSQuat<nswchar>> cwquatarray;
typedef std::vector<NSQuat<nssint>> siquatarray;
typedef std::vector<NSQuat<nsint>> iquatarray;
typedef std::vector<NSQuat<nslint>> liquatarray;
typedef std::vector<NSQuat<nsllint>> lliquatarray;
typedef std::vector<NSQuat<nsuchar>> ucquatarray;
typedef std::vector<NSQuat<nsusint>> usiquatarray;
typedef std::vector<NSQuat<nsuint>> uiquatarray;
typedef std::vector<NSQuat<nsulint>> uliquatarray;
typedef std::vector<NSQuat<nsullint>> ulliquatarray;
typedef std::vector<NSQuat<nsfloat>> fquatarray;
typedef std::vector<NSQuat<nsdouble>> quatarray;
typedef std::vector<NSQuat<nsldouble>> lquatarray;

typedef std::vector<NSMat2<nschar>> cmat2array;
typedef std::vector<NSMat2<nschar16>> c16mat2array;
typedef std::vector<NSMat2<nschar32>> c32mat2array;
typedef std::vector<NSMat2<nswchar>> cwmat2array;
typedef std::vector<NSMat2<nssint>> simat2array;
typedef std::vector<NSMat2<nsint>> imat2array;
typedef std::vector<NSMat2<nslint>> limat2array;
typedef std::vector<NSMat2<nsllint>> llimat2array;
typedef std::vector<NSMat2<nsuchar>> ucmat2array;
typedef std::vector<NSMat2<nsusint>> usimat2array;
typedef std::vector<NSMat2<nsuint>> uimat2array;
typedef std::vector<NSMat2<nsulint>> ulimat2array;
typedef std::vector<NSMat2<nsullint>> ullimat2array;
typedef std::vector<NSMat2<nsfloat>> fmat2array;
typedef std::vector<NSMat2<nsdouble>> mat2array;
typedef std::vector<NSMat2<nsldouble>> lmat2array;

typedef std::vector<NSMat3<nschar>> cmat3array;
typedef std::vector<NSMat3<nschar16>> c16mat3array;
typedef std::vector<NSMat3<nschar32>> c32mat3array;
typedef std::vector<NSMat3<nswchar>> cwmat3array;
typedef std::vector<NSMat3<nssint>> simat3array;
typedef std::vector<NSMat3<nsint>> imat3array;
typedef std::vector<NSMat3<nslint>> limat3array;
typedef std::vector<NSMat3<nsllint>> llimat3array;
typedef std::vector<NSMat3<nsuchar>> ucmat3array;
typedef std::vector<NSMat3<nsusint>> usimat3array;
typedef std::vector<NSMat3<nsuint>> uimat3array;
typedef std::vector<NSMat3<nsulint>> ulimat3array;
typedef std::vector<NSMat3<nsullint>> ullimat3array;
typedef std::vector<NSMat3<nsfloat>> fmat3array;
typedef std::vector<NSMat3<nsdouble>> mat3array;
typedef std::vector<NSMat3<nsldouble>> lmat3array;

typedef std::vector<NSMat4<nschar>> cmat4array;
typedef std::vector<NSMat4<nschar16>> c16mat4array;
typedef std::vector<NSMat4<nschar32>> c32mat4array;
typedef std::vector<NSMat4<nswchar>> cwmat4array;
typedef std::vector<NSMat4<nssint>> simat4array;
typedef std::vector<NSMat4<nsint>> imat4array;
typedef std::vector<NSMat4<nslint>> limat4array;
typedef std::vector<NSMat4<nsllint>> llimat4array;
typedef std::vector<NSMat4<nsuchar>> ucmat4array;
typedef std::vector<NSMat4<nsusint>> usimat4array;
typedef std::vector<NSMat4<nsuint>> uimat4array;
typedef std::vector<NSMat4<nsulint>> ulimat4array;
typedef std::vector<NSMat4<nsullint>> ullimat4array;
typedef std::vector<NSMat4<nsfloat>> fmat4array;
typedef std::vector<NSMat4<nsdouble>> mat4array;
typedef std::vector<NSMat4<nsldouble>> lmat4array;

typedef std::map<nsuint,NSVec2<nschar>> cvec2uimap;
typedef std::map<nsuint,NSVec2<nschar16>> c16vec2uimap;
typedef std::map<nsuint,NSVec2<nschar32>> c32vec2uimap;
typedef std::map<nsuint,NSVec2<nswchar>> cwvec2uimap;
typedef std::map<nsuint,NSVec2<nssint>> sivec2uimap;
typedef std::map<nsuint,NSVec2<nsint>> ivec2uimap;
typedef std::map<nsuint,NSVec2<nslint>> livec2uimap;
typedef std::map<nsuint,NSVec2<nsllint>> llivec2uimap;
typedef std::map<nsuint,NSVec2<nsuchar>> ucvec2uimap;
typedef std::map<nsuint,NSVec2<nsusint>> usivec2uimap;
typedef std::map<nsuint,NSVec2<nsuint>> uivec2uimap;
typedef std::map<nsuint,NSVec2<nsulint>> ulivec2uimap;
typedef std::map<nsuint,NSVec2<nsullint>> ullivec2uimap;
typedef std::map<nsuint,NSVec2<nsfloat>> fvec2uimap;
typedef std::map<nsuint,NSVec2<nsdouble>> vec2uimap;
typedef std::map<nsuint,NSVec2<nsldouble>> lvec2uimap;

typedef std::map<nsuint,NSVec3<nschar>> cvec3uimap;
typedef std::map<nsuint,NSVec3<nschar16>> c16vec3uimap;
typedef std::map<nsuint,NSVec3<nschar32>> c32vec3uimap;
typedef std::map<nsuint,NSVec3<nswchar>> cwvec3uimap;
typedef std::map<nsuint,NSVec3<nssint>> sivec3uimap;
typedef std::map<nsuint,NSVec3<nsint>> ivec3uimap;
typedef std::map<nsuint,NSVec3<nslint>> livec3uimap;
typedef std::map<nsuint,NSVec3<nsllint>> llivec3uimap;
typedef std::map<nsuint,NSVec3<nsuchar>> ucvec3uimap;
typedef std::map<nsuint,NSVec3<nsusint>> usivec3uimap;
typedef std::map<nsuint,NSVec3<nsuint>> uivec3uimap;
typedef std::map<nsuint,NSVec3<nsulint>> ulivec3uimap;
typedef std::map<nsuint,NSVec3<nsullint>> ullivec3uimap;
typedef std::map<nsuint,NSVec3<nsfloat>> fvec3uimap;
typedef std::map<nsuint,NSVec3<nsdouble>> vec3uimap;
typedef std::map<nsuint,NSVec3<nsldouble>> lvec3uimap;

typedef std::map<nsuint,NSVec4<nschar>> cvec4uimap;
typedef std::map<nsuint,NSVec4<nschar16>> c16vec4uimap;
typedef std::map<nsuint,NSVec4<nschar32>> c32vec4uimap;
typedef std::map<nsuint,NSVec4<nswchar>> cwvec4uimap;
typedef std::map<nsuint,NSVec4<nssint>> sivec4uimap;
typedef std::map<nsuint,NSVec4<nsint>> ivec4uimap;
typedef std::map<nsuint,NSVec4<nslint>> livec4uimap;
typedef std::map<nsuint,NSVec4<nsllint>> llivec4uimap;
typedef std::map<nsuint,NSVec4<nsuchar>> ucvec4uimap;
typedef std::map<nsuint,NSVec4<nsusint>> usivec4uimap;
typedef std::map<nsuint,NSVec4<nsuint>> uivec4uimap;
typedef std::map<nsuint,NSVec4<nsulint>> ulivec4uimap;
typedef std::map<nsuint,NSVec4<nsullint>> ullivec4uimap;
typedef std::map<nsuint,NSVec4<nsfloat>> fvec4uimap;
typedef std::map<nsuint,NSVec4<nsdouble>> vec4uimap;
typedef std::map<nsuint,NSVec4<nsldouble>> lvec4uimap;

typedef std::map<nsuint,NSQuat<nschar>> cquatuimap;
typedef std::map<nsuint,NSQuat<nschar16>> c16quatuimap;
typedef std::map<nsuint,NSQuat<nschar32>> c32quatuimap;
typedef std::map<nsuint,NSQuat<nswchar>> cwquatuimap;
typedef std::map<nsuint,NSQuat<nssint>> siquatuimap;
typedef std::map<nsuint,NSQuat<nsint>> iquatuimap;
typedef std::map<nsuint,NSQuat<nslint>> liquatuimap;
typedef std::map<nsuint,NSQuat<nsllint>> lliquatuimap;
typedef std::map<nsuint,NSQuat<nsuchar>> ucquatuimap;
typedef std::map<nsuint,NSQuat<nsusint>> usiquatuimap;
typedef std::map<nsuint,NSQuat<nsuint>> uiquatuimap;
typedef std::map<nsuint,NSQuat<nsulint>> uliquatuimap;
typedef std::map<nsuint,NSQuat<nsullint>> ulliquatuimap;
typedef std::map<nsuint,NSQuat<nsfloat>> fquatuimap;
typedef std::map<nsuint,NSQuat<nsdouble>> quatuimap;
typedef std::map<nsuint,NSQuat<nsldouble>> lquatuimap;

typedef std::map<nsuint,NSMat2<nschar>> cmat2uimap;
typedef std::map<nsuint,NSMat2<nschar16>> c16mat2uimap;
typedef std::map<nsuint,NSMat2<nschar32>> c32mat2uimap;
typedef std::map<nsuint,NSMat2<nswchar>> cwmat2uimap;
typedef std::map<nsuint,NSMat2<nssint>> simat2uimap;
typedef std::map<nsuint,NSMat2<nsint>> imat2uimap;
typedef std::map<nsuint,NSMat2<nslint>> limat2uimap;
typedef std::map<nsuint,NSMat2<nsllint>> llimat2uimap;
typedef std::map<nsuint,NSMat2<nsuchar>> ucmat2uimap;
typedef std::map<nsuint,NSMat2<nsusint>> usimat2uimap;
typedef std::map<nsuint,NSMat2<nsuint>> uimat2uimap;
typedef std::map<nsuint,NSMat2<nsulint>> ulimat2uimap;
typedef std::map<nsuint,NSMat2<nsullint>> ullimat2uimap;
typedef std::map<nsuint,NSMat2<nsfloat>> fmat2uimap;
typedef std::map<nsuint,NSMat2<nsdouble>> mat2uimap;
typedef std::map<nsuint,NSMat2<nsldouble>> lmat2uimap;

typedef std::map<nsuint,NSMat3<nschar>> cmat3uimap;
typedef std::map<nsuint,NSMat3<nschar16>> c16mat3uimap;
typedef std::map<nsuint,NSMat3<nschar32>> c32mat3uimap;
typedef std::map<nsuint,NSMat3<nswchar>> cwmat3uimap;
typedef std::map<nsuint,NSMat3<nssint>> simat3uimap;
typedef std::map<nsuint,NSMat3<nsint>> imat3uimap;
typedef std::map<nsuint,NSMat3<nslint>> limat3uimap;
typedef std::map<nsuint,NSMat3<nsllint>> llimat3uimap;
typedef std::map<nsuint,NSMat3<nsuchar>> ucmat3uimap;
typedef std::map<nsuint,NSMat3<nsusint>> usimat3uimap;
typedef std::map<nsuint,NSMat3<nsuint>> uimat3uimap;
typedef std::map<nsuint,NSMat3<nsulint>> ulimat3uimap;
typedef std::map<nsuint,NSMat3<nsullint>> ullimat3uimap;
typedef std::map<nsuint,NSMat3<nsfloat>> fmat3uimap;
typedef std::map<nsuint,NSMat3<nsdouble>> mat3uimap;
typedef std::map<nsuint,NSMat3<nsldouble>> lmat3uimap;

typedef std::map<nsuint,NSMat4<nschar>> cmat4uimap;
typedef std::map<nsuint,NSMat4<nschar16>> c16mat4uimap;
typedef std::map<nsuint,NSMat4<nschar32>> c32mat4uimap;
typedef std::map<nsuint,NSMat4<nswchar>> cwmat4uimap;
typedef std::map<nsuint,NSMat4<nssint>> simat4uimap;
typedef std::map<nsuint,NSMat4<nsint>> imat4uimap;
typedef std::map<nsuint,NSMat4<nslint>> limat4uimap;
typedef std::map<nsuint,NSMat4<nsllint>> llimat4uimap;
typedef std::map<nsuint,NSMat4<nsuchar>> ucmat4uimap;
typedef std::map<nsuint,NSMat4<nsusint>> usimat4uimap;
typedef std::map<nsuint,NSMat4<nsuint>> uimat4uimap;
typedef std::map<nsuint,NSMat4<nsulint>> ulimat4uimap;
typedef std::map<nsuint,NSMat4<nsullint>> ullimat4uimap;
typedef std::map<nsuint,NSMat4<nsfloat>> fmat4uimap;
typedef std::map<nsuint,NSMat4<nsdouble>> mat4uimap;
typedef std::map<nsuint,NSMat4<nsldouble>> lmat4uimap;

typedef std::map<nsint, NSVec2<nschar>> cvec2imap;
typedef std::map<nsint, NSVec2<nschar16>> c16vec2imap;
typedef std::map<nsint, NSVec2<nschar32>> c32vec2imap;
typedef std::map<nsint, NSVec2<nswchar>> cwvec2imap;
typedef std::map<nsint, NSVec2<nssint>> sivec2imap;
typedef std::map<nsint, NSVec2<nsint>> ivec2imap;
typedef std::map<nsint, NSVec2<nslint>> livec2imap;
typedef std::map<nsint, NSVec2<nsllint>> llivec2imap;
typedef std::map<nsint, NSVec2<nsuchar>> ucvec2imap;
typedef std::map<nsint, NSVec2<nsusint>> usivec2imap;
typedef std::map<nsint, NSVec2<nsuint>> uivec2imap;
typedef std::map<nsint, NSVec2<nsulint>> ulivec2imap;
typedef std::map<nsint, NSVec2<nsullint>> ullivec2imap;
typedef std::map<nsint, NSVec2<nsfloat>> fvec2imap;
typedef std::map<nsint, NSVec2<nsdouble>> vec2imap;
typedef std::map<nsint, NSVec2<nsldouble>> lvec2imap;

typedef std::map<nsint, NSVec3<nschar>> cvec3imap;
typedef std::map<nsint, NSVec3<nschar16>> c16vec3imap;
typedef std::map<nsint, NSVec3<nschar32>> c32vec3imap;
typedef std::map<nsint, NSVec3<nswchar>> cwvec3imap;
typedef std::map<nsint, NSVec3<nssint>> sivec3imap;
typedef std::map<nsint, NSVec3<nsint>> ivec3imap;
typedef std::map<nsint, NSVec3<nslint>> livec3imap;
typedef std::map<nsint, NSVec3<nsllint>> llivec3imap;
typedef std::map<nsint, NSVec3<nsuchar>> ucvec3imap;
typedef std::map<nsint, NSVec3<nsusint>> usivec3imap;
typedef std::map<nsint, NSVec3<nsuint>> uivec3imap;
typedef std::map<nsint, NSVec3<nsulint>> ulivec3imap;
typedef std::map<nsint, NSVec3<nsullint>> ullivec3imap;
typedef std::map<nsint, NSVec3<nsfloat>> fvec3imap;
typedef std::map<nsint, NSVec3<nsdouble>> vec3imap;
typedef std::map<nsint, NSVec3<nsldouble>> lvec3imap;

typedef std::map<nsint, NSVec4<nschar>> cvec4imap;
typedef std::map<nsint, NSVec4<nschar16>> c16vec4imap;
typedef std::map<nsint, NSVec4<nschar32>> c32vec4imap;
typedef std::map<nsint, NSVec4<nswchar>> cwvec4imap;
typedef std::map<nsint, NSVec4<nssint>> sivec4imap;
typedef std::map<nsint, NSVec4<nsint>> ivec4imap;
typedef std::map<nsint, NSVec4<nslint>> livec4imap;
typedef std::map<nsint, NSVec4<nsllint>> llivec4imap;
typedef std::map<nsint, NSVec4<nsuchar>> ucvec4imap;
typedef std::map<nsint, NSVec4<nsusint>> usivec4imap;
typedef std::map<nsint, NSVec4<nsuint>> uivec4imap;
typedef std::map<nsint, NSVec4<nsulint>> ulivec4imap;
typedef std::map<nsint, NSVec4<nsullint>> ullivec4imap;
typedef std::map<nsint, NSVec4<nsfloat>> fvec4imap;
typedef std::map<nsint, NSVec4<nsdouble>> vec4imap;
typedef std::map<nsint, NSVec4<nsldouble>> lvec4imap;

typedef std::map<nsint, NSQuat<nschar>> cquatimap;
typedef std::map<nsint, NSQuat<nschar16>> c16quatimap;
typedef std::map<nsint, NSQuat<nschar32>> c32quatimap;
typedef std::map<nsint, NSQuat<nswchar>> cwquatimap;
typedef std::map<nsint, NSQuat<nssint>> siquatimap;
typedef std::map<nsint, NSQuat<nsint>> iquatimap;
typedef std::map<nsint, NSQuat<nslint>> liquatimap;
typedef std::map<nsint, NSQuat<nsllint>> lliquatimap;
typedef std::map<nsint, NSQuat<nsuchar>> ucquatimap;
typedef std::map<nsint, NSQuat<nsusint>> usiquatimap;
typedef std::map<nsint, NSQuat<nsuint>> uiquatimap;
typedef std::map<nsint, NSQuat<nsulint>> uliquatimap;
typedef std::map<nsint, NSQuat<nsullint>> ulliquatimap;
typedef std::map<nsint, NSQuat<nsfloat>> fquatimap;
typedef std::map<nsint, NSQuat<nsdouble>> quatimap;
typedef std::map<nsint, NSQuat<nsldouble>> lquatimap;

typedef std::map<nsint, NSMat2<nschar>> cmat2imap;
typedef std::map<nsint, NSMat2<nschar16>> c16mat2imap;
typedef std::map<nsint, NSMat2<nschar32>> c32mat2imap;
typedef std::map<nsint, NSMat2<nswchar>> cwmat2imap;
typedef std::map<nsint, NSMat2<nssint>> simat2imap;
typedef std::map<nsint, NSMat2<nsint>> imat2imap;
typedef std::map<nsint, NSMat2<nslint>> limat2imap;
typedef std::map<nsint, NSMat2<nsllint>> llimat2imap;
typedef std::map<nsint, NSMat2<nsuchar>> ucmat2imap;
typedef std::map<nsint, NSMat2<nsusint>> usimat2imap;
typedef std::map<nsint, NSMat2<nsuint>> uimat2imap;
typedef std::map<nsint, NSMat2<nsulint>> ulimat2imap;
typedef std::map<nsint, NSMat2<nsullint>> ullimat2imap;
typedef std::map<nsint, NSMat2<nsfloat>> fmat2imap;
typedef std::map<nsint, NSMat2<nsdouble>> mat2imap;
typedef std::map<nsint, NSMat2<nsldouble>> lmat2imap;

typedef std::map<nsint, NSMat3<nschar>> cmat3imap;
typedef std::map<nsint, NSMat3<nschar16>> c16mat3imap;
typedef std::map<nsint, NSMat3<nschar32>> c32mat3imap;
typedef std::map<nsint, NSMat3<nswchar>> cwmat3imap;
typedef std::map<nsint, NSMat3<nssint>> simat3imap;
typedef std::map<nsint, NSMat3<nsint>> imat3imap;
typedef std::map<nsint, NSMat3<nslint>> limat3imap;
typedef std::map<nsint, NSMat3<nsllint>> llimat3imap;
typedef std::map<nsint, NSMat3<nsuchar>> ucmat3imap;
typedef std::map<nsint, NSMat3<nsusint>> usimat3imap;
typedef std::map<nsint, NSMat3<nsuint>> uimat3imap;
typedef std::map<nsint, NSMat3<nsulint>> ulimat3imap;
typedef std::map<nsint, NSMat3<nsullint>> ullimat3imap;
typedef std::map<nsint, NSMat3<nsfloat>> fmat3imap;
typedef std::map<nsint, NSMat3<nsdouble>> mat3imap;
typedef std::map<nsint, NSMat3<nsldouble>> lmat3imap;

typedef std::map<nsint, NSMat4<nschar>> cmat4imap;
typedef std::map<nsint, NSMat4<nschar16>> c16mat4imap;
typedef std::map<nsint, NSMat4<nschar32>> c32mat4imap;
typedef std::map<nsint, NSMat4<nswchar>> cwmat4imap;
typedef std::map<nsint, NSMat4<nssint>> simat4imap;
typedef std::map<nsint, NSMat4<nsint>> imat4imap;
typedef std::map<nsint, NSMat4<nslint>> limat4imap;
typedef std::map<nsint, NSMat4<nsllint>> llimat4imap;
typedef std::map<nsint, NSMat4<nsuchar>> ucmat4imap;
typedef std::map<nsint, NSMat4<nsusint>> usimat4imap;
typedef std::map<nsint, NSMat4<nsuint>> uimat4imap;
typedef std::map<nsint, NSMat4<nsulint>> ulimat4imap;
typedef std::map<nsint, NSMat4<nsullint>> ullimat4imap;
typedef std::map<nsint, NSMat4<nsfloat>> fmat4imap;
typedef std::map<nsint, NSMat4<nsdouble>> mat4imap;
typedef std::map<nsint, NSMat4<nsldouble>> lmat4imap;

typedef std::map<nsfloat, NSVec2<nschar>> cvec2fmap;
typedef std::map<nsfloat, NSVec2<nschar16>> c16vec2fmap;
typedef std::map<nsfloat, NSVec2<nschar32>> c32vec2fmap;
typedef std::map<nsfloat, NSVec2<nswchar>> cwvec2fmap;
typedef std::map<nsfloat, NSVec2<nssint>> sivec2fmap;
typedef std::map<nsfloat, NSVec2<nsint>> ivec2fmap;
typedef std::map<nsfloat, NSVec2<nslint>> livec2fmap;
typedef std::map<nsfloat, NSVec2<nsllint>> llivec2fmap;
typedef std::map<nsfloat, NSVec2<nsuchar>> ucvec2fmap;
typedef std::map<nsfloat, NSVec2<nsusint>> usivec2fmap;
typedef std::map<nsfloat, NSVec2<nsuint>> uivec2fmap;
typedef std::map<nsfloat, NSVec2<nsulint>> ulivec2fmap;
typedef std::map<nsfloat, NSVec2<nsullint>> ullivec2fmap;
typedef std::map<nsfloat, NSVec2<nsfloat>> fvec2fmap;
typedef std::map<nsfloat, NSVec2<nsdouble>> vec2fmap;
typedef std::map<nsfloat, NSVec2<nsldouble>> lvec2fmap;

typedef std::map<nsfloat, NSVec3<nschar>> cvec3fmap;
typedef std::map<nsfloat, NSVec3<nschar16>> c16vec3fmap;
typedef std::map<nsfloat, NSVec3<nschar32>> c32vec3fmap;
typedef std::map<nsfloat, NSVec3<nswchar>> cwvec3fmap;
typedef std::map<nsfloat, NSVec3<nssint>> sivec3fmap;
typedef std::map<nsfloat, NSVec3<nsint>> ivec3fmap;
typedef std::map<nsfloat, NSVec3<nslint>> livec3fmap;
typedef std::map<nsfloat, NSVec3<nsllint>> llivec3fmap;
typedef std::map<nsfloat, NSVec3<nsuchar>> ucvec3fmap;
typedef std::map<nsfloat, NSVec3<nsusint>> usivec3fmap;
typedef std::map<nsfloat, NSVec3<nsuint>> uivec3fmap;
typedef std::map<nsfloat, NSVec3<nsulint>> ulivec3fmap;
typedef std::map<nsfloat, NSVec3<nsullint>> ullivec3fmap;
typedef std::map<nsfloat, NSVec3<nsfloat>> fvec3fmap;
typedef std::map<nsfloat, NSVec3<nsdouble>> vec3fmap;
typedef std::map<nsfloat, NSVec3<nsldouble>> lvec3fmap;

typedef std::map<nsfloat, NSVec4<nschar>> cvec4fmap;
typedef std::map<nsfloat, NSVec4<nschar16>> c16vec4fmap;
typedef std::map<nsfloat, NSVec4<nschar32>> c32vec4fmap;
typedef std::map<nsfloat, NSVec4<nswchar>> cwvec4fmap;
typedef std::map<nsfloat, NSVec4<nssint>> sivec4fmap;
typedef std::map<nsfloat, NSVec4<nsint>> ivec4fmap;
typedef std::map<nsfloat, NSVec4<nslint>> livec4fmap;
typedef std::map<nsfloat, NSVec4<nsllint>> llivec4fmap;
typedef std::map<nsfloat, NSVec4<nsuchar>> ucvec4fmap;
typedef std::map<nsfloat, NSVec4<nsusint>> usivec4fmap;
typedef std::map<nsfloat, NSVec4<nsuint>> uivec4fmap;
typedef std::map<nsfloat, NSVec4<nsulint>> ulivec4fmap;
typedef std::map<nsfloat, NSVec4<nsullint>> ullivec4fmap;
typedef std::map<nsfloat, NSVec4<nsfloat>> fvec4fmap;
typedef std::map<nsfloat, NSVec4<nsdouble>> vec4fmap;
typedef std::map<nsfloat, NSVec4<nsldouble>> lvec4fmap;

typedef std::map<nsfloat, NSQuat<nschar>> cquatfmap;
typedef std::map<nsfloat, NSQuat<nschar16>> c16quatfmap;
typedef std::map<nsfloat, NSQuat<nschar32>> c32quatfmap;
typedef std::map<nsfloat, NSQuat<nswchar>> cwquatfmap;
typedef std::map<nsfloat, NSQuat<nssint>> siquatfmap;
typedef std::map<nsfloat, NSQuat<nsint>> iquatfmap;
typedef std::map<nsfloat, NSQuat<nslint>> liquatfmap;
typedef std::map<nsfloat, NSQuat<nsllint>> lliquatfmap;
typedef std::map<nsfloat, NSQuat<nsuchar>> ucquatfmap;
typedef std::map<nsfloat, NSQuat<nsusint>> usiquatfmap;
typedef std::map<nsfloat, NSQuat<nsuint>> uiquatfmap;
typedef std::map<nsfloat, NSQuat<nsulint>> uliquatfmap;
typedef std::map<nsfloat, NSQuat<nsullint>> ulliquatfmap;
typedef std::map<nsfloat, NSQuat<nsfloat>> fquatfmap;
typedef std::map<nsfloat, NSQuat<nsdouble>> quatfmap;
typedef std::map<nsfloat, NSQuat<nsldouble>> lquatfmap;

typedef std::map<nsfloat, NSMat2<nschar>> cmat2fmap;
typedef std::map<nsfloat, NSMat2<nschar16>> c16mat2fmap;
typedef std::map<nsfloat, NSMat2<nschar32>> c32mat2fmap;
typedef std::map<nsfloat, NSMat2<nswchar>> cwmat2fmap;
typedef std::map<nsfloat, NSMat2<nssint>> simat2fmap;
typedef std::map<nsfloat, NSMat2<nsint>> imat2fmap;
typedef std::map<nsfloat, NSMat2<nslint>> limat2fmap;
typedef std::map<nsfloat, NSMat2<nsllint>> llimat2fmap;
typedef std::map<nsfloat, NSMat2<nsuchar>> ucmat2fmap;
typedef std::map<nsfloat, NSMat2<nsusint>> usimat2fmap;
typedef std::map<nsfloat, NSMat2<nsuint>> uimat2fmap;
typedef std::map<nsfloat, NSMat2<nsulint>> ulimat2fmap;
typedef std::map<nsfloat, NSMat2<nsullint>> ullimat2fmap;
typedef std::map<nsfloat, NSMat2<nsfloat>> fmat2fmap;
typedef std::map<nsfloat, NSMat2<nsdouble>> mat2fmap;
typedef std::map<nsfloat, NSMat2<nsldouble>> lmat2fmap;

typedef std::map<nsfloat, NSMat3<nschar>> cmat3fmap;
typedef std::map<nsfloat, NSMat3<nschar16>> c16mat3fmap;
typedef std::map<nsfloat, NSMat3<nschar32>> c32mat3fmap;
typedef std::map<nsfloat, NSMat3<nswchar>> cwmat3fmap;
typedef std::map<nsfloat, NSMat3<nssint>> simat3fmap;
typedef std::map<nsfloat, NSMat3<nsint>> imat3fmap;
typedef std::map<nsfloat, NSMat3<nslint>> limat3fmap;
typedef std::map<nsfloat, NSMat3<nsllint>> llimat3fmap;
typedef std::map<nsfloat, NSMat3<nsuchar>> ucmat3fmap;
typedef std::map<nsfloat, NSMat3<nsusint>> usimat3fmap;
typedef std::map<nsfloat, NSMat3<nsuint>> uimat3fmap;
typedef std::map<nsfloat, NSMat3<nsulint>> ulimat3fmap;
typedef std::map<nsfloat, NSMat3<nsullint>> ullimat3fmap;
typedef std::map<nsfloat, NSMat3<nsfloat>> fmat3fmap;
typedef std::map<nsfloat, NSMat3<nsdouble>> mat3fmap;
typedef std::map<nsfloat, NSMat3<nsldouble>> lmat3fmap;

typedef std::map<nsfloat, NSMat4<nschar>> cmat4fmap;
typedef std::map<nsfloat, NSMat4<nschar16>> c16mat4fmap;
typedef std::map<nsfloat, NSMat4<nschar32>> c32mat4fmap;
typedef std::map<nsfloat, NSMat4<nswchar>> cwmat4fmap;
typedef std::map<nsfloat, NSMat4<nssint>> simat4fmap;
typedef std::map<nsfloat, NSMat4<nsint>> imat4fmap;
typedef std::map<nsfloat, NSMat4<nslint>> limat4fmap;
typedef std::map<nsfloat, NSMat4<nsllint>> llimat4fmap;
typedef std::map<nsfloat, NSMat4<nsuchar>> ucmat4fmap;
typedef std::map<nsfloat, NSMat4<nsusint>> usimat4fmap;
typedef std::map<nsfloat, NSMat4<nsuint>> uimat4fmap;
typedef std::map<nsfloat, NSMat4<nsulint>> ulimat4fmap;
typedef std::map<nsfloat, NSMat4<nsullint>> ullimat4fmap;
typedef std::map<nsfloat, NSMat4<nsfloat>> fmat4fmap;
typedef std::map<nsfloat, NSMat4<nsdouble>> mat4fmap;
typedef std::map<nsfloat, NSMat4<nsldouble>> lmat4fmap;

typedef std::map<nsdouble, NSVec2<nschar>> cvec2dmap;
typedef std::map<nsdouble, NSVec2<nschar16>> c16vec2dmap;
typedef std::map<nsdouble, NSVec2<nschar32>> c32vec2dmap;
typedef std::map<nsdouble, NSVec2<nswchar>> cwvec2dmap;
typedef std::map<nsdouble, NSVec2<nssint>> sivec2dmap;
typedef std::map<nsdouble, NSVec2<nsint>> ivec2dmap;
typedef std::map<nsdouble, NSVec2<nslint>> livec2dmap;
typedef std::map<nsdouble, NSVec2<nsllint>> llivec2dmap;
typedef std::map<nsdouble, NSVec2<nsuchar>> ucvec2dmap;
typedef std::map<nsdouble, NSVec2<nsusint>> usivec2dmap;
typedef std::map<nsdouble, NSVec2<nsuint>> uivec2dmap;
typedef std::map<nsdouble, NSVec2<nsulint>> ulivec2dmap;
typedef std::map<nsdouble, NSVec2<nsullint>> ullivec2dmap;
typedef std::map<nsdouble, NSVec2<nsfloat>> fvec2dmap;
typedef std::map<nsdouble, NSVec2<nsdouble>> vec2dmap;
typedef std::map<nsdouble, NSVec2<nsldouble>> lvec2dmap;

typedef std::map<nsdouble, NSVec3<nschar>> cvec3dmap;
typedef std::map<nsdouble, NSVec3<nschar16>> c16vec3dmap;
typedef std::map<nsdouble, NSVec3<nschar32>> c32vec3dmap;
typedef std::map<nsdouble, NSVec3<nswchar>> cwvec3dmap;
typedef std::map<nsdouble, NSVec3<nssint>> sivec3dmap;
typedef std::map<nsdouble, NSVec3<nsint>> ivec3dmap;
typedef std::map<nsdouble, NSVec3<nslint>> livec3dmap;
typedef std::map<nsdouble, NSVec3<nsllint>> llivec3dmap;
typedef std::map<nsdouble, NSVec3<nsuchar>> ucvec3dmap;
typedef std::map<nsdouble, NSVec3<nsusint>> usivec3dmap;
typedef std::map<nsdouble, NSVec3<nsuint>> uivec3dmap;
typedef std::map<nsdouble, NSVec3<nsulint>> ulivec3dmap;
typedef std::map<nsdouble, NSVec3<nsullint>> ullivec3dmap;
typedef std::map<nsdouble, NSVec3<nsfloat>> fvec3dmap;
typedef std::map<nsdouble, NSVec3<nsdouble>> vec3dmap;
typedef std::map<nsdouble, NSVec3<nsldouble>> lvec3dmap;

typedef std::map<nsdouble, NSVec4<nschar>> cvec4dmap;
typedef std::map<nsdouble, NSVec4<nschar16>> c16vec4dmap;
typedef std::map<nsdouble, NSVec4<nschar32>> c32vec4dmap;
typedef std::map<nsdouble, NSVec4<nswchar>> cwvec4dmap;
typedef std::map<nsdouble, NSVec4<nssint>> sivec4dmap;
typedef std::map<nsdouble, NSVec4<nsint>> ivec4dmap;
typedef std::map<nsdouble, NSVec4<nslint>> livec4dmap;
typedef std::map<nsdouble, NSVec4<nsllint>> llivec4dmap;
typedef std::map<nsdouble, NSVec4<nsuchar>> ucvec4dmap;
typedef std::map<nsdouble, NSVec4<nsusint>> usivec4dmap;
typedef std::map<nsdouble, NSVec4<nsuint>> uivec4dmap;
typedef std::map<nsdouble, NSVec4<nsulint>> ulivec4dmap;
typedef std::map<nsdouble, NSVec4<nsullint>> ullivec4dmap;
typedef std::map<nsdouble, NSVec4<nsfloat>> fvec4dmap;
typedef std::map<nsdouble, NSVec4<nsdouble>> vec4dmap;
typedef std::map<nsdouble, NSVec4<nsldouble>> lvec4dmap;

typedef std::map<nsdouble, NSQuat<nschar>> cquatdmap;
typedef std::map<nsdouble, NSQuat<nschar16>> c16quatdmap;
typedef std::map<nsdouble, NSQuat<nschar32>> c32quatdmap;
typedef std::map<nsdouble, NSQuat<nswchar>> cwquatdmap;
typedef std::map<nsdouble, NSQuat<nssint>> siquatdmap;
typedef std::map<nsdouble, NSQuat<nsint>> iquatdmap;
typedef std::map<nsdouble, NSQuat<nslint>> liquatdmap;
typedef std::map<nsdouble, NSQuat<nsllint>> lliquatdmap;
typedef std::map<nsdouble, NSQuat<nsuchar>> ucquatdmap;
typedef std::map<nsdouble, NSQuat<nsusint>> usiquatdmap;
typedef std::map<nsdouble, NSQuat<nsuint>> uiquatdmap;
typedef std::map<nsdouble, NSQuat<nsulint>> uliquatdmap;
typedef std::map<nsdouble, NSQuat<nsullint>> ulliquatdmap;
typedef std::map<nsdouble, NSQuat<nsfloat>> fquatdmap;
typedef std::map<nsdouble, NSQuat<nsdouble>> quatdmap;
typedef std::map<nsdouble, NSQuat<nsldouble>> lquatdmap;

typedef std::map<nsdouble, NSMat2<nschar>> cmat2dmap;
typedef std::map<nsdouble, NSMat2<nschar16>> c16mat2dmap;
typedef std::map<nsdouble, NSMat2<nschar32>> c32mat2dmap;
typedef std::map<nsdouble, NSMat2<nswchar>> cwmat2dmap;
typedef std::map<nsdouble, NSMat2<nssint>> simat2dmap;
typedef std::map<nsdouble, NSMat2<nsint>> imat2dmap;
typedef std::map<nsdouble, NSMat2<nslint>> limat2dmap;
typedef std::map<nsdouble, NSMat2<nsllint>> llimat2dmap;
typedef std::map<nsdouble, NSMat2<nsuchar>> ucmat2dmap;
typedef std::map<nsdouble, NSMat2<nsusint>> usimat2dmap;
typedef std::map<nsdouble, NSMat2<nsuint>> uimat2dmap;
typedef std::map<nsdouble, NSMat2<nsulint>> ulimat2dmap;
typedef std::map<nsdouble, NSMat2<nsullint>> ullimat2dmap;
typedef std::map<nsdouble, NSMat2<nsfloat>> fmat2dmap;
typedef std::map<nsdouble, NSMat2<nsdouble>> mat2dmap;
typedef std::map<nsdouble, NSMat2<nsldouble>> lmat2dmap;

typedef std::map<nsdouble, NSMat4<nschar>> cmat4dmap;
typedef std::map<nsdouble, NSMat4<nschar16>> c16mat4dmap;
typedef std::map<nsdouble, NSMat4<nschar32>> c32mat4dmap;
typedef std::map<nsdouble, NSMat4<nswchar>> cwmat4dmap;
typedef std::map<nsdouble, NSMat4<nssint>> simat4dmap;
typedef std::map<nsdouble, NSMat4<nsint>> imat4dmap;
typedef std::map<nsdouble, NSMat4<nslint>> limat4dmap;
typedef std::map<nsdouble, NSMat4<nsllint>> llimat4dmap;
typedef std::map<nsdouble, NSMat4<nsuchar>> ucmat4dmap;
typedef std::map<nsdouble, NSMat4<nsusint>> usimat4dmap;
typedef std::map<nsdouble, NSMat4<nsuint>> uimat4dmap;
typedef std::map<nsdouble, NSMat4<nsulint>> ulimat4dmap;
typedef std::map<nsdouble, NSMat4<nsullint>> ullimat4dmap;
typedef std::map<nsdouble, NSMat4<nsfloat>> fmat4dmap;
typedef std::map<nsdouble, NSMat4<nsdouble>> mat4dmap;
typedef std::map<nsdouble, NSMat4<nsldouble>> lmat4dmap;

typedef std::unordered_map<nsuint, NSVec2<nschar>> cvec2uiu_map;
typedef std::unordered_map<nsuint, NSVec2<nschar16>> c16vec2uiu_map;
typedef std::unordered_map<nsuint, NSVec2<nschar32>> c32vec2uiu_map;
typedef std::unordered_map<nsuint, NSVec2<nswchar>> cwvec2uiu_map;
typedef std::unordered_map<nsuint, NSVec2<nssint>> sivec2uiu_map;
typedef std::unordered_map<nsuint, NSVec2<nsint>> ivec2uiu_map;
typedef std::unordered_map<nsuint, NSVec2<nslint>> livec2uiu_map;
typedef std::unordered_map<nsuint, NSVec2<nsllint>> llivec2uiu_map;
typedef std::unordered_map<nsuint, NSVec2<nsuchar>> ucvec2uiu_map;
typedef std::unordered_map<nsuint, NSVec2<nsusint>> usivec2uiu_map;
typedef std::unordered_map<nsuint, NSVec2<nsuint>> uivec2uiu_map;
typedef std::unordered_map<nsuint, NSVec2<nsulint>> ulivec2uiu_map;
typedef std::unordered_map<nsuint, NSVec2<nsullint>> ullivec2uiu_map;
typedef std::unordered_map<nsuint, NSVec2<nsfloat>> fvec2uiu_map;
typedef std::unordered_map<nsuint, NSVec2<nsdouble>> vec2uiu_map;
typedef std::unordered_map<nsuint, NSVec2<nsldouble>> lvec2uiu_map;

typedef std::unordered_map<nsuint, NSVec3<nschar>> cvec3uiu_map;
typedef std::unordered_map<nsuint, NSVec3<nschar16>> c16vec3uiu_map;
typedef std::unordered_map<nsuint, NSVec3<nschar32>> c32vec3uiu_map;
typedef std::unordered_map<nsuint, NSVec3<nswchar>> cwvec3uiu_map;
typedef std::unordered_map<nsuint, NSVec3<nssint>> sivec3uiu_map;
typedef std::unordered_map<nsuint, NSVec3<nsint>> ivec3uiu_map;
typedef std::unordered_map<nsuint, NSVec3<nslint>> livec3uiu_map;
typedef std::unordered_map<nsuint, NSVec3<nsllint>> llivec3uiu_map;
typedef std::unordered_map<nsuint, NSVec3<nsuchar>> ucvec3uiu_map;
typedef std::unordered_map<nsuint, NSVec3<nsusint>> usivec3uiu_map;
typedef std::unordered_map<nsuint, NSVec3<nsuint>> uivec3uiu_map;
typedef std::unordered_map<nsuint, NSVec3<nsulint>> ulivec3uiu_map;
typedef std::unordered_map<nsuint, NSVec3<nsullint>> ullivec3uiu_map;
typedef std::unordered_map<nsuint, NSVec3<nsfloat>> fvec3uiu_map;
typedef std::unordered_map<nsuint, NSVec3<nsdouble>> vec3uiu_map;
typedef std::unordered_map<nsuint, NSVec3<nsldouble>> lvec3uiu_map;

typedef std::unordered_map<nsuint, NSVec4<nschar>> cvec4uiu_map;
typedef std::unordered_map<nsuint, NSVec4<nschar16>> c16vec4uiu_map;
typedef std::unordered_map<nsuint, NSVec4<nschar32>> c32vec4uiu_map;
typedef std::unordered_map<nsuint, NSVec4<nswchar>> cwvec4uiu_map;
typedef std::unordered_map<nsuint, NSVec4<nssint>> sivec4uiu_map;
typedef std::unordered_map<nsuint, NSVec4<nsint>> ivec4uiu_map;
typedef std::unordered_map<nsuint, NSVec4<nslint>> livec4uiu_map;
typedef std::unordered_map<nsuint, NSVec4<nsllint>> llivec4uiu_map;
typedef std::unordered_map<nsuint, NSVec4<nsuchar>> ucvec4uiu_map;
typedef std::unordered_map<nsuint, NSVec4<nsusint>> usivec4uiu_map;
typedef std::unordered_map<nsuint, NSVec4<nsuint>> uivec4uiu_map;
typedef std::unordered_map<nsuint, NSVec4<nsulint>> ulivec4uiu_map;
typedef std::unordered_map<nsuint, NSVec4<nsullint>> ullivec4uiu_map;
typedef std::unordered_map<nsuint, NSVec4<nsfloat>> fvec4uiu_map;
typedef std::unordered_map<nsuint, NSVec4<nsdouble>> vec4uiu_map;
typedef std::unordered_map<nsuint, NSVec4<nsldouble>> lvec4uiu_map;

typedef std::unordered_map<nsuint, NSQuat<nschar>> cquatuiu_map;
typedef std::unordered_map<nsuint, NSQuat<nschar16>> c16quatuiu_map;
typedef std::unordered_map<nsuint, NSQuat<nschar32>> c32quatuiu_map;
typedef std::unordered_map<nsuint, NSQuat<nswchar>> cwquatuiu_map;
typedef std::unordered_map<nsuint, NSQuat<nssint>> siquatuiu_map;
typedef std::unordered_map<nsuint, NSQuat<nsint>> iquatuiu_map;
typedef std::unordered_map<nsuint, NSQuat<nslint>> liquatuiu_map;
typedef std::unordered_map<nsuint, NSQuat<nsllint>> lliquatuiu_map;
typedef std::unordered_map<nsuint, NSQuat<nsuchar>> ucquatuiu_map;
typedef std::unordered_map<nsuint, NSQuat<nsusint>> usiquatuiu_map;
typedef std::unordered_map<nsuint, NSQuat<nsuint>> uiquatuiu_map;
typedef std::unordered_map<nsuint, NSQuat<nsulint>> uliquatuiu_map;
typedef std::unordered_map<nsuint, NSQuat<nsullint>> ulliquatuiu_map;
typedef std::unordered_map<nsuint, NSQuat<nsfloat>> fquatuiu_map;
typedef std::unordered_map<nsuint, NSQuat<nsdouble>> quatuiu_map;
typedef std::unordered_map<nsuint, NSQuat<nsldouble>> lquatuiu_map;

typedef std::unordered_map<nsuint, NSMat2<nschar>> cmat2uiu_map;
typedef std::unordered_map<nsuint, NSMat2<nschar16>> c16mat2uiu_map;
typedef std::unordered_map<nsuint, NSMat2<nschar32>> c32mat2uiu_map;
typedef std::unordered_map<nsuint, NSMat2<nswchar>> cwmat2uiu_map;
typedef std::unordered_map<nsuint, NSMat2<nssint>> simat2uiu_map;
typedef std::unordered_map<nsuint, NSMat2<nsint>> imat2uiu_map;
typedef std::unordered_map<nsuint, NSMat2<nslint>> limat2uiu_map;
typedef std::unordered_map<nsuint, NSMat2<nsllint>> llimat2uiu_map;
typedef std::unordered_map<nsuint, NSMat2<nsuchar>> ucmat2uiu_map;
typedef std::unordered_map<nsuint, NSMat2<nsusint>> usimat2uiu_map;
typedef std::unordered_map<nsuint, NSMat2<nsuint>> uimat2uiu_map;
typedef std::unordered_map<nsuint, NSMat2<nsulint>> ulimat2uiu_map;
typedef std::unordered_map<nsuint, NSMat2<nsullint>> ullimat2uiu_map;
typedef std::unordered_map<nsuint, NSMat2<nsfloat>> fmat2uiu_map;
typedef std::unordered_map<nsuint, NSMat2<nsdouble>> mat2uiu_map;
typedef std::unordered_map<nsuint, NSMat2<nsldouble>> lmat2uiu_map;

typedef std::unordered_map<nsuint, NSMat3<nschar>> cmat3uiu_map;
typedef std::unordered_map<nsuint, NSMat3<nschar16>> c16mat3uiu_map;
typedef std::unordered_map<nsuint, NSMat3<nschar32>> c32mat3uiu_map;
typedef std::unordered_map<nsuint, NSMat3<nswchar>> cwmat3uiu_map;
typedef std::unordered_map<nsuint, NSMat3<nssint>> simat3uiu_map;
typedef std::unordered_map<nsuint, NSMat3<nsint>> imat3uiu_map;
typedef std::unordered_map<nsuint, NSMat3<nslint>> limat3uiu_map;
typedef std::unordered_map<nsuint, NSMat3<nsllint>> llimat3uiu_map;
typedef std::unordered_map<nsuint, NSMat3<nsuchar>> ucmat3uiu_map;
typedef std::unordered_map<nsuint, NSMat3<nsusint>> usimat3uiu_map;
typedef std::unordered_map<nsuint, NSMat3<nsuint>> uimat3uiu_map;
typedef std::unordered_map<nsuint, NSMat3<nsulint>> ulimat3uiu_map;
typedef std::unordered_map<nsuint, NSMat3<nsullint>> ullimat3uiu_map;
typedef std::unordered_map<nsuint, NSMat3<nsfloat>> fmat3uiu_map;
typedef std::unordered_map<nsuint, NSMat3<nsdouble>> mat3uiu_map;
typedef std::unordered_map<nsuint, NSMat3<nsldouble>> lmat3uiu_map;

typedef std::unordered_map<nsuint, NSMat4<nschar>> cmat4uiu_map;
typedef std::unordered_map<nsuint, NSMat4<nschar16>> c16mat4uiu_map;
typedef std::unordered_map<nsuint, NSMat4<nschar32>> c32mat4uiu_map;
typedef std::unordered_map<nsuint, NSMat4<nswchar>> cwmat4uiu_map;
typedef std::unordered_map<nsuint, NSMat4<nssint>> simat4uiu_map;
typedef std::unordered_map<nsuint, NSMat4<nsint>> imat4uiu_map;
typedef std::unordered_map<nsuint, NSMat4<nslint>> limat4uiu_map;
typedef std::unordered_map<nsuint, NSMat4<nsllint>> llimat4uiu_map;
typedef std::unordered_map<nsuint, NSMat4<nsuchar>> ucmat4uiu_map;
typedef std::unordered_map<nsuint, NSMat4<nsusint>> usimat4uiu_map;
typedef std::unordered_map<nsuint, NSMat4<nsuint>> uimat4uiu_map;
typedef std::unordered_map<nsuint, NSMat4<nsulint>> ulimat4uiu_map;
typedef std::unordered_map<nsuint, NSMat4<nsullint>> ullimat4uiu_map;
typedef std::unordered_map<nsuint, NSMat4<nsfloat>> fmat4uiu_map;
typedef std::unordered_map<nsuint, NSMat4<nsdouble>> mat4uiu_map;
typedef std::unordered_map<nsuint, NSMat4<nsldouble>> lmat4uiu_map;

typedef std::unordered_map<nsint, NSVec2<nschar>> cvec2iu_map;
typedef std::unordered_map<nsint, NSVec2<nschar16>> c16vec2iu_map;
typedef std::unordered_map<nsint, NSVec2<nschar32>> c32vec2iu_map;
typedef std::unordered_map<nsint, NSVec2<nswchar>> cwvec2iu_map;
typedef std::unordered_map<nsint, NSVec2<nssint>> sivec2iu_map;
typedef std::unordered_map<nsint, NSVec2<nsint>> ivec2iu_map;
typedef std::unordered_map<nsint, NSVec2<nslint>> livec2iu_map;
typedef std::unordered_map<nsint, NSVec2<nsllint>> llivec2iu_map;
typedef std::unordered_map<nsint, NSVec2<nsuchar>> ucvec2iu_map;
typedef std::unordered_map<nsint, NSVec2<nsusint>> usivec2iu_map;
typedef std::unordered_map<nsint, NSVec2<nsuint>> uivec2iu_map;
typedef std::unordered_map<nsint, NSVec2<nsulint>> ulivec2iu_map;
typedef std::unordered_map<nsint, NSVec2<nsullint>> ullivec2iu_map;
typedef std::unordered_map<nsint, NSVec2<nsfloat>> fvec2iu_map;
typedef std::unordered_map<nsint, NSVec2<nsdouble>> vec2iu_map;
typedef std::unordered_map<nsint, NSVec2<nsldouble>> lvec2iu_map;

typedef std::unordered_map<nsint, NSVec3<nschar>> cvec3iu_map;
typedef std::unordered_map<nsint, NSVec3<nschar16>> c16vec3iu_map;
typedef std::unordered_map<nsint, NSVec3<nschar32>> c32vec3iu_map;
typedef std::unordered_map<nsint, NSVec3<nswchar>> cwvec3iu_map;
typedef std::unordered_map<nsint, NSVec3<nssint>> sivec3iu_map;
typedef std::unordered_map<nsint, NSVec3<nsint>> ivec3iu_map;
typedef std::unordered_map<nsint, NSVec3<nslint>> livec3iu_map;
typedef std::unordered_map<nsint, NSVec3<nsllint>> llivec3iu_map;
typedef std::unordered_map<nsint, NSVec3<nsuchar>> ucvec3iu_map;
typedef std::unordered_map<nsint, NSVec3<nsusint>> usivec3iu_map;
typedef std::unordered_map<nsint, NSVec3<nsuint>> uivec3iu_map;
typedef std::unordered_map<nsint, NSVec3<nsulint>> ulivec3iu_map;
typedef std::unordered_map<nsint, NSVec3<nsullint>> ullivec3iu_map;
typedef std::unordered_map<nsint, NSVec3<nsfloat>> fvec3iu_map;
typedef std::unordered_map<nsint, NSVec3<nsdouble>> vec3iu_map;
typedef std::unordered_map<nsint, NSVec3<nsldouble>> lvec3iu_map;

typedef std::unordered_map<nsint, NSVec4<nschar>> cvec4iu_map;
typedef std::unordered_map<nsint, NSVec4<nschar16>> c16vec4iu_map;
typedef std::unordered_map<nsint, NSVec4<nschar32>> c32vec4iu_map;
typedef std::unordered_map<nsint, NSVec4<nswchar>> cwvec4iu_map;
typedef std::unordered_map<nsint, NSVec4<nssint>> sivec4iu_map;
typedef std::unordered_map<nsint, NSVec4<nsint>> ivec4iu_map;
typedef std::unordered_map<nsint, NSVec4<nslint>> livec4iu_map;
typedef std::unordered_map<nsint, NSVec4<nsllint>> llivec4iu_map;
typedef std::unordered_map<nsint, NSVec4<nsuchar>> ucvec4iu_map;
typedef std::unordered_map<nsint, NSVec4<nsusint>> usivec4iu_map;
typedef std::unordered_map<nsint, NSVec4<nsuint>> uivec4iu_map;
typedef std::unordered_map<nsint, NSVec4<nsulint>> ulivec4iu_map;
typedef std::unordered_map<nsint, NSVec4<nsullint>> ullivec4iu_map;
typedef std::unordered_map<nsint, NSVec4<nsfloat>> fvec4iu_map;
typedef std::unordered_map<nsint, NSVec4<nsdouble>> vec4iu_map;
typedef std::unordered_map<nsint, NSVec4<nsldouble>> lvec4iu_map;

typedef std::unordered_map<nsint, NSQuat<nschar>> cquatiu_map;
typedef std::unordered_map<nsint, NSQuat<nschar16>> c16quatiu_map;
typedef std::unordered_map<nsint, NSQuat<nschar32>> c32quatiu_map;
typedef std::unordered_map<nsint, NSQuat<nswchar>> cwquatiu_map;
typedef std::unordered_map<nsint, NSQuat<nssint>> siquatiu_map;
typedef std::unordered_map<nsint, NSQuat<nsint>> iquatiu_map;
typedef std::unordered_map<nsint, NSQuat<nslint>> liquatiu_map;
typedef std::unordered_map<nsint, NSQuat<nsllint>> lliquatiu_map;
typedef std::unordered_map<nsint, NSQuat<nsuchar>> ucquatiu_map;
typedef std::unordered_map<nsint, NSQuat<nsusint>> usiquatiu_map;
typedef std::unordered_map<nsint, NSQuat<nsuint>> uiquatiu_map;
typedef std::unordered_map<nsint, NSQuat<nsulint>> uliquatiu_map;
typedef std::unordered_map<nsint, NSQuat<nsullint>> ulliquatiu_map;
typedef std::unordered_map<nsint, NSQuat<nsfloat>> fquatiu_map;
typedef std::unordered_map<nsint, NSQuat<nsdouble>> quatiu_map;
typedef std::unordered_map<nsint, NSQuat<nsldouble>> lquatiu_map;

typedef std::unordered_map<nsint, NSMat2<nschar>> cmat2iu_map;
typedef std::unordered_map<nsint, NSMat2<nschar16>> c16mat2iu_map;
typedef std::unordered_map<nsint, NSMat2<nschar32>> c32mat2iu_map;
typedef std::unordered_map<nsint, NSMat2<nswchar>> cwmat2iu_map;
typedef std::unordered_map<nsint, NSMat2<nssint>> simat2iu_map;
typedef std::unordered_map<nsint, NSMat2<nsint>> imat2iu_map;
typedef std::unordered_map<nsint, NSMat2<nslint>> limat2iu_map;
typedef std::unordered_map<nsint, NSMat2<nsllint>> llimat2iu_map;
typedef std::unordered_map<nsint, NSMat2<nsuchar>> ucmat2iu_map;
typedef std::unordered_map<nsint, NSMat2<nsusint>> usimat2iu_map;
typedef std::unordered_map<nsint, NSMat2<nsuint>> uimat2iu_map;
typedef std::unordered_map<nsint, NSMat2<nsulint>> ulimat2iu_map;
typedef std::unordered_map<nsint, NSMat2<nsullint>> ullimat2iu_map;
typedef std::unordered_map<nsint, NSMat2<nsfloat>> fmat2iu_map;
typedef std::unordered_map<nsint, NSMat2<nsdouble>> mat2iu_map;
typedef std::unordered_map<nsint, NSMat2<nsldouble>> lmat2iu_map;

typedef std::unordered_map<nsint, NSMat3<nschar>> cmat3iu_map;
typedef std::unordered_map<nsint, NSMat3<nschar16>> c16mat3iu_map;
typedef std::unordered_map<nsint, NSMat3<nschar32>> c32mat3iu_map;
typedef std::unordered_map<nsint, NSMat3<nswchar>> cwmat3iu_map;
typedef std::unordered_map<nsint, NSMat3<nssint>> simat3iu_map;
typedef std::unordered_map<nsint, NSMat3<nsint>> imat3iu_map;
typedef std::unordered_map<nsint, NSMat3<nslint>> limat3iu_map;
typedef std::unordered_map<nsint, NSMat3<nsllint>> llimat3iu_map;
typedef std::unordered_map<nsint, NSMat3<nsuchar>> ucmat3iu_map;
typedef std::unordered_map<nsint, NSMat3<nsusint>> usimat3iu_map;
typedef std::unordered_map<nsint, NSMat3<nsuint>> uimat3iu_map;
typedef std::unordered_map<nsint, NSMat3<nsulint>> ulimat3iu_map;
typedef std::unordered_map<nsint, NSMat3<nsullint>> ullimat3iu_map;
typedef std::unordered_map<nsint, NSMat3<nsfloat>> fmat3iu_map;
typedef std::unordered_map<nsint, NSMat3<nsdouble>> mat3iu_map;
typedef std::unordered_map<nsint, NSMat3<nsldouble>> lmat3iu_map;

typedef std::unordered_map<nsint, NSMat4<nschar>> cmat4iu_map;
typedef std::unordered_map<nsint, NSMat4<nschar16>> c16mat4iu_map;
typedef std::unordered_map<nsint, NSMat4<nschar32>> c32mat4iu_map;
typedef std::unordered_map<nsint, NSMat4<nswchar>> cwmat4iu_map;
typedef std::unordered_map<nsint, NSMat4<nssint>> simat4iu_map;
typedef std::unordered_map<nsint, NSMat4<nsint>> imat4iu_map;
typedef std::unordered_map<nsint, NSMat4<nslint>> limat4iu_map;
typedef std::unordered_map<nsint, NSMat4<nsllint>> llimat4iu_map;
typedef std::unordered_map<nsint, NSMat4<nsuchar>> ucmat4iu_map;
typedef std::unordered_map<nsint, NSMat4<nsusint>> usimat4iu_map;
typedef std::unordered_map<nsint, NSMat4<nsuint>> uimat4iu_map;
typedef std::unordered_map<nsint, NSMat4<nsulint>> ulimat4iu_map;
typedef std::unordered_map<nsint, NSMat4<nsullint>> ullimat4iu_map;
typedef std::unordered_map<nsint, NSMat4<nsfloat>> fmat4iu_map;
typedef std::unordered_map<nsint, NSMat4<nsdouble>> mat4iu_map;
typedef std::unordered_map<nsint, NSMat4<nsldouble>> lmat4iu_map;

typedef std::unordered_map<nsfloat, NSVec2<nschar>> cvec2fu_map;
typedef std::unordered_map<nsfloat, NSVec2<nschar16>> c16vec2fu_map;
typedef std::unordered_map<nsfloat, NSVec2<nschar32>> c32vec2fu_map;
typedef std::unordered_map<nsfloat, NSVec2<nswchar>> cwvec2fu_map;
typedef std::unordered_map<nsfloat, NSVec2<nssint>> sivec2fu_map;
typedef std::unordered_map<nsfloat, NSVec2<nsint>> ivec2fu_map;
typedef std::unordered_map<nsfloat, NSVec2<nslint>> livec2fu_map;
typedef std::unordered_map<nsfloat, NSVec2<nsllint>> llivec2fu_map;
typedef std::unordered_map<nsfloat, NSVec2<nsuchar>> ucvec2fu_map;
typedef std::unordered_map<nsfloat, NSVec2<nsusint>> usivec2fu_map;
typedef std::unordered_map<nsfloat, NSVec2<nsuint>> uivec2fu_map;
typedef std::unordered_map<nsfloat, NSVec2<nsulint>> ulivec2fu_map;
typedef std::unordered_map<nsfloat, NSVec2<nsullint>> ullivec2fu_map;
typedef std::unordered_map<nsfloat, NSVec2<nsfloat>> fvec2fu_map;
typedef std::unordered_map<nsfloat, NSVec2<nsdouble>> vec2fu_map;
typedef std::unordered_map<nsfloat, NSVec2<nsldouble>> lvec2fu_map;

typedef std::unordered_map<nsfloat, NSVec3<nschar>> cvec3fu_map;
typedef std::unordered_map<nsfloat, NSVec3<nschar16>> c16vec3fu_map;
typedef std::unordered_map<nsfloat, NSVec3<nschar32>> c32vec3fu_map;
typedef std::unordered_map<nsfloat, NSVec3<nswchar>> cwvec3fu_map;
typedef std::unordered_map<nsfloat, NSVec3<nssint>> sivec3fu_map;
typedef std::unordered_map<nsfloat, NSVec3<nsint>> ivec3fu_map;
typedef std::unordered_map<nsfloat, NSVec3<nslint>> livec3fu_map;
typedef std::unordered_map<nsfloat, NSVec3<nsllint>> llivec3fu_map;
typedef std::unordered_map<nsfloat, NSVec3<nsuchar>> ucvec3fu_map;
typedef std::unordered_map<nsfloat, NSVec3<nsusint>> usivec3fu_map;
typedef std::unordered_map<nsfloat, NSVec3<nsuint>> uivec3fu_map;
typedef std::unordered_map<nsfloat, NSVec3<nsulint>> ulivec3fu_map;
typedef std::unordered_map<nsfloat, NSVec3<nsullint>> ullivec3fu_map;
typedef std::unordered_map<nsfloat, NSVec3<nsfloat>> fvec3fu_map;
typedef std::unordered_map<nsfloat, NSVec3<nsdouble>> vec3fu_map;
typedef std::unordered_map<nsfloat, NSVec3<nsldouble>> lvec3fu_map;

typedef std::unordered_map<nsfloat, NSVec4<nschar>> cvec4fu_map;
typedef std::unordered_map<nsfloat, NSVec4<nschar16>> c16vec4fu_map;
typedef std::unordered_map<nsfloat, NSVec4<nschar32>> c32vec4fu_map;
typedef std::unordered_map<nsfloat, NSVec4<nswchar>> cwvec4fu_map;
typedef std::unordered_map<nsfloat, NSVec4<nssint>> sivec4fu_map;
typedef std::unordered_map<nsfloat, NSVec4<nsint>> ivec4fu_map;
typedef std::unordered_map<nsfloat, NSVec4<nslint>> livec4fu_map;
typedef std::unordered_map<nsfloat, NSVec4<nsllint>> llivec4fu_map;
typedef std::unordered_map<nsfloat, NSVec4<nsuchar>> ucvec4fu_map;
typedef std::unordered_map<nsfloat, NSVec4<nsusint>> usivec4fu_map;
typedef std::unordered_map<nsfloat, NSVec4<nsuint>> uivec4fu_map;
typedef std::unordered_map<nsfloat, NSVec4<nsulint>> ulivec4fu_map;
typedef std::unordered_map<nsfloat, NSVec4<nsullint>> ullivec4fu_map;
typedef std::unordered_map<nsfloat, NSVec4<nsfloat>> fvec4fu_map;
typedef std::unordered_map<nsfloat, NSVec4<nsdouble>> vec4fu_map;
typedef std::unordered_map<nsfloat, NSVec4<nsldouble>> lvec4fu_map;

typedef std::unordered_map<nsfloat, NSQuat<nschar>> cquatfu_map;
typedef std::unordered_map<nsfloat, NSQuat<nschar16>> c16quatfu_map;
typedef std::unordered_map<nsfloat, NSQuat<nschar32>> c32quatfu_map;
typedef std::unordered_map<nsfloat, NSQuat<nswchar>> cwquatfu_map;
typedef std::unordered_map<nsfloat, NSQuat<nssint>> siquatfu_map;
typedef std::unordered_map<nsfloat, NSQuat<nsint>> iquatfu_map;
typedef std::unordered_map<nsfloat, NSQuat<nslint>> liquatfu_map;
typedef std::unordered_map<nsfloat, NSQuat<nsllint>> lliquatfu_map;
typedef std::unordered_map<nsfloat, NSQuat<nsuchar>> ucquatfu_map;
typedef std::unordered_map<nsfloat, NSQuat<nsusint>> usiquatfu_map;
typedef std::unordered_map<nsfloat, NSQuat<nsuint>> uiquatfu_map;
typedef std::unordered_map<nsfloat, NSQuat<nsulint>> uliquatfu_map;
typedef std::unordered_map<nsfloat, NSQuat<nsullint>> ulliquatfu_map;
typedef std::unordered_map<nsfloat, NSQuat<nsfloat>> fquatfu_map;
typedef std::unordered_map<nsfloat, NSQuat<nsdouble>> quatfu_map;
typedef std::unordered_map<nsfloat, NSQuat<nsldouble>> lquatfu_map;

typedef std::unordered_map<nsfloat, NSMat2<nschar>> cmat2fu_map;
typedef std::unordered_map<nsfloat, NSMat2<nschar16>> c16mat2fu_map;
typedef std::unordered_map<nsfloat, NSMat2<nschar32>> c32mat2fu_map;
typedef std::unordered_map<nsfloat, NSMat2<nswchar>> cwmat2fu_map;
typedef std::unordered_map<nsfloat, NSMat2<nssint>> simat2fu_map;
typedef std::unordered_map<nsfloat, NSMat2<nsint>> imat2fu_map;
typedef std::unordered_map<nsfloat, NSMat2<nslint>> limat2fu_map;
typedef std::unordered_map<nsfloat, NSMat2<nsllint>> llimat2fu_map;
typedef std::unordered_map<nsfloat, NSMat2<nsuchar>> ucmat2fu_map;
typedef std::unordered_map<nsfloat, NSMat2<nsusint>> usimat2fu_map;
typedef std::unordered_map<nsfloat, NSMat2<nsuint>> uimat2fu_map;
typedef std::unordered_map<nsfloat, NSMat2<nsulint>> ulimat2fu_map;
typedef std::unordered_map<nsfloat, NSMat2<nsullint>> ullimat2fu_map;
typedef std::unordered_map<nsfloat, NSMat2<nsfloat>> fmat2fu_map;
typedef std::unordered_map<nsfloat, NSMat2<nsdouble>> mat2fu_map;
typedef std::unordered_map<nsfloat, NSMat2<nsldouble>> lmat2fu_map;

typedef std::unordered_map<nsfloat, NSMat3<nschar>> cmat3fu_map;
typedef std::unordered_map<nsfloat, NSMat3<nschar16>> c16mat3fu_map;
typedef std::unordered_map<nsfloat, NSMat3<nschar32>> c32mat3fu_map;
typedef std::unordered_map<nsfloat, NSMat3<nswchar>> cwmat3fu_map;
typedef std::unordered_map<nsfloat, NSMat3<nssint>> simat3fu_map;
typedef std::unordered_map<nsfloat, NSMat3<nsint>> imat3fu_map;
typedef std::unordered_map<nsfloat, NSMat3<nslint>> limat3fu_map;
typedef std::unordered_map<nsfloat, NSMat3<nsllint>> llimat3fu_map;
typedef std::unordered_map<nsfloat, NSMat3<nsuchar>> ucmat3fu_map;
typedef std::unordered_map<nsfloat, NSMat3<nsusint>> usimat3fu_map;
typedef std::unordered_map<nsfloat, NSMat3<nsuint>> uimat3fu_map;
typedef std::unordered_map<nsfloat, NSMat3<nsulint>> ulimat3fu_map;
typedef std::unordered_map<nsfloat, NSMat3<nsullint>> ullimat3fu_map;
typedef std::unordered_map<nsfloat, NSMat3<nsfloat>> fmat3fu_map;
typedef std::unordered_map<nsfloat, NSMat3<nsdouble>> mat3fu_map;
typedef std::unordered_map<nsfloat, NSMat3<nsldouble>> lmat3fu_map;

typedef std::unordered_map<nsfloat, NSMat4<nschar>> cmat4fu_map;
typedef std::unordered_map<nsfloat, NSMat4<nschar16>> c16mat4fu_map;
typedef std::unordered_map<nsfloat, NSMat4<nschar32>> c32mat4fu_map;
typedef std::unordered_map<nsfloat, NSMat4<nswchar>> cwmat4fu_map;
typedef std::unordered_map<nsfloat, NSMat4<nssint>> simat4fu_map;
typedef std::unordered_map<nsfloat, NSMat4<nsint>> imat4fu_map;
typedef std::unordered_map<nsfloat, NSMat4<nslint>> limat4fu_map;
typedef std::unordered_map<nsfloat, NSMat4<nsllint>> llimat4fu_map;
typedef std::unordered_map<nsfloat, NSMat4<nsuchar>> ucmat4fu_map;
typedef std::unordered_map<nsfloat, NSMat4<nsusint>> usimat4fu_map;
typedef std::unordered_map<nsfloat, NSMat4<nsuint>> uimat4fu_map;
typedef std::unordered_map<nsfloat, NSMat4<nsulint>> ulimat4fu_map;
typedef std::unordered_map<nsfloat, NSMat4<nsullint>> ullimat4fu_map;
typedef std::unordered_map<nsfloat, NSMat4<nsfloat>> fmat4fu_map;
typedef std::unordered_map<nsfloat, NSMat4<nsdouble>> mat4fu_map;
typedef std::unordered_map<nsfloat, NSMat4<nsldouble>> lmat4fu_map;

typedef std::unordered_map<nsdouble, NSVec2<nschar>> cvec2du_map;
typedef std::unordered_map<nsdouble, NSVec2<nschar16>> c16vec2du_map;
typedef std::unordered_map<nsdouble, NSVec2<nschar32>> c32vec2du_map;
typedef std::unordered_map<nsdouble, NSVec2<nswchar>> cwvec2du_map;
typedef std::unordered_map<nsdouble, NSVec2<nssint>> sivec2du_map;
typedef std::unordered_map<nsdouble, NSVec2<nsint>> ivec2du_map;
typedef std::unordered_map<nsdouble, NSVec2<nslint>> livec2du_map;
typedef std::unordered_map<nsdouble, NSVec2<nsllint>> llivec2du_map;
typedef std::unordered_map<nsdouble, NSVec2<nsuchar>> ucvec2du_map;
typedef std::unordered_map<nsdouble, NSVec2<nsusint>> usivec2du_map;
typedef std::unordered_map<nsdouble, NSVec2<nsuint>> uivec2du_map;
typedef std::unordered_map<nsdouble, NSVec2<nsulint>> ulivec2du_map;
typedef std::unordered_map<nsdouble, NSVec2<nsullint>> ullivec2du_map;
typedef std::unordered_map<nsdouble, NSVec2<nsfloat>> fvec2du_map;
typedef std::unordered_map<nsdouble, NSVec2<nsdouble>> vec2du_map;
typedef std::unordered_map<nsdouble, NSVec2<nsldouble>> lvec2du_map;

typedef std::unordered_map<nsdouble, NSVec3<nschar>> cvec3du_map;
typedef std::unordered_map<nsdouble, NSVec3<nschar16>> c16vec3du_map;
typedef std::unordered_map<nsdouble, NSVec3<nschar32>> c32vec3du_map;
typedef std::unordered_map<nsdouble, NSVec3<nswchar>> cwvec3du_map;
typedef std::unordered_map<nsdouble, NSVec3<nssint>> sivec3du_map;
typedef std::unordered_map<nsdouble, NSVec3<nsint>> ivec3du_map;
typedef std::unordered_map<nsdouble, NSVec3<nslint>> livec3du_map;
typedef std::unordered_map<nsdouble, NSVec3<nsllint>> llivec3du_map;
typedef std::unordered_map<nsdouble, NSVec3<nsuchar>> ucvec3du_map;
typedef std::unordered_map<nsdouble, NSVec3<nsusint>> usivec3du_map;
typedef std::unordered_map<nsdouble, NSVec3<nsuint>> uivec3du_map;
typedef std::unordered_map<nsdouble, NSVec3<nsulint>> ulivec3du_map;
typedef std::unordered_map<nsdouble, NSVec3<nsullint>> ullivec3du_map;
typedef std::unordered_map<nsdouble, NSVec3<nsfloat>> fvec3du_map;
typedef std::unordered_map<nsdouble, NSVec3<nsdouble>> vec3du_map;
typedef std::unordered_map<nsdouble, NSVec3<nsldouble>> lvec3du_map;

typedef std::unordered_map<nsdouble, NSVec4<nschar>> cvec4du_map;
typedef std::unordered_map<nsdouble, NSVec4<nschar16>> c16vec4du_map;
typedef std::unordered_map<nsdouble, NSVec4<nschar32>> c32vec4du_map;
typedef std::unordered_map<nsdouble, NSVec4<nswchar>> cwvec4du_map;
typedef std::unordered_map<nsdouble, NSVec4<nssint>> sivec4du_map;
typedef std::unordered_map<nsdouble, NSVec4<nsint>> ivec4du_map;
typedef std::unordered_map<nsdouble, NSVec4<nslint>> livec4du_map;
typedef std::unordered_map<nsdouble, NSVec4<nsllint>> llivec4du_map;
typedef std::unordered_map<nsdouble, NSVec4<nsuchar>> ucvec4du_map;
typedef std::unordered_map<nsdouble, NSVec4<nsusint>> usivec4du_map;
typedef std::unordered_map<nsdouble, NSVec4<nsuint>> uivec4du_map;
typedef std::unordered_map<nsdouble, NSVec4<nsulint>> ulivec4du_map;
typedef std::unordered_map<nsdouble, NSVec4<nsullint>> ullivec4du_map;
typedef std::unordered_map<nsdouble, NSVec4<nsfloat>> fvec4du_map;
typedef std::unordered_map<nsdouble, NSVec4<nsdouble>> vec4du_map;
typedef std::unordered_map<nsdouble, NSVec4<nsldouble>> lvec4du_map;

typedef std::unordered_map<nsdouble, NSQuat<nschar>> cquatdu_map;
typedef std::unordered_map<nsdouble, NSQuat<nschar16>> c16quatdu_map;
typedef std::unordered_map<nsdouble, NSQuat<nschar32>> c32quatdu_map;
typedef std::unordered_map<nsdouble, NSQuat<nswchar>> cwquatdu_map;
typedef std::unordered_map<nsdouble, NSQuat<nssint>> siquatdu_map;
typedef std::unordered_map<nsdouble, NSQuat<nsint>> iquatdu_map;
typedef std::unordered_map<nsdouble, NSQuat<nslint>> liquatdu_map;
typedef std::unordered_map<nsdouble, NSQuat<nsllint>> lliquatdu_map;
typedef std::unordered_map<nsdouble, NSQuat<nsuchar>> ucquatdu_map;
typedef std::unordered_map<nsdouble, NSQuat<nsusint>> usiquatdu_map;
typedef std::unordered_map<nsdouble, NSQuat<nsuint>> uiquatdu_map;
typedef std::unordered_map<nsdouble, NSQuat<nsulint>> uliquatdu_map;
typedef std::unordered_map<nsdouble, NSQuat<nsullint>> ulliquatdu_map;
typedef std::unordered_map<nsdouble, NSQuat<nsfloat>> fquatdu_map;
typedef std::unordered_map<nsdouble, NSQuat<nsdouble>> quatdu_map;
typedef std::unordered_map<nsdouble, NSQuat<nsldouble>> lquatdu_map;

typedef std::unordered_map<nsdouble, NSMat2<nschar>> cmat2du_map;
typedef std::unordered_map<nsdouble, NSMat2<nschar16>> c16mat2du_map;
typedef std::unordered_map<nsdouble, NSMat2<nschar32>> c32mat2du_map;
typedef std::unordered_map<nsdouble, NSMat2<nswchar>> cwmat2du_map;
typedef std::unordered_map<nsdouble, NSMat2<nssint>> simat2du_map;
typedef std::unordered_map<nsdouble, NSMat2<nsint>> imat2du_map;
typedef std::unordered_map<nsdouble, NSMat2<nslint>> limat2du_map;
typedef std::unordered_map<nsdouble, NSMat2<nsllint>> llimat2du_map;
typedef std::unordered_map<nsdouble, NSMat2<nsuchar>> ucmat2du_map;
typedef std::unordered_map<nsdouble, NSMat2<nsusint>> usimat2du_map;
typedef std::unordered_map<nsdouble, NSMat2<nsuint>> uimat2du_map;
typedef std::unordered_map<nsdouble, NSMat2<nsulint>> ulimat2du_map;
typedef std::unordered_map<nsdouble, NSMat2<nsullint>> ullimat2du_map;
typedef std::unordered_map<nsdouble, NSMat2<nsfloat>> fmat2du_map;
typedef std::unordered_map<nsdouble, NSMat2<nsdouble>> mat2du_map;
typedef std::unordered_map<nsdouble, NSMat2<nsldouble>> lmat2du_map;

typedef std::unordered_map<nsdouble, NSMat3<nschar>> cmat3du_map;
typedef std::unordered_map<nsdouble, NSMat3<nschar16>> c16mat3du_map;
typedef std::unordered_map<nsdouble, NSMat3<nschar32>> c32mat3du_map;
typedef std::unordered_map<nsdouble, NSMat3<nswchar>> cwmat3du_map;
typedef std::unordered_map<nsdouble, NSMat3<nssint>> simat3du_map;
typedef std::unordered_map<nsdouble, NSMat3<nsint>> imat3du_map;
typedef std::unordered_map<nsdouble, NSMat3<nslint>> limat3du_map;
typedef std::unordered_map<nsdouble, NSMat3<nsllint>> llimat3du_map;
typedef std::unordered_map<nsdouble, NSMat3<nsuchar>> ucmat3du_map;
typedef std::unordered_map<nsdouble, NSMat3<nsusint>> usimat3du_map;
typedef std::unordered_map<nsdouble, NSMat3<nsuint>> uimat3du_map;
typedef std::unordered_map<nsdouble, NSMat3<nsulint>> ulimat3du_map;
typedef std::unordered_map<nsdouble, NSMat3<nsullint>> ullimat3du_map;
typedef std::unordered_map<nsdouble, NSMat3<nsfloat>> fmat3du_map;
typedef std::unordered_map<nsdouble, NSMat3<nsdouble>> mat3du_map;
typedef std::unordered_map<nsdouble, NSMat3<nsldouble>> lmat3du_map;

typedef std::unordered_map<nsdouble, NSMat4<nschar>> cmat4du_map;
typedef std::unordered_map<nsdouble, NSMat4<nschar16>> c16mat4du_map;
typedef std::unordered_map<nsdouble, NSMat4<nschar32>> c32mat4du_map;
typedef std::unordered_map<nsdouble, NSMat4<nswchar>> cwmat4du_map;
typedef std::unordered_map<nsdouble, NSMat4<nssint>> simat4du_map;
typedef std::unordered_map<nsdouble, NSMat4<nsint>> imat4du_map;
typedef std::unordered_map<nsdouble, NSMat4<nslint>> limat4du_map;
typedef std::unordered_map<nsdouble, NSMat4<nsllint>> llimat4du_map;
typedef std::unordered_map<nsdouble, NSMat4<nsuchar>> ucmat4du_map;
typedef std::unordered_map<nsdouble, NSMat4<nsusint>> usimat4du_map;
typedef std::unordered_map<nsdouble, NSMat4<nsuint>> uimat4du_map;
typedef std::unordered_map<nsdouble, NSMat4<nsulint>> ulimat4du_map;
typedef std::unordered_map<nsdouble, NSMat4<nsullint>> ullimat4du_map;
typedef std::unordered_map<nsdouble, NSMat4<nsfloat>> fmat4du_map;
typedef std::unordered_map<nsdouble, NSMat4<nsdouble>> mat4du_map;
typedef std::unordered_map<nsdouble, NSMat4<nsldouble>> lmat4du_map;

// Math typedefs
typedef NSVec2<nschar> cvec2;
typedef NSVec2<nschar16> c16vec2;
typedef NSVec2<nschar32> c32vec2;
typedef NSVec2<nswchar> cwvec2;
typedef NSVec2<nssint> sivec2;
typedef NSVec2<nsint> ivec2;
typedef NSVec2<nslint> livec2;
typedef NSVec2<nsllint> liivec2;
typedef NSVec2<nsuchar> ucvec2;
typedef NSVec2<nsusint> usivec2;
typedef NSVec2<nsuint> uivec2;
typedef NSVec2<nsulint> ulivec2;
typedef NSVec2<nsullint> ullivec2;
typedef NSVec2<nsfloat> fvec2;
typedef NSVec2<nsdouble> vec2;
typedef NSVec2<nsldouble> lvec2;
typedef NSVec3<nschar> cvec3;
typedef NSVec3<nschar16> c16vec3;
typedef NSVec3<nschar32> c32vec3;
typedef NSVec3<nswchar> cwvec3;
typedef NSVec3<nssint> sivec3;
typedef NSVec3<nsint> ivec3;
typedef NSVec3<nslint> livec3;
typedef NSVec3<nsllint> liivec3;
typedef NSVec3<nsuchar> ucvec3;
typedef NSVec3<nsusint> usivec3;
typedef NSVec3<nsuint> uivec3;
typedef NSVec3<nsulint> ulivec3;
typedef NSVec3<nsullint> ullivec3;
typedef NSVec3<nsfloat> fvec3;
typedef NSVec3<nsdouble> vec3;
typedef NSVec3<nsldouble> lvec3;
typedef NSVec4<nschar> cvec4;
typedef NSVec4<nschar16> c16vec4;
typedef NSVec4<nschar32> c32vec4;
typedef NSVec4<nswchar> cwvec4;
typedef NSVec4<nssint> sivec4;
typedef NSVec4<nsint> ivec4;
typedef NSVec4<nslint> livec4;
typedef NSVec4<nsllint> liivec4;
typedef NSVec4<nsuchar> ucvec4;
typedef NSVec4<nsusint> usivec4;
typedef NSVec4<nsuint> uivec4;
typedef NSVec4<nsulint> ulivec4;
typedef NSVec4<nsullint> ullivec4;
typedef NSVec4<nsfloat> fvec4;
typedef NSVec4<nsdouble> vec4;
typedef NSVec4<nsldouble> lvec4;
typedef NSQuat<nschar> cquat;
typedef NSQuat<nschar16> c16quat;
typedef NSQuat<nschar32> c32quat;
typedef NSQuat<nswchar> cwquat;
typedef NSQuat<nssint> siquat;
typedef NSQuat<nsint> iquat;
typedef NSQuat<nslint> liquat;
typedef NSQuat<nsllint> liiquat;
typedef NSQuat<nsuchar> ucquat;
typedef NSQuat<nsusint> usiquat;
typedef NSQuat<nsuint> uiquat;
typedef NSQuat<nsulint> uliquat;
typedef NSQuat<nsullint> ulliquat;
typedef NSQuat<nsfloat> fquat;
typedef NSQuat<nsdouble> quat;
typedef NSQuat<nsldouble> lquat;
typedef NSMat2<nschar> cmat2;
typedef NSMat2<nschar16> c16mat2;
typedef NSMat2<nschar32> c32mat2;
typedef NSMat2<nswchar> cwmat2;
typedef NSMat2<nssint> simat2;
typedef NSMat2<nsint> imat2;
typedef NSMat2<nslint> limat2;
typedef NSMat2<nsllint> liimat2;
typedef NSMat2<nsuchar> ucmat2;
typedef NSMat2<nsusint> usimat2;
typedef NSMat2<nsuint> uimat2;
typedef NSMat2<nsulint> ulimat2;
typedef NSMat2<nsullint> ullimat2;
typedef NSMat2<nsfloat> fmat2;
typedef NSMat2<nsdouble> mat2;
typedef NSMat2<nsldouble> lmat2;
typedef NSMat3<nschar> cmat3;
typedef NSMat3<nschar16> c16mat3;
typedef NSMat3<nschar32> c32mat3;
typedef NSMat3<nswchar> cwmat3;
typedef NSMat3<nssint> simat3;
typedef NSMat3<nsint> imat3;
typedef NSMat3<nslint> limat3;
typedef NSMat3<nsllint> liimat3;
typedef NSMat3<nsuchar> ucmat3;
typedef NSMat3<nsusint> usimat3;
typedef NSMat3<nsuint> uimat3;
typedef NSMat3<nsulint> ulimat3;
typedef NSMat3<nsullint> ullimat3;
typedef NSMat3<nsfloat> fmat3;
typedef NSMat3<nsdouble> mat3;
typedef NSMat3<nsldouble> lmat3;
typedef NSMat4<nschar> cmat4;
typedef NSMat4<nschar16> c16mat4;
typedef NSMat4<nschar32> c32mat4;
typedef NSMat4<nswchar> cwmat4;
typedef NSMat4<nssint> simat4;
typedef NSMat4<nsint> imat4;
typedef NSMat4<nslint> limat4;
typedef NSMat4<nsllint> liimat4;
typedef NSMat4<nsuchar> ucmat4;
typedef NSMat4<nsusint> usimat4;
typedef NSMat4<nsuint> uimat4;
typedef NSMat4<nsulint> ulimat4;
typedef NSMat4<nsullint> ullimat4;
typedef NSMat4<nsfloat> fmat4;
typedef NSMat4<nsdouble> mat4;
typedef NSMat4<nsldouble> lmat4;

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

	nsfloat dx();
	nsfloat dy();
	nsfloat dz();

	void extend(
		const std::vector<fvec3> & pVertices,
		const fmat4 & pTransform = fmat4()
		);

	void set(
		const fvec3 & pMin,
		const fvec3 pMax
		);

	nsfloat volume();

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
	for (nsuint i = 0; i < 8; ++i)
		pup(p, box.mVerts[i], varName + "vert[" + std::to_string(i) + "]");
}

#endif
