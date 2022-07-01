#pragma once
#include "response.h"
#include <curl/curl.h>
#include <iostream>
#include <map>

class Request
{
private:
	bool is_initialized = false;

public:

	void Initialize();

	Response Get(
		const std::string& url,
		const std::map<std::string, std::string>& headers,
		const std::string* proxy = nullptr
	);

	Response Post(
		const std::string& url,
		const std::string& data,
		const std::map<std::string, std::string>& headers,
		const std::string* proxy = nullptr
	);

	Response CustomRequest(
		const std::string& url,
		const std::string& data,
		const std::map<std::string, std::string>& headers,
		const std::string& request_type,
		const std::string* proxy = nullptr
	);
};
