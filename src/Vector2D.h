#pragma once
#include <type_traits>

template<typename T>
struct Vector2D {

public:

	Vector2D() {
		r[0] = (T)0;
		r[1] = (T)0;
	}
	Vector2D(T x, T y) {
		r[0] = x;
		r[1] = y;
	}

	T operator[](int i) {
		return r[i];
	}

	T operator[](int i) const {
		return r[i];
	}

private:
	
	T r[2];

};
