#pragma once

#include <stdio.h>
#include <array>
#include <iterator>
#include <algorithm>
#include <type_traits>
#include <memory>

#include "arena.h"

#define LOG_DEBUG_INFO_TO_COUT

namespace tools
{
	template<
		typename T, 
		size_t reserve_so_much_objects, 
		std::size_t alignment = alignof(std::max_align_t)
	> class reserve_allocator
	{
		// GCC extension 5.15 Structures With No Members
		static_assert(sizeof(T) != 0, "Structures that have zero size are not supported");
	public:
		using value_type = T;
		using pointer = typename std::add_pointer<T>::type;
		using const_pointer = typename std::add_const<pointer>::type;
		using reference = typename std::add_lvalue_reference<pointer>::type;
		using const_reference = typename std::add_const<reference>::type;

		template<typename U>
		struct rebind
		{
			using other = reserve_allocator<U, reserve_so_much_objects, alignment>;
		};

		static auto constexpr alignment = alignment;
		static auto constexpr size = reserve_so_much_objects;
		using memory_area_type = memory_area<size, alignment>;
	private:
		std::unique_ptr<memory_area_type> _reserved_memory;
	public:
		reserve_allocator()
			: _reserved_memory(
				std::make_unique<typename decltype(_reserved_memory)::element_type>()
			)
		{
		}
		~reserve_allocator() = default;

		template<
			typename U, 
			size_t UReserved,
			std::size_t UAlignment
		> reserve_allocator(reserve_allocator<U, UReserved, UAlignment> const & other)
			: reserve_allocator()
		{
		}

		pointer allocate(std::size_t objects_count) const
		{
#ifdef LOG_DEBUG_INFO_TO_COUT
			std::cout << __FUNCTION__ << std::endl
				<< __FUNCSIG__ << std::endl
				<< "[objects_count = " << objects_count << "]"
				<< std::endl << std::endl;
#endif
			return reinterpret_cast<pointer>(
				_reserved_memory->template allocate<alignof(T)>(
					objects_count * sizeof(T)
				)
			);
		}
		void deallocate(pointer p, std::size_t objects_count) const
		{
#ifdef LOG_DEBUG_INFO_TO_COUT
			std::cout << __FUNCTION__ << std::endl
				<< __FUNCSIG__ << std::endl
				<< "[objects_count = " << objects_count
				<< ", p = " << p << "]"
				<< std::endl << std::endl;
#endif
			_reserved_memory->deallocate(
				reinterpret_cast<uint8_t*>(p),
				objects_count * sizeof(T)
			);
		}

		template<typename U, typename ...Args>
		void construct(U* p, Args&& ...args) const
		{
#ifdef LOG_DEBUG_INFO_TO_COUT
			std::cout << __FUNCTION__ << std::endl
				<< __FUNCSIG__ << std::endl
				<< "[p = " << p
				<< ", args_count = " << sizeof...(args) << "]"
				<< std::endl << std::endl;
#endif

			new(p) U(std::forward<Args>(args)...);
		}
		void destroy(pointer p) const
		{
#ifdef LOG_DEBUG_INFO_TO_COUT
			std::cout << __FUNCTION__ << std::endl
				<< __FUNCSIG__ << std::endl
				<< "[p = " << p << "]"
				<< std::endl << std::endl; 
#endif
			p->~T();
		}
	};
}