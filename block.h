#include <string>
#include <cassert>

struct IBlockBase
{
	virtual ~IBlockBase()
	{}

	virtual std::string GetBlockName() const =0;
};

template <typename InputT>
struct IBlockConsumer : virtual public IBlockBase
{
	virtual ~IBlockConsumer()
	{}

	virtual void Process(const InputT&) =0;
};

template <>
struct IBlockConsumer<void> : virtual public IBlockBase
{
	virtual ~IBlockConsumer()
	{}

	virtual void Process() { }
};

template <typename OutputT>
struct IBlockProducer : virtual public IBlockBase
{
	virtual ~IBlockProducer()
	{}

	void SetChildBlock(IBlockConsumer<OutputT>* child)
	{
		assert(!mChild && child);
		mChild = child;
	}

	void Send(const OutputT& t)
	{
		if (!mChild)
		{
			assert(false);
			return;
		}

		mChild->Process(t);
	}

private:
	IBlockConsumer<OutputT>* mChild = nullptr;
};

template <>
struct IBlockProducer<void> : virtual public IBlockBase
{
	virtual ~IBlockProducer()
	{}
};

template <typename BlockT, typename InputT, typename OutputT>
struct IBlock :
	public IBlockConsumer<InputT>,
	public IBlockProducer<OutputT>
{
	using block_type = BlockT;
	using input_type = InputT;
	using output_type = OutputT;

	virtual ~IBlock()
	{}

	std::string GetBlockName() const override { return BlockT::GetName(); }
};

