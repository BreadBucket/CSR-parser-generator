#pragma once
#include <memory>


template <typename T>
inline std::unique_ptr<T> ptr(T* p){
	return std::unique_ptr<T>(p);
}
