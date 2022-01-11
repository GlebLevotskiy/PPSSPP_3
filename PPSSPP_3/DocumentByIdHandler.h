#pragma once

#include "HttpUtility.h"
#include "Document.h"
#include "DocumentsRepository.h"

#include <string>
#include <vector>

class DocumentByIdHandler
{
public:
	DocumentByIdHandler(struct mg_connection* connection);

	void handle(
		struct mg_http_message* request,
		std::string methodType);

private:
	struct mg_connection* _connection;
	DocumentsRepository _repository;

	void send_document(const int id);

	void remove_document(const int id);

	void update_document(
		const int id,
		const char* body_ptr);
};

