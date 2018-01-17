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

	template <typename ObjectT>
	void Send(const ObjectT&)
	{
		//TODO
	}
};

template <typename ObjectT>
struct IBlock : public IBlockBase
{
	virtual ~IBlock()
	{}

	virtual void OnReceive(const ObjectT&) =0;
};

struct Square{};
struct Triangle{};
struct Circle{};

struct BlockA : public IBlock<Square>
{
	void OnReceive(const Square&) override
	{}
};

struct BlockB : public IBlock<Triangle>
{
	void OnReceive(const Triangle&) override
	{}
};

struct BlockC : public IBlock<Circle>
{
	void OnReceive(const Circle&) override
	{}
};

struct BlockD : public IBlock<Triangle>
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
	bool Register(const std::string& name)
	{
		auto p = mCreators.emplace(name, []() { return std::make_unique<BlockT>(); });
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
	factory.Register<BlockA>("A");
	factory.Register<BlockB>("B");

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

	for (const auto& node : nodes)
	{
		std::cout << "elem='" << node << "'" << std::endl;
	}

	auto flow = CreateFlow(nodes);

	return 0;
}
