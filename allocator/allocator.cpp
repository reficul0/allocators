#include "pch.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <iostream>
#include <fstream>
#include <list>
#include <algorithm>
#include <iterator>

#include "logging_allocator.h"
#include "reserve_allocator.h"

struct hard { 
	int value1, 
		value2; 
	hard(int value1_, int value2_) 
		: value1(value1_)
		, value2(value2_) 
	{
	}
};

template<typename Allocator>
void test(std::string test_file_name)
{
	std::ofstream out(test_file_name); //откроем файл для вывод
	std::streambuf *coutbuf = std::cout.rdbuf(); //запомним старый буфер
	std::cout.rdbuf(out.rdbuf()); //и теперь все будет в файл out.txt!

	auto values = std::list<typename Allocator::value_type, Allocator>{};

	std::cout << "-------------cycle begin---------------" << std::endl;

	for (size_t i = 0; i < 3; ++i)
	{
		values.emplace_back(i);
		std::cout << "-------------value{" << i << "}---------------" << std::endl;
	}

	std::cout << "-------------cycle end---------------" << std::endl;

	std::cout.rdbuf(coutbuf);
}

int main()
{
	test<tools::logging_allocator<size_t>>("logging_out.txt");

	test<tools::reserve_allocator<size_t, 3>>("reserve_out.txt");
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtDumpMemoryLeaks();
}