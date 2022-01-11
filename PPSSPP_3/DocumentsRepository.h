#pragma once

#include <vector>
#include <algorithm>

#include "Document.h"

class DocumentsRepository
{
private:
	static std::vector<Document> documents;

public:
	std::vector<Document> get_documents();

	void add_document(const Document& document);

	bool contains(const int id);

	Document& get_by_id(const int id);

	void remove_by_id(const int id);

	void update_document(const Document& document);
};

