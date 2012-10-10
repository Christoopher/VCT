
#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>

template <class T>
struct Vec3
{

	Vec3()
	{
		for(int i = 0; i < 3; ++i)
		{
			pos[i]=T();
		}
	}
	Vec3(T x, T y, T z)
	{
		pos[0] = x;
		pos[1] = y;
		pos[2] = z;
	}

	Vec3(const Vec3<T> & v)
	{
		pos[0] = v(0);
		pos[1] = v(1);
		pos[2] = v(2);
	}

	Vec3<T> & operator=(const Vec3<T> & v)
	{
		pos[0] = v(0);
		pos[1] = v(1);
		pos[2] = v(2);
		return *this;
	}

	template <class U>
	Vec3(const Vec3<U> & v)
	{
		pos[0] = v(0);
		pos[1] = v(1);
		pos[2] = v(2);
	}
	


	Vec3<T> operator*(const T scale) 
	{
		Vec3 vec;
		vec.pos[0] = pos[0]*scale;
		vec.pos[1] = pos[1]*scale;
		vec.pos[2] = pos[2]*scale;
		return vec;
	}

	Vec3<T> operator+(const Vec3<T>& v) 
	{
		Vec3 vec;
		vec.pos[0] = pos[0] +  v(0);
		vec.pos[1] = pos[1] +  v(1);
		vec.pos[2] = pos[2] +  v(2);
		return vec;
	}

	T operator()(int i) const
	{
		return pos[i];
	}

	T operator()(int i)
	{
		return pos[i];
	}

	T pos[3];	
};
typedef Vec3<int> Vec3i;
typedef Vec3<float> Vec3f;



template <class T>
std::ostream & operator<<(std::ostream & o, const Vec3<T> & p)
{
	o << "[" << p(0) << ", " << p(1) << ", " << p(2) << "]";
	return o;
}

#endif