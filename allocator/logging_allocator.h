#pragma once

#include <stdio.h>
#include <array>
#include <iterator>
#include <algorithm>
#include <type_traits>
#include <memory>

namespace tools
{
	template<typename T>
	class logging_allocator
	{
		// GCC extension 5.15 Structures With No Members
		static_assert(sizeof(T) != 0, "Structures that have zero size are not supported");
	public:
		using value_type = T;
#pragma region for_map
		using pointer = typename std::add_pointer<T>::type;
		using const_pointer = typename std::add_const<pointer>::type;
		using reference = typename std::add_lvalue_reference<pointer>::type;
		using const_reference = typename std::add_const<reference>::type;

		template<typename U>
		struct rebind
		{
			// Некоторые компиляторы могут сгенерировать ребиндер самостоятельно, 
			// но нам надо чтобы он был ане зависимости от компилятора	, 
			// поэтому напишем его сами.
			using other = logging_allocator<U>;
		};
#pragma endregion

		logging_allocator()
		{
		}
		~logging_allocator() = default;

		template<typename U>
		logging_allocator(logging_allocator<U> const &other)
		{
		}

		pointer allocate(std::size_t objects_count) const
		{
			using return_type = decltype(allocate(objects_count));

			std::cout << __FUNCTION__ << std::endl
				<< __FUNCSIG__ << std::endl
				<< "[objects_count = " << objects_count << "]"
				<< std::endl << std::endl;

			if (objects_count > std::numeric_limits<std::size_t>::max() / sizeof(T))
				throw std::bad_alloc();

			auto p = static_cast<return_type>(
				std::malloc(objects_count * sizeof(T))
				);
			return p != nullptr ? p
				: throw std::bad_alloc{};
		}
		void deallocate(pointer p, std::size_t objects_count) const
		{
			std::cout << __FUNCTION__ << std::endl
				<< __FUNCSIG__ << std::endl
				<< "[objects_count = " << objects_count
				<< ", p = " << p << "]"
				<< std::endl << std::endl;
			std::free(p);
		}

		template<typename U, typename ...Args>
		void construct(U* p, Args&& ...args) const
		{
			std::cout << __FUNCTION__ << std::endl
				<< __FUNCSIG__ << std::endl
				<< "[p = " << p
				<< ", args_count = " << sizeof...(args) << "]"
				<< std::endl << std::endl;
			new(p) U(std::forward<Args>(args)...);
		}
		void destroy(pointer p) const
		{
			std::cout << __FUNCTION__ << std::endl
				<< __FUNCSIG__ << std::endl
				<< "[p = " << p << "]"
				<< std::endl << std::endl;
			p->~T();
		}
	};
}