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
	int value1; 
	hard(int value1_) 
		: value1(value1_)
	{
	}
};

template<typename Allocator>
void test(std::string test_file_name)
{
	std::ofstream out(test_file_name); 
	std::streambuf *coutbuf = std::cout.rdbuf();
	std::cout.rdbuf(out.rdbuf());

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
	test<tools::logging_allocator<int>>("logging_out.txt");
	test<tools::reserve_allocator<int, 3>>("reserve_out.txt");
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtDumpMemoryLeaks();
}