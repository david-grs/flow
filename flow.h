#include "block_factory.h"

#include <vector>
#include <memory>
#include <algorithm>

struct Flow
{
	explicit Flow(std::vector<std::unique_ptr<IBlockBase>>&& blocks) :
		mBlocks(std::move(blocks))
	{
		mFirstBlock = dynamic_cast<IBlockConsumer<void>*>(mBlocks[0].get());

		if (!mFirstBlock)
		{
			throw std::runtime_error("unexpected first block");
		}
	}

	void Run()
	{
		mFirstBlock->Process();
	}

	std::size_t size() const { return mBlocks.size(); }
	std::unique_ptr<IBlockBase>& operator[](std::size_t i) { return mBlocks[i]; }

private:
	std::vector<std::unique_ptr<IBlockBase>> mBlocks;
	IBlockConsumer<void>* mFirstBlock;
};

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
		return Flow(std::move(blocks));
	}

	Flow CreateFlow(const std::vector<std::string>& blockNames)
	{
		return CreateFlow<std::vector<std::string>>(blockNames);
	}

private:
	BlockFactory<BlockCreator> mFactory;
};
