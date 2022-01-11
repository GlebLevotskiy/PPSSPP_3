#include "DocumentsRepository.h"

std::vector<Document> DocumentsRepository::documents(0);

std::vector<Document> DocumentsRepository::get_documents()
{
    return documents;
}

void DocumentsRepository::add_document(const Document& document)
{
    documents.push_back(document);
}

bool DocumentsRepository::contains(const int id)
{
	std::vector<Document>::iterator it = std::find_if(
		documents.begin(),
		documents.end(),
		[id](Document document) { return document.get_id() == id; });

	return it != documents.end();
}

Document& DocumentsRepository::get_by_id(const int id)
{
	return *(std::find_if(
		documents.begin(),
		documents.end(),
		[id](Document document) { return document.get_id() == id; }));
}

void DocumentsRepository::remove_by_id(const int id)
{
	std::erase_if(
		documents,
		[id](Document document) { return document.get_id() == id; });
}

void DocumentsRepository::update_document(const Document& new_document)
{
	auto new_doc_id = new_document.get_id();

	std::replace_if(
		documents.begin(),
		documents.end(),
		[new_doc_id](Document document) { return document.get_id() == new_doc_id; },
		new_document);
}
