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

template <typename T, typename... Args>
template <typename K>
const T& TypeMap<T, Args...>::Get() const
{
	return mElements[0];
}

template <typename T, typename... Args>
template <typename K>
T& TypeMap<T, Args...>::Get()
{
	return mElements[0];
}

