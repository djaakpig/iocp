#pragma once

#define NON_COPYABLE(ClassName)\
	ClassName(const ClassName&) = delete;\
	ClassName(const ClassName&&) = delete;\
	const ClassName& operator =(const ClassName&) = delete;\
	const ClassName& operator =(const ClassName&&) = delete;

template<typename T>
inline void SafeDelete(T*& pSingle)
{
	if(pSingle)
	{
		delete pSingle;
		pSingle = nullptr;
	}
}

template<typename T>
inline void SafeDeleteArray(T*& pArray)
{
	if(pArray)
	{
		delete[] pArray;
		pArray = nullptr;
	}
}