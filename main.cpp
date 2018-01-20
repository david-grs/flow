#include "typemap.hpp"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <functional>

const std::string config = "A-B";

struct IBlockBase
{
	virtual ~IBlockBase()
	{}

	virtual bool IsValidParent(IBlockBase*) const { return false; }
	virtual const std::string& GetBlockName() const =0;
};

template <typename OutputT>
struct IBlockSender : public IBlockBase
{
};

template <typename BlockT, typename InputT, typename OutputT>
struct IBlock : public IBlockSender<OutputT>
{
	using block_type = BlockT;
	using input_type = InputT;
	using output_type = OutputT;

	virtual ~IBlock()
	{}

	bool IsValidParent(IBlockBase* blk) const override final { return dynamic_cast<IBlockSender<InputT>*>(blk); }
	const std::string& GetBlockName() const { return BlockT::GetName(); }

	virtual void OnReceive(const InputT&) =0;

	void Send(const OutputT&)
	{
		// TODO
	}
};

struct Square{};
struct Triangle{};
struct Circle{};

struct BlockA : public IBlock<BlockA, Square, Triangle>
{
	static const std::string& GetName() { static const std::string name = "A"; return name; }

	void OnReceive(const Square&) override
	{}
};

struct BlockB : public IBlock<BlockB, Triangle, Circle>
{
	static const std::string& GetName() { static const std::string name = "B"; return name; }

	void OnReceive(const Triangle&) override
	{}
};

struct BlockC : public IBlock<BlockC, Circle, Square>
{
	static const std::string& GetName() { static const std::string name = "C"; return name; }

	void OnReceive(const Circle&) override
	{}
};

struct BlockD : public IBlock<BlockD, Triangle, Triangle>
{
	static const std::string& GetName() { static const std::string name = "D"; return name; }

	void OnReceive(const Triangle&) override
	{}
};

template <typename... Args>
struct BlockFactory
{
	template <typename ProduceT>
	std::unique_ptr<IBlockBase> CreateProducer(const std::string& name)
	{
		// TODO take mCreators from typemap ProduceT

		auto it = mCreators.find(name);
		if (it == mCreators.cend())
			throw std::runtime_error("unknown block name: " + name);

		return it->second();
	}

	std::unique_ptr<IBlockBase> CreateLeaf(const std::string& name)
	{
		// TODO take mCreators from typemap ProduceT

		auto it = mCreators.find(name);
		if (it == mCreators.cend())
			throw std::runtime_error("unknown block name: " + name);

		return it->second();
	}

	template <typename BlockT>
	bool Register()
	{
		using OutputType = typename BlockT::output_type;

		auto& creators = mCreators.template Get<OutputType>();

		auto p = creators.emplace(BlockT::GetName(), []() { return std::make_unique<BlockT>(); });
		return p.second;
	}

private:
	using BlockCreator = std::function<std::unique_ptr<IBlockBase>()>;

	TypeMap<std::map<std::string, BlockCreator>, Args...> mCreators;
};

template <typename StringListT>
std::vector<std::unique_ptr<IBlockBase>> CreateFlow(const StringListT& blockNames)
{
	BlockFactory<Square, Triangle, Circle> factory;
	factory.Register<BlockA>();
	factory.Register<BlockB>();
	factory.Register<BlockC>();
	factory.Register<BlockD>();

	using StringT = typename  StringListT::value_type;

	std::vector<std::unique_ptr<IBlockBase>> blocks;

	auto it = blockNames.crbegin();
	//const StringT& blockName = *it;

	//auto newBlock = factory.CreateLeaf(blockName);
	//blocks.push_back(std::move(newBlock));

	for (; it != blockNames.crend(); ++it)
	{

	}

	std::reverse(blocks.begin(), blocks.end());
	return blocks;
}

int main()
{
	IBlockBase* blkA = new BlockA;
	IBlockBase* blkB = new BlockB;
	IBlockBase* blkC = new BlockC;

	std::cout << std::boolalpha << blkC->IsValidParent(blkA) << std::endl;



#if 0
	std::vector<std::string> nodes;
	boost::split(nodes, config, boost::is_any_of("-"));

	auto flow = CreateFlow(nodes);

	for (const auto& block : flow)
	{
		std::cout << "elem='" << block->GetBlockName() << "'" << std::endl;
	}
#endif
	return 0;
}
