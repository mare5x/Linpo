#pragma once

#include <string>
#include <sstream>

// place for functions that need to be reimplemented to compile with ndk
namespace patch
{
	template<class T>
	std::string to_string(const T& n)
	{
		std::ostringstream stream;
		stream << n;
		return stream.str();
	}
}