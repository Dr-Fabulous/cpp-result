#ifndef FB_CPP_RESULT_TEST_HPP
#define FB_CPP_RESULT_TEST_HPP
#include <exception>
#include <stdexcept>
#include <array>
#include <functional>
#include <iostream>

namespace fb::test {
	template <typename... Ts>
	constexpr auto make_tests(Ts&&... ts) {
		return std::array<void(*)(), sizeof...(Ts)>{std::forward<Ts&&>(ts)...};
	}

	int run(auto&& tests) {
		try {
			for (const auto& t : tests) {
				std::invoke(t);
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
}

#endif
