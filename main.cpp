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

struct BlockFactory
{
	std::unique_ptr<IBlockBase> Create(const std::string& name)
	{
		auto it = mCreators.find(name);
		if (it == mCreators.cend())
			throw std::runtime_error("unknown block name: " + name);

		return it->second();
	}

	template <typename BlockT>
	void Register()
	{
		mCreators.emplace(BlockT::GetName(), []() { return std::make_unique<BlockT>(); });
	}

private:
	using BlockCreator = std::function<std::unique_ptr<IBlockBase>()>;

	std::map<std::string, BlockCreator> mCreators;
};

template <typename StringListT>
std::vector<std::unique_ptr<IBlockBase>> CreateFlow(const StringListT& blockNames)
{
	BlockFactory factory;
	factory.Register<BlockA>();
	factory.Register<BlockB>();
	factory.Register<BlockC>();
	factory.Register<BlockD>();

	std::vector<std::unique_ptr<IBlockBase>> blocks;

	for (auto it = blockNames.crbegin(); it != blockNames.crend(); ++it)
	{
		using StringT = typename  StringListT::value_type;
		const StringT& blockName = *it;

		auto newBlock = factory.Create(blockName);

		if (!blocks.empty() && !blocks.back()->IsValidParent(newBlock.get()))
		{
			throw std::runtime_error("invalid link " + newBlock->GetBlockName() + "->" + blocks.back()->GetBlockName());
		}

		blocks.push_back(std::move(newBlock));
	}

	std::reverse(blocks.begin(), blocks.end());
	return blocks;
}

int main()
{
	std::vector<std::string> nodes;
	boost::split(nodes, config, boost::is_any_of("-"));

	auto flow = CreateFlow(nodes);

	for (const auto& block : flow)
	{
		std::cout << "elem='" << block->GetBlockName() << "'" << std::endl;
	}

	return 0;
}
