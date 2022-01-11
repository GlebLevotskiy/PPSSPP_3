#include "DocumentByIdHandler.h"

DocumentByIdHandler::DocumentByIdHandler(mg_connection* connection)
{
	_connection = connection;
	_repository = DocumentsRepository();
}

void
DocumentByIdHandler::handle(
	struct mg_http_message* request,
	std::string methodType)
{
	std::string uri = std::basic_string(request->uri.ptr, request->uri.len);
	int id_value_start = uri.find_last_of(':');
	std::string id_value = uri.substr(id_value_start + 1, uri.size() - id_value_start);

	int id;

	try
	{
		id = std::stoi(id_value);
	}
	catch (const std::exception&)
	{
		Replies::reply_bad_request(_connection, "Invalid Id value.");
		return;
	}

	DocumentsRepository documents_repository = DocumentsRepository();

	if (!documents_repository.contains(id))
	{
		Replies::reply_successed_text(_connection, "No such document");
		return;
	}

	if (methodType == "GET")
	{
		send_document(id);
	}
	else if (methodType == "DELETE")
	{
		remove_document(id);
	}
	else if (methodType == "PUT")
	{
		update_document(id, request->body.ptr);
	}
}

void
DocumentByIdHandler::send_document(const int id)
{
	Json::Value result;
	Json::StreamWriterBuilder builder;

	auto document = _repository.get_by_id(id);

	result["id"] = document.get_id();
	result["value"] = document.get_root();

	const std::string json_file = Json::writeString(builder, result);

	Replies::reply_successed_json(_connection, json_file);
}

void
DocumentByIdHandler::remove_document(const int id)
{
	_repository.remove_by_id(id);
	Replies::reply_successed_text(_connection, "Item successfully removed.");
}

void
DocumentByIdHandler::update_document(
	const int id,
	const char* body_ptr)
{
	Json::Reader json_reader;
	Json::Value root;

	if (!json_reader.parse(body_ptr, root, false))
	{
		Replies::reply_bad_request(_connection, "Invalid document structure.");
		return;
	}

	auto document = _repository.get_by_id(id);

	document.set_root(root);
	_repository.update_document(document);

	Replies::reply_successed_text(_connection, "Item successfully modyfied.");
}

