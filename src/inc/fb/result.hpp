#ifndef FB_CPP_RESULT_HPP
#define FB_CPP_RESULT_HPP
#include <type_traits>
#include <concepts>
#include <utility>
#include <variant>
#include <functional>
#include <stdexcept>

namespace fb {
	namespace result_detail {
		template <typename T, template <typename...> typename U>
		struct is_template_of : std::false_type {};

		template <template <typename...> typename U, typename... Us>
		struct is_template_of<U<Us...>, U> : std::true_type {};

		template <typename T, template <typename...> typename U>
		concept template_of = is_template_of<T, U>::value;
	}

	template <typename E>
	class error {
	public:
		using error_type = std::decay_t<E>;

	private:
		error_type m_err;

	public:
		constexpr error() = default;
		constexpr error(error const&) = default;
		constexpr error(error&&) = default;

		constexpr error(error_type const& e) :
			m_err{e}
		{}

		constexpr error(error_type&& e) :
			m_err{std::move(e)}
		{}

		template <typename... Args>
		constexpr error(std::in_place_t, Args&&... args) :
			m_err{std::forward<Args&&>(args)...}
		{}

		constexpr error& operator=(error const&) = default;
		constexpr error& operator=(error&&) = default;

		constexpr error& operator=(error_type const& e) {
			m_err = e;
			return *this;
		}

		constexpr error& operator=(error_type&& e) {
			m_err = std::move(e);
			return *this;
		}

		template <typename U>
		constexpr error& operator=(U&& u) {
			m_err = std::forward<U&&>(u);
			return *this;
		}

		constexpr error_type& operator*() & { return m_err; }
		constexpr error_type&& operator*() && { return std::move(m_err); }
		constexpr error_type const& operator*() const& { return m_err; }
		constexpr error_type const&& operator*() const&& { return std::move(m_err); }

		friend void swap(error& e1, error& e2) {
			using std::swap;
			swap(e1.m_err, e2.m_err);
		}
	};

	template <typename E>
	constexpr error<E> make_error(E&& e) {
		return {std::forward<E&&>(e)};
	}

	template <typename E, typename... Args>
	constexpr error<E> make_error(Args&&... args) {
		return {std::in_place, std::forward<Args&&>(args)...};
	}

	struct bad_result_access : public std::runtime_error {
		using std::runtime_error::runtime_error;
	};

	template <typename V, typename E>
	class result {
	public:
		using value_type = std::decay_t<V>;
		using error_type = std::decay_t<E>;

	private:
		std::variant<value_type, error_type> m_value;

	public:
		constexpr result() = default;
		constexpr result(result const&) = default;
		constexpr result(result&&) = default;

		constexpr result(value_type const& v) :
			m_value{std::in_place_index<0>, v}
		{}

		constexpr result(value_type&& v) :
			m_value{std::in_place_index<0>, std::move(v)}
		{}

		template <typename U>
		constexpr result(U&& u) :
			m_value{std::in_place_index<0>, std::forward<U&&>(u)}
		{}

		constexpr result(error<error_type> const& e) :
			m_value{std::in_place_index<1>, *e}
		{}

		constexpr result(error<error_type>&& e) :
			m_value{std::in_place_index<1>, std::move(*e)}
		{}

		template <typename U>
		constexpr result(error<U> const& e) :
			m_value{std::in_place_index<1>, *e}
		{}

		template <typename U>
		constexpr result(error<U>&& e) :
			m_value{std::in_place_index<1>, std::move(*e)}
		{}

		constexpr result& operator=(result const&) = default;
		constexpr result& operator=(result&&) = default;

		constexpr result& operator=(value_type const& v) {
			m_value.template emplace<0>(v);
			return *this;
		}

		constexpr result& operator=(value_type&& v) {
			m_value.template emplace<0>(std::move(v));
			return *this;
		}

		template <typename U>
		constexpr result& operator=(U&& u) {
			m_value.template emplace<0>(std::forward<U&&>(u));
			return *this;
		}

		constexpr result& operator=(error<error_type> const& e) {
			m_value.template emplace<1>(*e());
			return *this;
		}

		constexpr result& operator=(error<error_type>&& e) {
			m_value.template emplace<1>(std::move(*e()));
			return *this;
		}

		template <typename U>
		constexpr result& operator=(error<U> const& e) {
			m_value.template emplace<1>(*e());
			return *this;
		}

		template <typename U>
		constexpr result& operator=(error<U>&& e) {
			m_value.template emplace<1>(std::move(*e()));
			return *this;
		}

		constexpr bool has_val() const noexcept {
			return !m_value.index();
		}

		constexpr bool has_err() const noexcept {
			return m_value.index() == 1;
		}

		constexpr operator bool() const noexcept {
			return has_val();
		}

		constexpr value_type& operator*() & {
			if (!has_val()) {
				throw bad_result_access{"missing value"};
			}

			return std::get<0>(m_value);
		}

		constexpr value_type&& operator*() && {
			if (!has_val()) {
				throw bad_result_access{"missing value"};
			}

			return std::move(std::get<0>(m_value));
		}

		constexpr value_type const& operator*() const& {
			if (!has_val()) {
				throw bad_result_access{"missing value"};
			}

			return std::get<0>(m_value);
		}

		constexpr value_type const&& operator*() const&& {
			if (!has_val()) {
				throw bad_result_access{"missing value"};
			}

			return std::move(std::get<0>(m_value));
		}

		constexpr value_type* operator->() {
			if (!has_val()) {
				throw bad_result_access{"missing value"};
			}

			return std::addressof(**this);
		}

		constexpr value_type const* operator->() const {
			if (!has_val()) {
				throw bad_result_access{"missing value"};
			}

			return std::addressof(**this);
		}

		constexpr error_type& err() & {
			if (!has_err()) {
				throw bad_result_access{"missing error"};
			}

			return std::get<1>(m_value);
		}

		constexpr error_type&& err() && {
			if (!has_err()) {
				throw bad_result_access{"missing error"};
			}

			return std::move(std::get<1>(m_value));
		}

		constexpr error_type const& err() const& {
			if (!has_err()) {
				throw bad_result_access{"missing error"};
			}

			return std::get<1>(m_value);
		}

		constexpr error_type const&& err() const&& {
			if (!has_err()) {
				throw bad_result_access{"missing error"};
			}

			return std::move(std::get<1>(m_value));
		}

		template <typename F> requires requires(F&& f, value_type&& v) {{std::invoke(f, v)} -> std::convertible_to<value_type>;}
		constexpr result& fmap(F&& f) noexcept(std::is_nothrow_invocable_v<F&&, value_type>) {
			if (has_val()) {
				**this = std::invoke(f, **this);
			}

			return *this;
		}

		template <typename F> requires requires(F&& f, value_type&& v) {
			{std::invoke(f, v)} -> result_detail::template_of<result>;
			requires std::same_as<error_type, typename std::invoke_result_t<F&&, value_type&&>::error_type>;
		}
		constexpr std::invoke_result_t<F&&, value_type&&> bind(F&& f) {
			if (has_val()) {
				return std::invoke(f, **this);
			} else {
				return make_error(std::move(err()));
			}
		}

		template <std::invocable<> F> requires std::convertible_to<std::invoke_result_t<F&&>, value_type>
		constexpr value_type or_value(F&& f) {
			if (!has_val()) {
				return std::invoke(f);
			} else {
				return **this;
			}
		}
	};
}

#endif
