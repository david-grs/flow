#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <functional>
#include <cassert>

struct IBlockBase
{
	virtual ~IBlockBase()
	{}

	virtual const std::string& GetBlockName() const =0;
};

template <typename InputT>
struct IBlockConsumer : public IBlockBase
{
	virtual ~IBlockConsumer()
	{}

	virtual void OnReceive(const InputT&) =0;
};

template <typename BlockT, typename InputT, typename OutputT>
struct IBlock :
	public IBlockConsumer<InputT>
{
	using block_type = BlockT;
	using input_type = InputT;
	using output_type = OutputT;

	virtual ~IBlock()
	{}

	const std::string& GetBlockName() const override { return BlockT::GetName(); }

	void SetChildBlock(IBlockConsumer<OutputT>* child)
	{
		assert(mChild);
		mChild = child;
	}

	void Send(const OutputT& t)
	{
		mChild->OnReceive(t);
	}

private:
	IBlockConsumer<OutputT>* mChild = nullptr;
};

template <typename BlockT>
bool IsValidChild(IBlockBase* child)
{
	using OutputT = typename BlockT::output_type;
	return dynamic_cast<IBlockConsumer<OutputT>*>(child) != nullptr;
}

struct BlockCreator
{
	template <typename BlockT>
	std::unique_ptr<IBlockBase> Create(IBlockBase* childBase)
	{
		if (!childBase)
		{
			return std::make_unique<BlockT>();
		}

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
	void Register()
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

struct FlowFactory
{
	template <typename BlockT>
	void Register()
	{
		mFactory.template Register<BlockT>();
	}

	template <typename StringListT>
	std::vector<std::unique_ptr<IBlockBase>> CreateFlow(const StringListT& blockNames)
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
		return blocks;
	}

	std::vector<std::unique_ptr<IBlockBase>> CreateFlow(const std::vector<std::string>& blockNames)
	{
		return CreateFlow<std::vector<std::string>>(blockNames);
	}

private:
	BlockFactory<BlockCreator> mFactory;
};
