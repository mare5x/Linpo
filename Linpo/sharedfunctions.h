#pragma once

#include <string>
#include <iostream>

template <class T>
inline void mlog(const std::string &message, const T &append) {
	std::cerr << message << append << std::endl;
}

inline void mlog(const std::string &message) {
	std::cerr << message << std::endl;
}