#pragma once

#include <tuple>
#include <vector>

template <typename T, typename... Args>
struct TypeMap
{
	using TupleT = std::tuple<Args...>;

	template <typename K>
	const T& Get() const;

	template <typename K>
	T& Get();

private:
	std::vector<T> mElements;
};


// TODO impl
//
