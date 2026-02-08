/********************************************************************
* DataBasic tests
* - Tests all DataBasic properties and whatnots
********************************************************************/

#ifndef BENCHMARK_CONFIG_ON

#include "all_tests.h"

#include "aol/aol.h"
#include "aol/data_components/profiles.h"

#include <iostream> // std::cout

namespace AoL
{

struct SampleData : AoL::DataBasic<SampleData>
{
	int x;
	int y;

	SampleData() noexcept :
		DataBasic{ },
		x{ 0 },
		y{ 0 }
	{
	}
};

void BasicDataTests() noexcept
{
	SampleData data{};
	data.name = "Data #1";
	data.short_name = "D1";
	data.id = static_cast<SampleData::IDT>(1);
	data.x = 100;
	data.y = 100 * data.x;

	std::cout	<< "Basic data plus derived class data\n";
	std::cout	<< "Data name: "	<< data.name << '\n'
				<< "Symbol: "		<< data.short_name << '\n'
				<< "ID: "			<< data.id.value_ << '\n'
				<< "X coordinate: " << data.x << '\n'
				<< "Y coordinate: " << data.y << "\n\n";
}

} // namespace AoL

#endif
