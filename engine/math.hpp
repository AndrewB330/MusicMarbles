// Copied from github.com/AndrewB330/visualizer-cpp

#ifndef __MATH330__
#define __MATH330__
#include <cmath>
#include <algorithm>

const double PI = 3.1415926535897932384626;
const double DPI = 2 * PI;
const double HPI = PI * 0.5;

template<typename T>
struct Vector2 {
	T x, y;

	template<typename U>
    constexpr explicit Vector2(Vector2<U> v) :x((T)v.x), y((T)v.y) {}
	constexpr Vector2(T x, T y) :x(x), y(y) {}
    constexpr Vector2() :x(T()), y(T()) {}

	inline T len_sqr() {
		return x*x + y*y;
	}
	inline double len() {
		return sqrt(x*x + y*y);
	}
	inline Vector2<double> normal() {
		if (len_sqr() == 0) return *this;
		double inv = 1 / len();
		return Vector2(x*inv, y*inv);
	}
	inline Vector2<T> rot90() {
		return Vector2(-y, x);
	}
	inline Vector2<double> rot(double angle) {
		double sn = sin(angle);
		double cs = cos(angle);
		return Vector2(x*cs - y*sn, x*sn + y*cs);
	}
	friend T operator^(const Vector2 & a, const Vector2 & b) {
		return a.x*b.y - a.y*b.x;
	}
	friend T operator*(const Vector2 & a, const Vector2 & b) {
		return a.x*b.x + a.y*b.y;
	}
	friend Vector2 operator+(const Vector2 & a, const Vector2 & b) {
		return Vector2(a.x + b.x, a.y + b.y);;
	}
	friend Vector2 operator-(const Vector2 & a, const Vector2 & b) {
		return Vector2(a.x - b.x, a.y - b.y);;
	}
	friend Vector2 operator*(const Vector2 & a, T k) {
		return Vector2(a.x*k, a.y*k);
	}
	friend Vector2 operator*(T k, const Vector2 & a) {
		return Vector2(a.x*k, a.y*k);
	}
	friend Vector2 operator/(const Vector2 & a, T k) {
		return Vector2(a.x / k, a.y / k);
	}

	const Vector2 & operator+=(const Vector2 & v) {
		x += v.x; y += v.y;
		return *this;
	}
	const Vector2 & operator-=(const Vector2 & v) {
		x -= v.x; y -= v.y;
		return *this;
	}
	const Vector2 & operator*=(const T k) {
		x *= k; y *= k;
		return *this;
	}
	const Vector2 & operator/=(const T k) {
		x /= k; y /= k;
		return *this;
	}

	Vector2 operator-() {
		return Vector2(-x, -y);
	}
};

typedef Vector2<double> Vec2d;

#endif