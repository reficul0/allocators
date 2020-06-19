#pragma once

#include <cstddef>
#include <stdint.h>
#include <cassert>

namespace tools
{
	template<size_t arena_bytes_count, size_t alignment = alignof(std::max_align_t)>
	class memory_area
	{
		alignas(alignment) uint8_t _buf[arena_bytes_count];
		uint8_t *_buf_end = _buf + arena_bytes_count;
		uint8_t *_current;
	public:
		memory_area() noexcept
			: _current(_buf)
		{
		}
		memory_area(memory_area const&) = delete;
		memory_area& operator=(memory_area const&) = delete;

		template<size_t required_alignment>
		uint8_t* allocate(size_t bytes_count);
		void deallocate(uint8_t *p, size_t bytes_count) noexcept;

		static constexpr size_t size() noexcept { return arena_bytes_count; }
		size_t used() const noexcept { return static_assert<size_t>(_current - _buf); }
		void reset() noexcept { _current = _buf; };
	private:
		static size_t aligh_up(size_t n) noexcept
		{
			return (n + (alignment - 1)) & ~(alignment - 1);
		}
		bool is_pointer_in_buffer(uint8_t *p) noexcept
		{
			return _buf <= p && p <= _buf_end;
		}
	};

	template<size_t arena_bytes_count, size_t alignment>
	template<size_t required_alignment>
	inline uint8_t* memory_area<arena_bytes_count, alignment>::allocate(size_t bytes_count)
	{
		static_assert(required_alignment <= alignment, "alignment is too small for this memory_area");
		assert(is_pointer_in_buffer(_current) && "alloc has outlived memory_area");

		auto const aligned_bytes_count = aligh_up(bytes_count);
		if (static_cast<decltype(aligned_bytes_count)>(
			_buf + arena_bytes_count - _current
			) >= aligned_bytes_count
			) {
			auto *_current_before_advance = _current;
			_current += aligned_bytes_count;
			return _current_before_advance;
		}
		static_assert(alignment <= alignof(std::max_align_t), "you've chosen an "
			"alignment that is larger than alignof(std::max_align_t), and "
			"cannot be guaranteed by normal operator new");
		return static_cast<uint8_t*>(::operator new(bytes_count));
	}

	template<size_t arena_bytes_count, size_t alignment>
	inline void memory_area<arena_bytes_count, alignment>::deallocate(
		uint8_t *p, size_t bytes_count
	) noexcept {
		assert(is_pointer_in_buffer(_current) && "alloc has outlived memory_area");
		if (is_pointer_in_buffer(p))
		{
			bytes_count = aligh_up(bytes_count);
			if (p + bytes_count == _current)
				_current = p;
		}
		else
			::operator delete(p);
	}
}