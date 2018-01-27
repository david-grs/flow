#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

struct IBlockBase
{
	virtual ~IBlockBase()
	{}

	virtual const std::string& GetBlockName() const =0;
};

template <typename InputT>
struct IBlockConsumer : public virtual IBlockBase
{
	virtual ~IBlockConsumer()
	{}

	virtual void OnReceive(const InputT&) =0;
};

template <typename OutputT>
struct IBlockProducer : public virtual IBlockBase
{
	virtual ~IBlockProducer()
	{}

	virtual void Send(const OutputT&) = 0;
};

template <typename BlockT, typename InputT, typename OutputT>
struct IBlock :
	public IBlockConsumer<InputT>,
	public IBlockProducer<OutputT>
{
	using block_type = BlockT;
	using input_type = InputT;
	using output_type = OutputT;

	explicit IBlock(IBlockBase* parent) :
		mParent(parent)
	{

	}

	virtual ~IBlock()
	{}

	const std::string& GetBlockName() const override { return BlockT::GetName(); }

	void Send(const OutputT&) override
	{
		// TODO
	}

private:
	IBlockBase* mParent;
};

template <typename ChildBlockT>
bool IsValidParent(IBlockBase* parent)
{
	using InputT = typename ChildBlockT::input_type;
	return dynamic_cast<IBlockProducer<InputT>*>(parent) != nullptr;
}

struct BlockCreator
{
	template <typename BlockT>
	std::unique_ptr<IBlockBase> Create(IBlockBase* parentBase)
	{
		if (!parentBase)
		{
			return std::make_unique<BlockT>();
		}

		if (!IsValidParent<BlockT>(parentBase))
		{
			throw std::runtime_error("invalid link " + parentBase->GetBlockName() + "->" + BlockT::GetName());
		}

		auto* parent = dynamic_cast<IBlockProducer<typename BlockT::input_type>*>(parentBase);
		return std::make_unique<BlockT>(parent);
	}
};

template <typename CreatorT>
struct BlockFactory
{
	std::unique_ptr<IBlockBase> Create(const std::string& name, IBlockBase* parent)
	{
		auto it = mCreators.find(name);
		if (it == mCreators.cend())
			throw std::runtime_error("unknown block name: " + name);

		return it->second(parent);
	}

	template <typename BlockT>
	void Register()
	{
		mCreators.emplace(BlockT::GetName(), [](IBlockBase* parent)
		{
			return CreatorT{}.template Create<BlockT>(parent);
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
		IBlockBase* parent = nullptr;
		std::vector<std::unique_ptr<IBlockBase>> blocks;

		for (const auto& blockName : blockNames)
		{
			auto newBlock = mFactory.Create(blockName, parent);
			parent = newBlock.get();

			blocks.push_back(std::move(newBlock));
		}

		return blocks;
	}

	std::vector<std::unique_ptr<IBlockBase>> CreateFlow(const std::vector<std::string>& blockNames)
	{
		return CreateFlow<std::vector<std::string>>(blockNames);
	}

private:
	BlockFactory<BlockCreator> mFactory;
};
