#include "DocumentHandler.h"

DocumentHandler::DocumentHandler(mg_connection* connection)
{
	_connection = connection;
	_repository = DocumentsRepository();
}

void DocumentHandler::handle(
	mg_http_message* request,
	std::string methodType)
{
	if (methodType == "POST")
	{
		load_document(request->body.ptr);
	}
	else if (methodType == "GET")
	{
		if (request->query.len != 0)
		{
			std::stringstream stream(std::basic_string(request->query.ptr, request->query.len));
			QueryString qs(stream);

			if (!qs.hasParam("key"))
			{
				Replies::reply_bad_request(_connection, "Invalid query");
				return;
			}

			Json::Value root;
			auto key = qs.getParam("key")->firstValue();

			DocumentsRepository repository = DocumentsRepository();

			if (!qs.hasParam("value") || qs.getParam("value")->valueCount() == 0)
			{
				root = Json::Value(Json::ValueType::nullValue);
			}
			else
			{
				Json::Reader json_reader;
				auto value = qs.getParam("value")->firstValue();

				if (!json_reader.parse(value, root, false))
				{
					root = Json::Value(value);
				}
			}

			send_filtered_documents(key, root);
		}
		else
		{
			send_all_documents();
		}
	}
}

void
DocumentHandler::load_document(const char* body_ptr)
{
	Json::Reader json_reader;
	Json::StreamWriterBuilder builder;
	Json::Value root;

	json_reader.parse(body_ptr, root, false);
	Document new_document = Document(root);

	_repository.add_document(new_document);

	Json::Value result;
	result["id"] = new_document.get_id();

	const std::string json_file = Json::writeString(builder, result);

	Replies::reply_successed_json(_connection, json_file);
}

void
DocumentHandler::send_all_documents()
{
	Json::StreamWriterBuilder builder;
	Json::Value temp_value;
	Json::Value result = Json::Value(Json::ValueType::arrayValue);

	for (auto document : _repository.get_documents())
	{
		temp_value["id"] = document.get_id();
		temp_value["value"] = document.get_root();
		result.append(temp_value);
	}

	const std::string json_file = Json::writeString(builder, result);

	Replies::reply_successed_json(_connection, json_file);
	return;
}

void
DocumentHandler::send_filtered_documents(
	const std::string key,
	Json::Value root)
{
	Json::StreamWriterBuilder builder;

	auto documents = _repository.get_documents();

	std::vector<Document> selected_documents = std::vector<Document>();
	std::copy_if(
		documents.begin(),
		documents.end(),
		std::back_inserter(selected_documents),
		[key, root](Document document) {
			auto members = get_members(document.get_root(), key);

			return root.type() == Json::ValueType::nullValue
				? members.size() != 0
				: std::any_of(
					members.begin(),
					members.end(),
					[root](Json::Value member) { return member == root; });
		});

	if (selected_documents.size() == 0)
	{
		mg_http_reply(
			_connection,
			200,
			"Content-Type: application/text\r\n",
			"No such elements."
		);
		return;
	}

	Json::Value result, temp_value;

	for (auto document : selected_documents)
	{
		temp_value["id"] = document.get_id();
		temp_value["value"] = document.get_root();
		result.append(temp_value);
	}

	const std::string json_file = Json::writeString(builder, result);

	Replies::reply_successed_json(_connection, json_file);
}

std::vector<Json::Value>
DocumentHandler::get_members(
	Json::Value root,
	std::string key)
{
	std::vector<Json::Value> results = std::vector<Json::Value>();

	if (!root.isObject() && !root.isArray())
	{
		return results;
	}

	auto search_result = root.isMember(key);

	if (search_result)
	{
		results.push_back(root[key]);
	}

	for (auto id : root.getMemberNames())
	{
		auto child_result = get_members(root[id], key);

		results.insert(results.end(), child_result.begin(), child_result.end());
	}

	return results;
}
