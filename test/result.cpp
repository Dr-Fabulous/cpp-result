#include <memory>
#include "test.hpp"
#include "fb/result.hpp"

constinit auto tests = std::tuple{
	[](){
		auto foo = [](int i) -> fb::result<int, int> {
			if (i >= 0) {
				return i;
			} else {
				return fb::make_error(i);
			}
		};

		auto r1 = foo(1);

		test::assert(r1);
		test::assert(*r1 == 1);

		auto r2 = foo(-1);

		test::assert(!r2);
		test::assert(r2.err() == -1);

		r2 = r1;

		test::assert(r2);
		test::assert(*r2 == 1);

		r1 = 100;

		test::assert(r1);
		test::assert(*r1 == 100);

		r1 = 5.1f;

		test::assert(r1);
		test::assert(*r1 == 5);
	},

	[](){
		auto foo = [](int i) -> fb::result<std::unique_ptr<int>, int> {
			if (i >= 0) {
				return std::make_unique<int>(i);
			} else {
				return fb::make_error(i);
			}
		};

		auto r1 = foo(10);

		test::assert(r1);
		test::assert(**r1 == 10);

		auto r2 = foo(-1);

		test::assert(!r2);
		test::assert(r2.err() == -1);

		r2 = std::move(r1);

		test::assert(r2);
		test::assert(**r2 == 10);
	},

	[](){
		auto square = [](int i) -> fb::result<int, char const*> {
			if (i <= 0) {
				return fb::make_error("int cannot be zero or lower");
			}

			return i * i;
		};

		auto add_5 = [](int i) {
			return i + 5;
		};

		auto r1 = square(0)
			 .bind(square)
			 .fmap(add_5)
			 .or_value([](){ return 100; });

		test::assert(r1 == 100);

		auto r2 = square(5)
			 .fmap(add_5)
			 .bind(square)
			 .or_value([](){ return 100; });

		test::assert(r2 == 900);
	}
};

int main(int argc, char const* argv[]) {
	return test::run(tests);
}
