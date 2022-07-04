#include "tmail.h"

// Returns heap allocated memory
T_Mail::Client* T_Mail::MakeClient(const std::string& email, const std::string& password, const std::string* proxy )
{
	T_Mail::Client* client = new T_Mail::Client();

	std::string data = "{\"address\": \"" + email + "\", \"password\": \"" + password + "\"}";
	std::map<std::string, std::string> headers = {
		{ "Host", "api.mail.tm" },
		{ "Content-Length", std::to_string(data.size()).c_str() },
		{ "Content-Type", "application/json" },
		{ "Connection", "keep-alive" },
		{ "Accept", "application/json" },
	};
	Response response = this->request->Post("https://api.mail.tm/accounts", data, headers, proxy);
	if (!response.ok || response.status_code != 201)
	{
		return client;
	}
	client->email = email;
	client->password = password;

	response = this->request->Post("https://api.mail.tm/token", data, headers, proxy);
	if (!response.ok || response.status_code != 200)
	{
		return client;
	}
	nlohmann::json j_response = nlohmann::json::parse(response.text);
	std::string token = j_response["token"].get<std::string>();
	std::string id = j_response["id"].get<std::string>();

	client->token = token;
	client->id = id;
	client->ok = true;
	client->request = new Request();
	client->request->Initialize();

	this->headers = std::map<std::string, std::string>{
			{ "Host", "api.mail.tm" },
			{ "Connection", "keep-alive" },
			{ "Content-Type", "application/json" },
			{ "Accept", "application/json" },
			{ "Authorization", std::format("Bearer {}", client->token) },
	};


	client->headers = std::map<std::string, std::string>{
			{ "Host", "api.mail.tm" },
			{ "Connection", "keep-alive" },
			{ "Content-Type", "application/json" },
			{ "Accept", "application/json" },
			{ "Authorization", std::format("Bearer {}", client->token) },
	};
	

	return client;
}

bool T_Mail::DeleteClient(T_Mail::Client* client, const std::string* proxy )
{
	// if the clients token and id isnt empty, we delete the account from the db
	if (!client->token.empty() && !client->id.empty())
	{
		Response response = this->request->CustomRequest(
			std::format("https://api.mail.tm/accounts/{}", client->id),
			"",
			this->headers,
			"DELETE",
			proxy
		);

		// Check if we deleted the email account
		if (!response.ok || response.status_code != 204)
		{
			return false;
		}
	}
	

	// If we did, continue with freeing the memory
	delete client;
	return true;
}


struct Inbox T_Mail::Client::GetInbox(const std::string* proxy)
{
	struct Inbox inbox;
	uint32_t iterator = 0;
	Response response = this->request->Get("https://api.mail.tm/messages", this->headers, proxy);
	if (response.status_code != 200)
	{
		return inbox;
	}

	nlohmann::json j_response = nlohmann::json::parse(response.text);

	while (true)
	{
		try
		{
			struct PreviewMessage previewMessage;
			previewMessage.id = j_response[iterator]["id"].get<std::string>();
			previewMessage.account_id = j_response[iterator]["accountId"].get<std::string>();
			previewMessage.msg_id = j_response[iterator]["msgid"].get<std::string>();
			previewMessage.subject = j_response[iterator]["subject"].get<std::string>();
			previewMessage.intro = j_response[iterator]["intro"].get<std::string>();
			previewMessage.seen = j_response[iterator]["seen"].get<bool>();
			previewMessage.is_deleted = j_response[iterator]["isDeleted"].get<bool>();
			previewMessage.has_attachments = j_response[iterator]["hasAttachments"].get<bool>();
			previewMessage.size = j_response[iterator]["size"].get<uint64_t>();
			previewMessage.download_url = j_response[iterator]["downloadUrl"].get<std::string>();
			previewMessage.creation_date = j_response[iterator]["createdAt"].get<std::string>();
			previewMessage.updated_date = j_response[iterator]["updatedAt"].get<std::string>();

			previewMessage.from.address = j_response[iterator]["from"]["address"].get<std::string>();
			previewMessage.from.name = j_response[iterator]["from"]["name"].get<std::string>();

			PreviewMessage::To to;
			to.address = j_response[iterator]["to"][0]["address"].get<std::string>();
			to.name = j_response[iterator]["to"][0]["name"].get<std::string>();
			previewMessage.to.push_back(to);

			inbox.preview_messages.push_back(previewMessage);
			iterator++;
		}
		catch (const std::exception&) 
		{
			break;
		}
	}

	
	
	inbox.ok = true;
	return inbox;
	
}

struct Message T_Mail::Client::OpenPreviewMessage(const PreviewMessage& previewMessage, const std::string* proxy)
{
	struct Message message;
	Response response = this->request->Get(std::format("https://api.mail.tm/messages/{}", previewMessage.id), this->headers, proxy);
	if (!response.ok || response.status_code != 200)
	{
		return message;
	}

	nlohmann::json j_response = nlohmann::json::parse(response.text);

	message.id = j_response["id"].get<std::string>();
	message.account_id = j_response["accountId"].get<std::string>();
	message.msg_id = j_response["msgid"].get<std::string>();
	message.subject = j_response["subject"].get<std::string>();
	message.seen = j_response["seen"].get<bool>();
	message.is_deleted = j_response["isDeleted"].get<bool>();
	message.has_attachments = j_response["hasAttachments"].get<bool>();
	message.size = j_response["size"].get<uint64_t>();
	message.download_url = j_response["downloadUrl"].get<std::string>();
	message.creation_date = j_response["createdAt"].get<std::string>();
	message.updated_date = j_response["updatedAt"].get<std::string>();
	message.is_flagged = j_response["flagged"].get<bool>();

	message.from.address = j_response["from"]["address"].get<std::string>();
	message.from.name = j_response["from"]["name"].get<std::string>();

	Message::To to;
	to.address = j_response["to"][0]["address"].get<std::string>();
	to.name = j_response["to"][0]["name"].get<std::string>();
	message.to.push_back(to);

	message.html = j_response["html"][0].get<std::string>();
	
	return message;

}

void T_Mail::Initialize()
{
	this->request = new Request;
	this->request->Initialize();

}
