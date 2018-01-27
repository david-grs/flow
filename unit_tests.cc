#include "flow.h"

#include "gtest/gtest.h"

#include <iostream>

struct Square{};
struct Triangle{};
struct Circle{};

// TODO gmock
static int bCalls = {};

struct BlockA : public IBlock<BlockA, Square, Triangle>
{
	explicit BlockA(IBlockProducer<Square>* parent = nullptr) :
		IBlock<BlockA, Square, Triangle>(parent)
	{}

	static const std::string& GetName() { static const std::string name = "A"; return name; }

	void OnReceive(const Square&)
	{}
};

struct BlockB : public IBlock<BlockB, Triangle, Circle>
{
	explicit BlockB(IBlockProducer<Triangle>* parent = nullptr) :
		IBlock<BlockB, Triangle, Circle>(parent)
	{}

	static const std::string& GetName() { static const std::string name = "B"; return name; }

	void OnReceive(const Triangle&)
	{
		++bCalls;
	}
};

struct BlockC : public IBlock<BlockC, Circle, Square>
{
	explicit BlockC(IBlockProducer<Circle>* parent = nullptr) :
		IBlock<BlockC, Circle, Square>(parent)
	{}

	static const std::string& GetName() { static const std::string name = "C"; return name; }

	void OnReceive(const Circle&)
	{}
};

struct BlockD : public IBlock<BlockD, Triangle, Triangle>
{
	explicit BlockD(IBlockProducer<Triangle>* parent = nullptr) :
		IBlock<BlockD, Triangle, Triangle>(parent)
	{}

	static const std::string& GetName() { static const std::string name = "D"; return name; }

	void OnReceive(const Triangle&)
	{}
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
	auto flow = mFactory.CreateFlow({"A", "B"});

	BlockA* blkA = dynamic_cast<BlockA*>(flow[0].get());

	EXPECT_EQ(0, bCalls);
	blkA->Send({});

	EXPECT_EQ(1, bCalls);
	blkA->Send({});

	EXPECT_EQ(2, bCalls);
}
