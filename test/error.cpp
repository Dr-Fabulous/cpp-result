#include "test.hpp"
#include "fb/result.hpp"

constinit auto tests = fb::test::make_tests();

int main(int argc, char const* argv[]) {
	return fb::test::run(tests);
}
