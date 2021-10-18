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

	bool operator==(const Vector2D<T>& b) const {
		return (r[0] == b[0]) && (r[1] == b[1]);
	}

	bool operator!=(const Vector2D<T>& b) const {
		return !((r[0] == b[0]) && (r[1] == b[1]));
	}

private:
	
	T r[2];

};
