#include <utility>
#include <array>
#include <exception>
#include <stdexcept>
#include <iostream>
#include "fb/result.hpp"

template <typename T, typename... Ts>
constexpr auto make_array(Ts&&... ts) {
	return std::array<T, sizeof...(Ts)>{std::forward<Ts&&>(ts)...};
}

constexpr auto tests = make_array<void(*)()>(
	[]{
		throw std::runtime_error{"bleh"};
	},

	[]{

	}
);

int main(int argc, char const* argv[]) {
	try {
		for (const auto& t : tests) {
			t();
		}
	} catch(std::exception& e) {
		std::cout << "\033[1;31mERROR\033[0m: " << e.what() << "\n";
		return 1;
	} catch(...) {
		std::cerr << "\033[1;31mERROR\033[0m: unknown exception\n";
		return 2;
	}

	return 0;
}
