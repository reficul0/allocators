#pragma once

#include <stdio.h>
#include <array>
#include <iterator>
#include <algorithm>
#include <type_traits>
#include <memory>

namespace tools
{
	template<typename T, size_t reserve_so_much>
	class reserve_allocator
	{
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
			using other = reserve_allocator<U, reserve_so_much>;
		};
#pragma endregion

		mutable size_t used_count;
		mutable std::array<pointer, reserve_so_much> reserved_memory;

		reserve_allocator()
			: used_count(0)
		{
			for(auto &p : reserved_memory)
				p = static_cast<pointer>(std::malloc(sizeof(T)));
		}
		~reserve_allocator()
		{
			for (size_t i = used_count; i < reserve_so_much; ++i)
				std::free(reserved_memory[i]);
		}

		template<
			typename U, 
			size_t UReserved
		>
		reserve_allocator(reserve_allocator<U, UReserved> const & other)
			: reserve_allocator()
		{
		}

		pointer allocate(std::size_t objects_count) const
		{
			using return_type = decltype(allocate(objects_count));

			std::cout << __FUNCTION__ << std::endl
				<< __FUNCSIG__ << std::endl
				<< "[objects_count = " << objects_count << "]" 
				<< std::endl << std::endl;

			if (_has_free_reserved_memory(objects_count))
				return _get_next_from_reserved_memory(objects_count);

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
				<< ", p = "<< p << "]"
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
	private:
		bool _has_free_reserved_memory(std::size_t objects_count) const
		{
			/*std::cout << __FUNCTION__ << std::endl
				<< __FUNCSIG__ << std::endl
				<< "[objects_count = " << objects_count << "]"
				<< std::endl << std::endl;*/

			auto now_free = reserve_so_much - used_count;
			return now_free != 0 && now_free >= objects_count;
		}
		pointer _get_next_from_reserved_memory(std::size_t objects_count) const
		{
			/*std::cout << __FUNCTION__ << std::endl
				<< __FUNCSIG__ << std::endl
				<< "[objects_count = " << objects_count << "]"
				<< std::endl << std::endl;*/

			auto &ptr_in_reserved_memory = reserved_memory[used_count];
			auto ptr_for_returning = ptr_in_reserved_memory;

			auto new_size = used_count + objects_count;
			for(size_t i = used_count; i < new_size; ++i)
				reserved_memory[i] = nullptr;
			used_count = new_size;

			return ptr_for_returning;
		}
	};
}