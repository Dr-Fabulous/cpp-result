#ifndef FB_CPP_RESULT_HPP
#define FB_CPP_RESULT_HPP
#include <type_traits>
#include <utility>
#include <variant>
#include <functional>

namespace fb {
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

		error_type& get() & { return m_err; }
		error_type&& get() && { return std::move(m_err); }
		error_type const& get() const& { return m_err; }
		error_type const&& get() const&& { return std::move(m_err); }

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
		return {std::in_place_t{}, std::forward<Args&&>(args)...};
	}

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
			m_value{std::in_place_index_t<0>{}, v}
		{}

		constexpr result(value_type&& v) :
			m_value{std::in_place_index_t<0>{}, std::move(v)}
		{}

		template <typename U>
		constexpr result(U&& u) :
			m_value{std::in_place_index_t<0>{}, std::forward<U&&>(u)}
		{}

		constexpr result(error<error_type> const& e) :
			m_value{std::in_place_index_t<1>{}, e.get()}
		{}

		constexpr result(error<error_type>&& e) :
			m_value{std::in_place_index_t<1>{}, std::move(e.get())}
		{}

		template <typename U>
		constexpr result(error<U> const& e) :
			m_value{std::in_place_index_t<1>{}, e.get()}
		{}

		template <typename U>
		constexpr result(error<U>&& e) :
			m_value{std::in_place_index_t<1>{}, std::move(e.get())}
		{}

		constexpr result& operator=(result const&) = default;
		constexpr result& operator=(result&&) = default;

		constexpr result& operator=(value_type const& v) {
			m_value.emplace<0>(v);
			return *this;
		}

		constexpr result& operator=(value_type&& v) {
			m_value.emplace<0>(std::move(v));
			return *this;
		}

		template <typename U>
		constexpr result& operator=(U&& u) {
			m_value.emplace<0>(std::forward<U&&>(u));
			return *this;
		}

		constexpr result& operator=(error<error_type> const& e) {
			m_value.emplace<1>(e.get());
			return *this;
		}

		constexpr result& operator=(error<error_type>&& e) {
			m_value.emplace<1>(std::move(e.get()));
			return *this;
		}

		template <typename U>
		constexpr result& operator=(error<U> const& e) {
			m_value.emplace<1>(e.get());
			return *this;
		}

		template <typename U>
		constexpr result& operator=(error<U>&& e) {
			m_value.emplace<1>(std::move(e.get()));
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

		constexpr value_type& val() & { return std::get<0>(m_value); }
		constexpr value_type&& val() && { return std::move(std::get<0>(m_value)); }
		constexpr value_type const& val() const& { return std::get<0>(m_value); }
		constexpr value_type const&& val() const&& { return std::move(std::get<0>(m_value)); }

		template <typename F>
		constexpr value_type val_or(F&& f) {
			if (has_val()) {
				return val();
			} else {
				return std::invoke(std::forward<F&&>(f));
			}
		}

		constexpr error_type& err() & { return std::get<1>(m_value); }
		constexpr error_type&& err() && { return std::move(std::get<1>(m_value)); }
		constexpr error_type const& err() const& { return std::get<1>(m_value); }
		constexpr error_type const&& err() const&& { return std::move(std::get<1>(m_value)); }

		template <typename F>
		constexpr error_type err_or(F&& f) {
			if (has_err()) {
				return err();
			} else {
				return std::invoke(std::forward<F&&>(f));
			}
		}
	};
}

#endif
