#include "http.h"

size_t write_to_string(void* ptr, size_t size, size_t count, void* stream);


void Request::Initialize()
{
	if (this->is_initialized)
	{
		return;
	}
	(void)curl_global_init(CURL_GLOBAL_ALL);
	this->is_initialized = true;
}

struct Response Request::Get(const std::string& url, const std::map<std::string, std::string>& headers, const std::string* proxy)
{
	uint64_t status_code{};
	std::string response_holder;
	std::string header_holder;
	struct Response response;
	this->Initialize();
	CURL* curl = curl_easy_init();
	if (curl)
	{
		struct curl_slist* curl_headers = nullptr;
		for (auto const& item : headers)
		{
			curl_headers = curl_slist_append(curl_headers, std::string(item.first + ": " + item.second).c_str());
		}
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_headers);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_holder);
		curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
		if (proxy)
		{
			curl_easy_setopt(curl, CURLOPT_PROXY, proxy->c_str());
		}
		curl_easy_setopt(curl, CURLOPT_WRITEHEADER, &header_holder);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_to_string);
		CURLcode res = curl_easy_perform(curl);
		if (res == CURLE_OK)
		{
			response.ok = true;
			struct curl_slist* cookies = nullptr;
			res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
			if (!res && cookies)
			{
				struct curl_slist* each = cookies;
				std::vector<std::string> _cookies;
				while (each)
				{
					_cookies.push_back(std::string(each->data));
					each = each->next;
				}
				/* we must free these cookies when we are done */
				curl_slist_free_all(cookies);
				response.cookies = _cookies;
			}
		}
		else
		{
			response.ok = false;
		}
		curl_slist_free_all(curl_headers);
	}
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
	curl_easy_cleanup(curl);
	response.text = response_holder;
	response.status_code = status_code;
	return response;

};

struct Response Request::Post(const std::string& url, const std::string& data, const std::map<std::string, std::string>& headers, const std::string* proxy)
{
	uint64_t status_code{};
	std::string response_holder;
	std::string header_holder;
	struct Response response;
	this->Initialize();
	CURL* curl = curl_easy_init();
	if (curl)
	{
		struct curl_slist* curl_headers = nullptr;
		for (auto const& item : headers)
		{
			curl_headers = curl_slist_append(curl_headers, std::string(item.first + ": " + item.second).c_str());
		}
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_headers);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
		if (proxy)
		{
			curl_easy_setopt(curl, CURLOPT_PROXY, proxy->c_str());
		}
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_holder);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_to_string);
		curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
		curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
		curl_easy_setopt(curl, CURLOPT_WRITEHEADER, &header_holder);
		CURLcode res = curl_easy_perform(curl);
		if (res == CURLE_OK)
		{
			response.ok = true;
			struct curl_slist* cookies = nullptr;
			res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
			if (!res && cookies)
			{
				struct curl_slist* each = cookies;
				std::vector<std::string> _cookies;
				while (each)
				{
					_cookies.push_back(std::string(each->data));
					each = each->next;
				}
				curl_slist_free_all(cookies);
				response.cookies = _cookies;
			}
		}
		else
		{
			response.ok = false;
		}
	}

	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
	curl_easy_cleanup(curl);
	response.text = response_holder;
	response.status_code = status_code;
	return response;
}


struct Response Request::CustomRequest(const std::string& url, const std::string& data, const std::map<std::string, std::string>& headers, const std::string& request_type, const std::string* proxy)
{
	uint64_t status_code{};
	std::string response_holder;
	std::string header_holder;
	struct Response response;
	this->Initialize();
	CURL* curl = curl_easy_init();
	if (curl)
	{
		struct curl_slist* curl_headers = nullptr;
		for (auto const& item : headers)
		{
			curl_headers = curl_slist_append(curl_headers, std::string(item.first + ": " + item.second).c_str());
		}
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, request_type.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_headers);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
		if (proxy)
		{
			curl_easy_setopt(curl, CURLOPT_PROXY, proxy->c_str());
		}
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_holder);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_to_string);
		curl_easy_setopt(curl, CURLOPT_WRITEHEADER, &header_holder);
		CURLcode res = curl_easy_perform(curl);
		if (res == CURLE_OK)
		{
			response.ok = true;
			struct curl_slist* cookies = nullptr;
			res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
			if (!res && cookies)
			{
				struct curl_slist* each = cookies;
				std::vector<std::string> _cookies;
				while (each)
				{
					_cookies.push_back(std::string(each->data));
					each = each->next;
				}
				curl_slist_free_all(cookies);
				response.cookies = _cookies;
			}
		}
		else
		{
			response.ok = false;
		}
	}
	
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
	curl_easy_cleanup(curl);
	response.text = response_holder;
	response.status_code = status_code;
	return response;
}

size_t write_to_string(void* ptr, size_t size, size_t count, void* stream)
{
	((std::string*)stream)->append((char*)ptr, 0, size * count);
	return size * count;
}
