#include "pch.h"

#include <boost/bind.hpp>
#include <boost/filesystem/path.hpp>

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

#define BOOST_TEST_MODULE custom_main
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(memory_leak_test_suite)

struct hard {
	int value1;
	hard(int value1_)
		: value1(value1_)
	{
	}
};

template<typename Callable>
bool test_for_memory_leaks_return_false_if_any(Callable &&test_me)
{
	_CrtMemState s1, s2, s3;

	_CrtMemCheckpoint(&s1);
	std::forward<Callable>(test_me)();
	_CrtMemCheckpoint(&s2);

	if (_CrtMemDifference(&s3, &s1, &s2))
	{
		_CrtMemDumpStatistics(&s3);
		return false;
	}
	return true;
}

template<typename Allocator>
void make_payload(boost::filesystem::path test_info_destination_path)
{
	std::ofstream out(test_info_destination_path.string());
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

BOOST_AUTO_TEST_CASE(logging_allocator_int_test_memory_leak)
{
	BOOST_TEST(
		test_for_memory_leaks_return_false_if_any(
			boost::bind(
				make_payload<tools::logging_allocator<int>>,
				"tests_out/logging_int_out.txt"
			)
		)
	);
}

BOOST_AUTO_TEST_CASE(reserve_allocator_int_test_memory_leak)
{
	BOOST_TEST(
		test_for_memory_leaks_return_false_if_any(
			boost::bind(
				make_payload<tools::reserve_allocator<int, 3>>,
				"tests_out/reserve_int_out.txt"
			)
		)
	);
}

BOOST_AUTO_TEST_CASE(logging_allocator_hard_test_memory_leak)
{
	BOOST_TEST(
		test_for_memory_leaks_return_false_if_any(
			boost::bind(
				make_payload<tools::logging_allocator<hard>>,
				"tests_out/logging_hard_out.txt"
			)
		)
	);
}

BOOST_AUTO_TEST_CASE(reserve_allocator_hard_test_memory_leak)
{
	BOOST_TEST(
		test_for_memory_leaks_return_false_if_any(
			boost::bind(
				make_payload<tools::reserve_allocator<hard, 3>>,
				"tests_out/reserve_hard_out.txt"
			)
		)
	);
}

BOOST_AUTO_TEST_SUITE_END()

int main(int argc, char* argv[], char* envp[])
{
	return boost::unit_test::unit_test_main(init_unit_test, argc, argv);
}