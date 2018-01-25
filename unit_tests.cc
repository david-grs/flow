#include "flow.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <iostream>

struct Square{};
struct Triangle{};
struct Circle{};

struct BlockA : public IBlock<BlockA, Square, Triangle>
{
	explicit BlockA(IBlockBase*)
	{}

	static const std::string& GetName() { static const std::string name = "A"; return name; }

	void OnReceive(const Square&) override
	{}
};

struct BlockB : public IBlock<BlockB, Triangle, Circle>
{
	explicit BlockB(IBlockBase*)
	{}

	static const std::string& GetName() { static const std::string name = "B"; return name; }

	MOCK_METHOD0(OnReceive, void());

	void OnReceive(const Triangle&) override
	{
		OnReceive();
	}
};

struct BlockC : public IBlock<BlockC, Circle, Square>
{
	explicit BlockC(IBlockBase*)
	{}

	static const std::string& GetName() { static const std::string name = "C"; return name; }

	void OnReceive(const Circle&) override
	{}
};

struct BlockD : public IBlock<BlockD, Triangle, Triangle>
{
	explicit BlockD(IBlockBase*)
	{}

	static const std::string& GetName() { static const std::string name = "D"; return name; }

	void OnReceive(const Triangle&) override
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

	//std::unique_ptr<IBlockBase> blkA = flow[0];
	//std::unique_ptr<IBlockBase> blkB = flow[1];

	//EXPECT_CALL(blkA.get(), OnReceive());
}
