#pragma once

#include <string>
#include <algorithm>

#include "json/json.h"
#include "HttpUtility.h"
#include "DocumentsRepository.h"

class DocumentHandler
{
public:
	DocumentHandler(struct mg_connection* connection);

	void handle(
		struct mg_http_message* request,
		std::string methodType);

private:
	struct mg_connection* _connection;
	DocumentsRepository _repository;

	void load_document(const char* body_ptr);

	void send_all_documents();

	void send_filtered_documents(
		const std::string key,
		Json::Value root);

	static std::vector<Json::Value> get_members(
		Json::Value root,
		std::string key);
};

