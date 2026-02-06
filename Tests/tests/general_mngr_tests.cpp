/********************************************************************
* GeneralManager<D> tests
* - Tests for GeneralManager for properties and whatnots
********************************************************************/

#include "all_tests.h"

#include "aol/aol.h"
#include "aol/data_components/managers.h"

#include <iostream>

namespace AoL
{

struct SampleManager : public AoL::DataManager<SampleManager>
{
	AoL::Vector<int> numbers;
	AoL::KeyOrderMap<int, int> two_numbers;

	SampleManager() noexcept :
		GenericMngr{ }
	{
		numbers.reserve(8);
	}

private:
	friend GenericMngr;

	void InitImpl()
	{
		numbers.reserve(100);
		for (int i = 0; i < 100; i++)
		{
			numbers.emplace_back(i);
		}

		for (int i = 0; i < 100; ++i)
		{
			two_numbers[i] += i * i;
		}
	}

	void DeInitImpl()
	{
		numbers.clear();
		two_numbers.clear();
	}
};

void GenericManagerTests() noexcept
{
	SampleManager mngr{};
	std::cout << "Manager first initialization\n\n";
	{
		mngr.Initialize();

		std::cout << "Number size: " << mngr.numbers.size() << '\n';
		for (auto n : mngr.numbers)
		{
			std::cout << "Number: " << n << '\n';
		}
		std::cout << '\n';

		std::cout << "Two number size: " << mngr.two_numbers.size() << '\n';
		for (auto [n1, n2] : mngr.two_numbers)
		{
			std::cout << "N1: " << n1 << " | N2: " << n2 << '\n';
		}
		std::cout << '\n';

		mngr.DeInitialize();

		std::cout << "Number size: " << mngr.numbers.size() << '\n';
		std::cout << "Two number size: " << mngr.two_numbers.size() << '\n';
		std::cout << '\n';
	}

	std::cout << "Manager second initialization\n\n";
	{
		mngr.Initialize();
		std::cout << "Number size: " << mngr.numbers.size() << '\n';
		std::cout << "Two number size: " << mngr.two_numbers.size() << '\n';
		mngr.DeInitialize();
		std::cout << "Number size: " << mngr.numbers.size() << '\n';
		std::cout << "Two number size: " << mngr.two_numbers.size() << '\n';
	}
}

} // namespace AoL

