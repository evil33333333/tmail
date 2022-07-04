#pragma once
#include <vector>
#include <iostream>


struct Message
{
	struct From
	{
		std::string address;
		std::string name;
	};

	struct To
	{
		std::string address;
		std::string name;
	};
	
	std::string html;
	std::string id_path;
	std::string id;
	std::string account_id;
	std::string msg_id;
	Message::From from;
	std::vector<Message::To> to;
	std::string subject;
	bool seen;
	bool is_deleted;
	bool is_flagged;
	bool has_attachments;
	uint64_t size;
	std::string download_url;
	std::string creation_date;
	std::string updated_date;
	bool ok = false;
};

struct PreviewMessage
{
	struct From
	{
		std::string address;
		std::string name;
	};

	struct To
	{
		std::string address;
		std::string name;
	};
	std::string id;
	std::string account_id;
	std::string msg_id;
	PreviewMessage::From from;
	std::vector<PreviewMessage::To> to;
	std::string subject;
	std::string intro;
	bool seen;
	bool is_deleted;
	bool has_attachments;
	uint64_t size;
	std::string download_url;
	std::string creation_date;
	std::string updated_date;
	bool ok = false;
};
