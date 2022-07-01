#pragma once
#include <iostream>
#include <map>
#include <vector>
struct Response
{
	bool ok;
	std::string response_body;
	int status_code;
	std::map<std::string, std::string> headers;
	std::vector<std::string> cookies;
	std::string raw_response;
};
