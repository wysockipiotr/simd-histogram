#pragma once

#include <cstddef>
#include <memory>
#include <cstdlib>

template <class T, std::size_t A>
class aligned_allocator {
public:
	using value_type = T;
	using reference = T & ;
	using const_reference = const T&;
	using pointer = T * ;
	using const_pointer = const T*;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;

	template <class U>
	struct rebind { typedef aligned_allocator<U, A> other; };

	inline aligned_allocator() noexcept {}
	inline aligned_allocator(const aligned_allocator&) noexcept {}

	template <class U>
	inline explicit aligned_allocator(const aligned_allocator<U, A>&) noexcept {}

	inline ~aligned_allocator() noexcept {}

	inline pointer address(reference r) { return &r; }
	inline const_pointer address(const_reference r) const { return &r; }

	auto allocate(size_type n, const_pointer hint = 0);
	inline void deallocate(pointer p, size_type);

	inline void construct(pointer p, const_reference value) { new (p) value_type(value); }
	inline void destroy(pointer p) { p->~value_type(); }

	inline auto max_size() const noexcept { return size_type(-1) / sizeof(T); }

	inline auto operator==(const aligned_allocator&) { return true; }
	inline auto operator!=(const aligned_allocator&) { return false; }
};

template <class T, std::size_t A>
auto aligned_allocator<T, A>::allocate(size_type n, const_pointer hint) {
	if (auto res = static_cast<T*>(_aligned_malloc(n * sizeof(T), A))) {
		return res;
	}
	throw std::bad_alloc{};
}

template <class T, std::size_t A>
void aligned_allocator<T, A>::deallocate(pointer p, size_type) {
	_aligned_free(p);
}