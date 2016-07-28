#pragma once

#include <string>
#include <iostream>
#include "grid_types.h"


template <class T>
inline void mlog(const std::string &message, const T &append) {
	std::cerr << message << append << std::endl;
}

inline void mlog(const std::string &message) {
	std::cerr << message << std::endl;
}

inline bool operator==(const Line &lhs, const Line &rhs) {
	return lhs.owner == rhs.owner && lhs.start == rhs.start && lhs.end == rhs.end;
}

inline bool operator!=(const Line &lhs, const Line &rhs) {
	return !(lhs == rhs);
}