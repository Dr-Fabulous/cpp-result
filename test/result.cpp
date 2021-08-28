#include "test.hpp"
#include "fb/result.hpp"

constinit auto tests = std::tuple{
	[]{
		auto foo = [](int i) -> fb::result<int, int> {
			if (i >= 0) {
				return i;
			} else {
				return fb::make_error(i);
			}
		};

		auto r1 = foo(1);

		test::assert(r1 && *r1 == 1);

		auto r2 = foo(0);

		test::assert(!r2 && r2.err() == 0);

		r2 = r1;

		test::assert(r2 && *r2 == 1);

		r1 = 100;

		test::assert(r1 && *r1 == 100);
	}
};

int main(int argc, char const* argv[]) {
	return test::run(tests);
}
