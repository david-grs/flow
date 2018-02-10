#include "block_factory.h"

#include <vector>
#include <memory>
#include <algorithm>

using Flow = std::vector<std::unique_ptr<IBlockBase>>;

struct FlowFactory
{
	template <typename BlockT>
	void Register()
	{
		mFactory.template Register<BlockT>();
	}

	template <typename StringListT>
	Flow CreateFlow(const StringListT& blockNames)
	{
		IBlockBase* child = nullptr;
		std::vector<std::unique_ptr<IBlockBase>> blocks;

		for (auto it = blockNames.crbegin(); it != blockNames.crend(); ++it)
		{
			using StringT = typename StringListT::value_type;
			const StringT& blockName = *it;

			auto newBlock = mFactory.Create(blockName, child);
			child = newBlock.get();

			blocks.push_back(std::move(newBlock));
		}

		std::reverse(blocks.begin(), blocks.end());

		{
			auto* firstBlock = blocks[0].get();

			if (!dynamic_cast<IBlockConsumer<void>*>(firstBlock))
				throw std::runtime_error("unexpected first block");
		}
		return blocks;
	}

	Flow CreateFlow(const std::vector<std::string>& blockNames)
	{
		return CreateFlow<std::vector<std::string>>(blockNames);
	}

private:
	BlockFactory<BlockCreator> mFactory;
};
