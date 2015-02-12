/* --------------------------------------------------------------
	File:			AXSingleton.h
	Description:	Defines a class to enforce the singleton rule.  All a class needs to do
					is inherit this class.
	Date:			June 29, 2005
	Author:			James Long

-------------------------------------------------------------- */

#if !defined(AXSINGLETON_INCLUDE)
#define AXSINGLETON_INCLUDE

#include "..\\AXCommon.h"
#include <cassert>

template<typename T>
class AXSingleton {
private:
	static T* _SingletonPointer;
public:
	AXSingleton() {
		assert(!_SingletonPointer && "Tried to create multiple instances of a singleton class!");
		_SingletonPointer = (T*)this;
	}
	virtual ~AXSingleton() {
		_SingletonPointer = NULL;
	}
	static T& Instance() {
		return *_SingletonPointer;
	}
	static T* InstancePtr() {
		return _SingletonPointer;
	}
};

template <typename T>
T* AXSingleton<T>::_SingletonPointer = NULL;

#endif
