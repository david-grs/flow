#include "block.h"

#include <memory>
#include <cassert>
#include <map>
#include <functional>

template <typename BlockT>
bool IsValidChild(IBlockBase* child)
{
	using OutputT = typename BlockT::output_type;
	return dynamic_cast<IBlockConsumer<OutputT>*>(child) != nullptr;
}

struct BlockCreator
{
	template <typename BlockT>
	std::unique_ptr<IBlockBase> Create()
	{
		return std::make_unique<BlockT>();
	}

	template <typename BlockT>
	std::unique_ptr<IBlockBase> Create(IBlockBase* childBase)
	{
		if (!IsValidChild<BlockT>(childBase))
		{
			throw std::runtime_error("invalid link " + BlockT::GetName() + "->" + childBase->GetBlockName());
		}

		std::unique_ptr<BlockT> newBlock = std::make_unique<BlockT>();

		auto* child = dynamic_cast<IBlockConsumer<typename BlockT::output_type>*>(childBase);
		newBlock->SetChildBlock(child);

		return newBlock;
	}
};

template <typename CreatorT>
struct BlockFactory
{
	std::unique_ptr<IBlockBase> Create(const std::string& name, IBlockBase* child)
	{
		auto it = mCreators.find(name);
		if (it == mCreators.cend())
			throw std::runtime_error("unknown block name: " + name);

		return it->second(child);
	}

	template <typename BlockT>
	std::enable_if_t<std::is_same<typename BlockT::output_type, void>::value> Register()
	{
		mCreators.emplace(BlockT::GetName(), [](IBlockBase* child)
		{
			if (child)
			{
				throw std::runtime_error("unexpected child");
			}

			return CreatorT{}.template Create<BlockT>();
		});
	}

	template <typename BlockT>
	std::enable_if_t<!std::is_same<typename BlockT::output_type, void>::value> Register()
	{
		mCreators.emplace(BlockT::GetName(), [](IBlockBase* child)
		{
			return CreatorT{}.template Create<BlockT>(child);
		});
	}

private:
	using BlockCreator = std::function<std::unique_ptr<IBlockBase>(IBlockBase*)>;

	std::map<std::string, BlockCreator> mCreators;
};
