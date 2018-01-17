#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <functional>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

const std::string config = "A-B";

struct IBlockBase
{
	virtual ~IBlockBase()
	{}

	virtual const std::string& GetBlockName() const =0;

	template <typename ObjectT>
	void Send(const ObjectT&)
	{
		//TODO
	}
};

template <typename BlockT, typename ObjectT>
struct IBlock : public IBlockBase
{
	virtual ~IBlock()
	{}

	const std::string& GetBlockName() const { return BlockT::GetName(); }

	virtual void OnReceive(const ObjectT&) =0;
};

struct Square{};
struct Triangle{};
struct Circle{};

struct BlockA : public IBlock<BlockA, Square>
{
	static const std::string& GetName() { static const std::string name = "A"; return name; }

	void OnReceive(const Square&) override
	{}
};

struct BlockB : public IBlock<BlockB, Triangle>
{
	static const std::string& GetName() { static const std::string name = "B"; return name; }

	void OnReceive(const Triangle&) override
	{}
};

struct BlockC : public IBlock<BlockC, Circle>
{
	void OnReceive(const Circle&) override
	{}
};

struct BlockD : public IBlock<BlockD, Triangle>
{
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
	bool Register()
	{
		auto p = mCreators.emplace(BlockT::GetName(), []() { return std::make_unique<BlockT>(); });
		return p.second;
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

	std::vector<std::unique_ptr<IBlockBase>> blocks;

	for (auto it = blockNames.crbegin(); it != blockNames.crend(); ++it)
	{
		using StringT = typename  StringListT::value_type;
		const StringT& blockName = *it;

		auto newBlock = factory.Create(blockName);
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
