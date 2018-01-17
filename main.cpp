#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

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

template <typename StringT>
std::unique_ptr<IBlockBase> createBlock(const StringT& name)
{
	return {};
}

template <typename StringListT>
std::vector<std::unique_ptr<IBlockBase>> createFlow(const StringListT& blockNames)
{
	std::vector<std::unique_ptr<IBlockBase>> blocks;

	for (auto it = blockNames.crbegin(); it != blockNames.crend(); ++it)
	{
		using StringT = typename  StringListT::value_type;
		const StringT& blockName = *it;

		auto newBlock = createBlock(blockName);
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

	return 0;
}
