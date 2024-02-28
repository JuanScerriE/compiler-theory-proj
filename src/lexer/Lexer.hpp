#pragma once

#include <array>
#include <string>

namespace Vought {

class Lexer {
public:
	explicit Lexer(std::string source);
private:
	std::array<std::array<int, 16>, 16> tx_table;
	int line = 0;
	size_t current = 0;
	size_t start = 0;
};

} // namespace Vought
