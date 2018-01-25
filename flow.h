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

template <typename OutputT>
struct IBlockProducer : public IBlockBase
{
	virtual ~IBlockProducer()
	{}
};

template <typename BlockT, typename InputT, typename OutputT>
struct IBlock : public IBlockProducer<OutputT>
{
	using block_type = BlockT;
	using input_type = InputT;
	using output_type = OutputT;

	virtual ~IBlock()
	{}

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
	explicit BlockA(IBlockBase*)
	{}

	static const std::string& GetName() { static const std::string name = "A"; return name; }

	void OnReceive(const Square&) override
	{}
};

struct BlockB : public IBlock<BlockB, Triangle, Circle>
{
	explicit BlockB(IBlockBase*)
	{}

	static const std::string& GetName() { static const std::string name = "B"; return name; }

	void OnReceive(const Triangle&) override
	{}
};

struct BlockC : public IBlock<BlockC, Circle, Square>
{
	explicit BlockC(IBlockBase*)
	{}

	static const std::string& GetName() { static const std::string name = "C"; return name; }

	void OnReceive(const Circle&) override
	{}
};

struct BlockD : public IBlock<BlockD, Triangle, Triangle>
{
	explicit BlockD(IBlockBase*)
	{}

	static const std::string& GetName() { static const std::string name = "D"; return name; }

	void OnReceive(const Triangle&) override
	{}
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
	std::unique_ptr<IBlockBase> Create(IBlockBase* parent)
	{
		if (parent && !IsValidParent<BlockT>(parent))
		{
			throw std::runtime_error("invalid link " + parent->GetBlockName() + "->" + BlockT::GetName());
		}

		std::unique_ptr<BlockT> blk = std::make_unique<BlockT>(parent);
		return blk;
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

template <typename StringListT>
std::vector<std::unique_ptr<IBlockBase>> CreateFlow(const StringListT& blockNames)
{
	BlockFactory<BlockCreator> factory;
	factory.Register<BlockA>();
	factory.Register<BlockB>();
	factory.Register<BlockC>();
	factory.Register<BlockD>();

	IBlockBase* parent = nullptr;
	std::vector<std::unique_ptr<IBlockBase>> blocks;

	for (const auto& blockName : blockNames)
	{
		auto newBlock = factory.Create(blockName, parent);
		parent = newBlock.get();

		blocks.push_back(std::move(newBlock));
	}

	return blocks;
}

std::vector<std::unique_ptr<IBlockBase>> CreateFlow(const std::vector<std::string>& blockNames)
{
	return CreateFlow<std::vector<std::string>>(blockNames);
}
