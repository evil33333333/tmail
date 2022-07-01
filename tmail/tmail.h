#pragma once
#include "http.h"
#include "message.h"
#include "inbox.h"
#include <nlohmann/json.hpp>
#include <format>

class T_Mail
{
private:
	Request* request;
	std::map<std::string, std::string> headers;
	
public:
	class Client
	{
	public:
		bool ok = false;
		std::string password;
		std::string email;
		std::string token;
		std::string id;
		Request* request;

		void InitializeClient();


		Inbox GetInbox(
			const std::string* proxy = nullptr
		);

		Message OpenPreviewMessage(
			const PreviewMessage& previewMessage,
			const std::string* proxy = nullptr
		);
	private:
		
		bool is_initialized = false;
		std::map<std::string, std::string> headers;
		
	};

	T_Mail::Client* MakeClient(
		const std::string& username,
		const std::string& password,
		const std::string* proxy = nullptr
	);

	bool DeleteClient(
		T_Mail::Client* client,
		const std::string* proxy = nullptr
	);

	
};

