#ifndef _MISC_HPP
#define _MISC_HPP

enum Boolean { FALSE=(0==1), TRUE=(1==1) };

template <class T>
inline void swap(T &a, T &b)
	{
	T t(a);
	a = b;
	b = t;
	}

template <class T>
inline T min(T a, T b)
	{
	return (a<b) ? a : b;
	}

template <class T>
inline T max(T a, T b)
	{
	return (a>b) ? a : b;
	}

template <class T>
inline T absolute(T a)
	{
	return (a<0) ? -a : a;
	}

template <class T>
inline void sort(T &a, T &b)
	{
	if (a>b)
		swap(a, b);
	}

#endif