#include "Document.h"

int Document::ID;

Document::Document()
{
	id = Document::ID++;
	root = Json::nullValue;
}

Document::Document(Json::Value _root)
{
	id = Document::ID++;
	root = _root;
}

Document::Document(const Document& document)
{
	id = document.get_id();
	root = document.get_root();
}



int
Document::get_id() const
{
	return id;
}

Json::Value
Document::get_root() const
{
	return root;
}

void
Document::set_root(Json::Value new_root)
{
	root = new_root;
}

bool
Document::operator == (const Document& document) const
{
	return id == document.get_id();
}

bool
Document::operator != (const Document& document) const
{
	return !(*this == document);
}