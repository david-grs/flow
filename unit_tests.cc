#include "flow.h"

#include "gtest/gtest.h"

#include <iostream>

struct Square{};
struct Triangle{};
struct Circle{};

std::map<std::string, int> Calls;

struct BlockA : public IBlock<BlockA, Square, Triangle>
{
	static const std::string& GetName() { static const std::string name = "A"; return name; }

	void OnReceive(const Square&)
	{
		FAIL();
	}
};

struct BlockB : public IBlock<BlockB, Triangle, Circle>
{
	static const std::string& GetName() { static const std::string name = "B"; return name; }

	void OnReceive(const Triangle&)
	{
		if (++Calls[GetName()] % 2 == 0)
		{
			Send({});
		}
	}
};

struct BlockC : public IBlock<BlockC, Circle, Square>
{
	static const std::string& GetName() { static const std::string name = "C"; return name; }

	void OnReceive(const Circle&)
	{
		++Calls[GetName()];
	}
};

struct BlockD : public IBlock<BlockD, Triangle, Triangle>
{
	static const std::string& GetName() { static const std::string name = "D"; return name; }

	void OnReceive(const Triangle&)
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

TEST_F(FlowTest, Basic)
{
	auto flow = mFactory.CreateFlow({"A", "B", "C"});

	for (const auto& block : flow)
	{
		std::cout << "block='" << block->GetBlockName() << "'" << std::endl;
	}
}

TEST_F(FlowTest, Valid)
{
	EXPECT_NO_THROW(mFactory.CreateFlow({"A"}));
	EXPECT_NO_THROW(mFactory.CreateFlow({"A", "B"}));
	EXPECT_NO_THROW(mFactory.CreateFlow({"B", "C"}));
	EXPECT_NO_THROW(mFactory.CreateFlow({"C", "A"}));
	EXPECT_NO_THROW(mFactory.CreateFlow({"D", "D"}));
	EXPECT_NO_THROW(mFactory.CreateFlow({"A", "D", "D", "B", "C", "A"}));
}

TEST_F(FlowTest, Invalid)
{
	EXPECT_THROW(mFactory.CreateFlow({"A", "A"}), std::runtime_error);
	EXPECT_THROW(mFactory.CreateFlow({"A", "C"}), std::runtime_error);
	EXPECT_THROW(mFactory.CreateFlow({"B", "A"}), std::runtime_error);
	EXPECT_THROW(mFactory.CreateFlow({"B", "B"}), std::runtime_error);
	EXPECT_THROW(mFactory.CreateFlow({"C", "B"}), std::runtime_error);
	EXPECT_THROW(mFactory.CreateFlow({"C", "C"}), std::runtime_error);
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
