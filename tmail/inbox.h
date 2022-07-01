#pragma once
#include "message.h"
#include <vector>
#include <iostream>

struct Inbox
{
	bool ok = false;
	std::string context;
	std::string id;
	std::string collection_type;
	std::vector<PreviewMessage> messages;
	uint64_t total_items;
};