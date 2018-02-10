flow
====
Simple workflow class

```
FlowFactory factory;
factory.Register<BlockA>();
factory.Register<BlockB>();
factory.Register<BlockC>();

auto flow = factory.CreateFlow({"A", "B", "C"});
flow.Run();
```
