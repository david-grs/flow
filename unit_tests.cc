#include "flow.h"

#include "gtest/gtest.h"

#include <map>
#include <string>
#include <iostream>

struct Square{};
struct Triangle{};
struct Circle{};

static std::map<std::string, int> Calls;

struct BlockA : public IBlock<BlockA, void, Triangle>
{
	static std::string GetName() { return "A"; }

	void Process(const Square&)
	{
		FAIL();
	}
};

struct BlockB : public IBlock<BlockB, Triangle, Circle>
{
	static std::string GetName() { return "B"; }

	void Process(const Triangle&)
	{
		if (++Calls[GetName()] % 2 == 0)
		{
			Send({});
		}
	}
};

struct BlockC : public IBlock<BlockC, Circle, Square>
{
	static std::string GetName() { return "C"; }

	void Process(const Circle&)
	{
		++Calls[GetName()];
	}
};

struct BlockD : public IBlock<BlockD, Triangle, Triangle>
{
	static std::string GetName() { return "D"; }

	void Process(const Triangle&)
	{
		++Calls[GetName()];
	}
};

struct FlowTest : public  ::testing::Test
{
	FlowTest()
	{
		mFactory.Register<BlockA>();
		mFactory.Register<BlockB>();
		mFactory.Register<BlockC>();
		mFactory.Register<BlockD>();
	}

protected:
	FlowFactory mFactory;
};

TEST_F(FlowTest, GetBlockName)
{
	auto flow = mFactory.CreateFlow({"A", "B", "C"});

	EXPECT_EQ(3, flow.size());
	EXPECT_EQ("A", flow[0]->GetBlockName());
	EXPECT_EQ("B", flow[1]->GetBlockName());
	EXPECT_EQ("C", flow[2]->GetBlockName());
}

TEST_F(FlowTest, Valid)
{
	EXPECT_NO_THROW(mFactory.CreateFlow({"A"}));
	EXPECT_NO_THROW(mFactory.CreateFlow({"A", "B"}));
	EXPECT_NO_THROW(mFactory.CreateFlow({"A", "D", "D", "B", "C"}));
}

TEST_F(FlowTest, Invalid)
{
	EXPECT_ANY_THROW(mFactory.CreateFlow({"C", "A"}));
	EXPECT_ANY_THROW(mFactory.CreateFlow({"D", "D"}));
	EXPECT_ANY_THROW(mFactory.CreateFlow({"B", "C"}));
	EXPECT_ANY_THROW(mFactory.CreateFlow({"A", "A"}));
	EXPECT_ANY_THROW(mFactory.CreateFlow({"A", "C"}));
	EXPECT_ANY_THROW(mFactory.CreateFlow({"B", "A"}));
	EXPECT_ANY_THROW(mFactory.CreateFlow({"B", "B"}));
	EXPECT_ANY_THROW(mFactory.CreateFlow({"C", "B"}));
	EXPECT_ANY_THROW(mFactory.CreateFlow({"C", "C"}));
}

TEST_F(FlowTest, Send)
{
	auto flow = mFactory.CreateFlow({"A", "B", "C"});

	BlockA* blkA = dynamic_cast<BlockA*>(flow[0].get());

	EXPECT_TRUE(Calls.empty());

	blkA->Send({});
	EXPECT_EQ(1, Calls["B"]);
	EXPECT_EQ(0, Calls["C"]);

	blkA->Send({});
	EXPECT_EQ(2, Calls["B"]);
	EXPECT_EQ(1, Calls["C"]);
}
