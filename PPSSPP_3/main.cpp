#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable : 4996)

extern "C" {
	#include "mongoose.h"
}

#include "DocumentByIdHandler.h"
#include "DocumentHandler.h"

static void
handler(
	struct mg_connection* c,
	int ev,
	void* ev_data,
	void* handler_data)
{
	if (ev == MG_EV_HTTP_MSG)
	{
		struct mg_http_message* request = (struct mg_http_message*)ev_data;
		std::string methodType = std::basic_string(request->method.ptr, request->method.len);

		if (mg_http_match_uri(request, "/api/serviceurl"))
		{
			DocumentHandler handler = DocumentHandler(c);
			handler.handle(
				request,
				methodType);	
		}
		else if (mg_http_match_uri(request, "/api/serviceurl/*"))
		{
			DocumentByIdHandler handler = DocumentByIdHandler(c);
			handler.handle(
				request,
				methodType);
		}
		else
		{
			Replies::reply_bad_request(c, "No such handler.");
		}
	}
}

int
main(
	int argc,
	char* argv[])
{
	struct mg_mgr mgr;
	mg_mgr_init(&mgr);											   
	mg_http_listen(&mgr, "http://localhost:8000", handler, &mgr);  
	for (;;) mg_mgr_poll(&mgr, 1000);							   
	mg_mgr_free(&mgr);											   
	return 0;
}