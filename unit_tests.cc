#include "flow.h"

#include <gtest/gtest.h>

#include <iostream>

TEST(flow, valid)
{
	auto flow = CreateFlow({"A", "B", "C"});

	for (const auto& block : flow)
	{
		std::cout << "elem='" << block->GetBlockName() << "'" << std::endl;
	}
}
