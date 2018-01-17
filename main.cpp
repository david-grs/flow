#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

const std::string config = "A-B";

int main()
{
	std::vector<std::string> nodes;
	boost::split(nodes, config, boost::is_any_of("-"));

	for (const auto& node : nodes)
	{
		std::cout << "elem='" << node << "'" << std::endl;
	}

	return 0;
}
