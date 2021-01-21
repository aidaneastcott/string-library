
#include "SimpleString.hpp"

#include <iomanip>
#include <iostream>
#include <string>
#include <utility>


template <typename Type>
void info(Type &&arg) {
	constexpr std::streamsize WIDTH = 12;
	std::cout << std::left << std::setw(WIDTH) << "contents:" << '"' << arg << "\"\n";
	std::cout << std::left << std::setw(WIDTH) << "size:" << arg.size() << '\n';
	std::cout << std::left << std::setw(WIDTH) << "capacity:" << arg.capacity() << "\n\n";
}

template <typename Type, typename ...Types>
void info(Type &&arg, Types &&...args) {
	info(arg);
	info(std::forward<Types>(args)...);
}


int main() {

	using simple::String;


	String big = "very big and long string of text. ";
	String small = "short text. ";


	String result = big + small;


	info(big, small, result);

	return 0;
}
