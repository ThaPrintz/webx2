#include "webhooks.h"
#include <Shlwapi.h>
#include "config.h"

void* DEFAULT(void* pparam, void* pparam2)
{
	webxlib* srv    = (webxlib*)pparam;
	cl_info* client = (cl_info*)pparam2;

	if (client->rheaders["METHOD"] == "GET") {
		const char* data = client->rheaders["DATA"].c_str();

		HTTP_packet resp;
		resp.httpversion	= "HTTP/1.1";
		resp.server			= "webxlib HTTP Framework";
		resp.content_security_policy = "Content-Security-Policy: default-src 'self'\r\n";
		resp.connection = "close";

		uint8_t* filex;
		size_t filesz;

		if (strcmp(data, "/") == 0) {
			if (srv->fileExists(mainpage)) {
				filex = srv->LoadFiletoMem((char*)mainpage, &filesz);

				resp.responsecode = "200 OK";
				resp.content_length = std::to_string(filesz).c_str();
				resp.content_type = "text/html";
				resp.response_content = (char*)filex;

				auto reply = srv->BuildResponsePacket(resp);

				client->cl->Send(reply.c_str(), reply.size());

				free(filex);

				return NULL;
			}
		} else {
			if (srv->fileExists(data)) {
				auto mimetypes		= srv->GetMimetypesTable();
				std::string fType	= PathFindExtensionA((LPCSTR)data);

				if (fType != "\0") {
					//its a file

				} else {
					//its a directory
				}
			} else {
				//404
			}
		}
	} else if (client->rheaders["METHOD"] == "HEAD") {
	} else if (client->rheaders["METHOD"] == "POST") {
	} else if (client->rheaders["METHOD"] == "PUT") {
	} else if (client->rheaders["METHOD"] == "DELETE") {
	} else if (client->rheaders["METHOD"] == "CONNECT") {
	} else if (client->rheaders["METHOD"] == "OPTIONS") {
	} else if (client->rheaders["METHOD"] == "TRACE") {
	} else if (client->rheaders["METHOD"] == "PATCH") {
	}
}