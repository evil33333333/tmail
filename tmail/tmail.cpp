#include "tmail.h"

T_Mail::T_Mail()
{
	this->request = new Request();
	this->request->Initialize();
}


T_Mail::~T_Mail()
{
	delete this->request;
}


// Returns heap allocated
T_Mail::Client* T_Mail::MakeClient(const std::string& email, const std::string& password, const std::string* proxy = nullptr)
{
	T_Mail::Client* client = new T_Mail::Client();

	std::string data = "{\"address\":\"" + email + "\",\"password\":\"" + password + "\"}";
	std::map<std::string, std::string> headers = {
		{ "Content-Type", "application/json" },
		{ "Accept", "application/json" },
	};
	Response response = this->request->Post("https://api.mail.tm/accounts/", data, headers, proxy);
	if (!response.ok || response.status_code != 201)
	{
		return client;
	}
	client->email = email;
	client->password = password;

	response = this->request->Post("https://api.mail.tm/token/", data, headers, proxy);
	if (!response.ok || response.status_code != 200)
	{
		return client;
	}

	nlohmann::json j_response = nlohmann::json::parse(response.response_body);
	std::string token = j_response["token"].get<std::string>();
	std::string id = j_response["id"].get<std::string>();

	client->token = token;
	client->id = id;
	client->ok = true;
	client->request = this->request;

	this->headers = std::map<std::string, std::string>{
			{ "Content-Type", "application/json" },
			{ "Accept", "application/json" },
			{ "Authorization", std::format("Bearer {}", client->token) },
	};

	return client;
}

bool T_Mail::DeleteClient(T_Mail::Client* client, const std::string* proxy = nullptr)
{
	// if the clients token and id isnt empty, we delete the account from the db
	if (!client->token.empty() && !client->id.empty())
	{
		Response response = this->request->CustomRequest(
			std::format("https://api.mail.tm/accounts/{}/", client->id),
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
}


void T_Mail::Client::InitializeClient()
{
	if (this->is_initialized)
	{
		return;
	}
	this->headers = std::map<std::string, std::string>{
		{ "Content-Type", "application/json" },
		{ "Accept", "application/json" },
		{ "Authorization", std::format("Bearer {}", this->token) },
	};
}

struct Inbox T_Mail::Client::GetInbox(const std::string* proxy = nullptr)
{
	struct Inbox inbox;
	this->InitializeClient();

	Response response = this->request->Get("https://api.mail.tm/messages", this->headers, proxy);
	if (!response.ok || response.status_code != 200)
	{
		return inbox;
	}

	nlohmann::json j_response = nlohmann::json::parse(response.response_body);

	inbox.context = j_response["@context"].get<std::string>();
	inbox.id = j_response["@id"].get<std::string>();
	inbox.collection_type = j_response["@type"].get<std::string>();
	inbox.total_items = j_response["hydra:totalItems"].get<uint64_t>();
	for (uint64_t i = 0; i < inbox.total_items; i++)
	{
		struct PreviewMessage previewMessage;
		previewMessage.id_path = j_response[std::format("hydra:member.{}.@id", i)].get<std::string>();
		previewMessage.type = j_response[std::format("hydra:member.{}.@type", i)].get<std::string>();
		previewMessage.id = j_response[std::format("hydra:member.{}.id", i)].get<std::string>();
		previewMessage.account_id = j_response[std::format("hydra:member.{}.acccountId", i)].get<std::string>();
		previewMessage.msg_id = j_response[std::format("hydra:member.{}.msgid", i)].get<std::string>();
		previewMessage.subject = j_response[std::format("hydra:member.{}.subject", i)].get<std::string>();
		previewMessage.intro = j_response[std::format("hydra:member.{}.intro", i)].get<std::string>();
		previewMessage.seen = j_response[std::format("hydra:member.{}.seen", i)].get<bool>();
		previewMessage.is_deleted = j_response[std::format("hydra:member.{}.isDeleted", i)].get<bool>();
		previewMessage.has_attachments = j_response[std::format("hydra:member.{}.hasAttachments", i)].get<bool>();
		previewMessage.size = j_response[std::format("hydra:member.{}.size", i)].get<uint64_t>();
		previewMessage.download_url = j_response[std::format("hydra:member.{}.download_url", i)].get<std::string>();
		previewMessage.creation_date = j_response[std::format("hydra:member.{}.createdAt", i)].get<std::string>();
		previewMessage.updated_date = j_response[std::format("hydra:member.{}.updatedAt", i)].get<std::string>();

		previewMessage.from.address = j_response[std::format("hydra:member.{}.from.address", i)].get<std::string>();
		previewMessage.from.name = j_response[std::format("hydra:member.{}.from.name", i)].get<std::string>();

		PreviewMessage::To to;
		to.address = j_response[std::format("hydra:member.{}.to.0.address", i)].get<std::string>();
		to.name = j_response[std::format("hydra:member.{}.to.0.name", i)].get<std::string>();
		previewMessage.to.push_back(to);

		inbox.messages.push_back(previewMessage);
	}

	
	inbox.ok = true;
	return inbox;
	
}

struct Message T_Mail::Client::OpenPreviewMessage(const PreviewMessage& previewMessage, const std::string* proxy = nullptr)
{
	struct Message message;
	this->InitializeClient();
	Response response = this->request->Get(std::format("https://api.mail.tm/{}", previewMessage.id_path), this->headers, proxy);
	if (!response.ok || response.status_code != 200)
	{
		return message;
	}

	nlohmann::json j_response = nlohmann::json::parse(response.response_body);

	message.id_path = j_response["@id"].get<std::string>();
	message.type = j_response["@type"].get<std::string>();
	message.id = j_response["id"].get<std::string>();
	message.account_id = j_response["acccountId"].get<std::string>();
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

	message.from.address = j_response["from.address"].get<std::string>();
	message.from.name = j_response["from.name"].get<std::string>();

	Message::To to;
	to.address = j_response["to.0.address"].get<std::string>();
	to.name = j_response["to.0.name"].get<std::string>();
	message.to.push_back(to);

	message.html = j_response["html.0"].get<std::string>();
	
	return message;

}