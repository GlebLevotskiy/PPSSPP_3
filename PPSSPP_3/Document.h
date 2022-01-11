#pragma once

#include "json/json.h"

class Document
{
private:
	static int ID;
	int id;
	Json::Value root;

public:
	Document();
	Document(Json::Value _root);
	Document(const Document& document);

	int get_id() const;

	Json::Value get_root() const;

	void set_root(Json::Value);

	bool operator == (const Document&) const;
	bool operator != (const Document&) const;
};