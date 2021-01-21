# String Library

## Overview
A simple header-only string library written in C++ using a similar interface to `std::string`.

## Features
- Automatic memory management using RAII techniques
- Strong exception guarantee (state is unmodified if an exception is thrown)
- Use of move semantics wherever possible (e.g. `operator+()` is overloaded to take R-value references, `substring()` is overloaded with ref-qualifiers)
- Basic string operations like concatenation, substring, insert, trim, etc.
- Comparing with C-style strings and `char`, as well as lexicographic comparison functions
- Constructing from C-style strings and `std::initializer_list`
- Writing to C++ output streams
- Fully const-correct and decorated with `noexcept` specifiers

## Todo
- Add iterator support
- Add allocator support
- Implement SSO (Small String Optimization)

## Project Requirements
C++14 language version.

## License
Licensed under [MIT](LICENSE).
