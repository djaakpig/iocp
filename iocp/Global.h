#pragma once

template<typename T>
inline void SafeDelete( T*& pSingle )
{
	if( pSingle )
	{
		delete pSingle;
		pSingle = nullptr;
	}
}

template<typename T>
inline void SafeDeleteArray( T*& pArray )
{
	if( pArray )
	{
		delete[] pArray;
		pArray = nullptr;
	}
}