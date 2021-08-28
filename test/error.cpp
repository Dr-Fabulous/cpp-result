#include "test.hpp"
#include "fb/result.hpp"

class code {
private:
	int m_code = 5;

public:
	code() = default;
	code(code const&) = default;
	code(code&&) = default;

	explicit code(int c) :
		m_code{c}
	{}

	code& operator=(code const&) = default;
	code& operator=(code&&) = default;

	int operator*() const noexcept {
		return m_code;
	}
};

constinit auto tests = std::tuple{
	[]{
		auto e1 = fb::make_error<code>();
		auto e2 = e1;
		auto e3 = std::move(e2);

		test::assert(**e1 == 5
			  && **e2 == 5
			  && **e3 == 5);
	},

	[]{
		auto e1 = fb::make_error<code>(100);
		auto e2 = e1;

		test::assert(**e1 == **e2);

		e2 = code{50};

		test::assert(**e2 == 50);
	},

	[]{
		auto e1 = fb::make_error<code>(1);
		auto e2 = fb::make_error<code>(2);

		std::swap(e1, e2);

		test::assert(**e1 == 2
			  && **e2 == 1);
	}
};

int main(int argc, char const* argv[]) {
	return test::run(tests);
}
