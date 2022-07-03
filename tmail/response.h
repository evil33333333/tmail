#pragma once
#include <iostream>
#include <map>
#include <vector>
struct Response
{
	bool ok;
	std::string text;
	uint64_t status_code;
	std::map<std::string, std::string> headers;
	std::vector<std::string> cookies;
};
