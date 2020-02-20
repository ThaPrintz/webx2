#include "pch.h"

//catalyst function
static void _default(void* ptr, void* pParam);

static std::map<std::string, void*> reqhandlers;
static std::map<std::string, void*> remotefuncs;

void HTTPHandler::RegisterRequestHandler(const char* req, void* ptr)
{
	reqhandlers[req] = ptr;
}

void HTTPHandler::CallRequestHandler(const char* req, void* pParam, void* pParam2)
{
	(decltype(&_default)(reqhandlers[req]))(pParam, pParam2);
}

void HTTPHandler::RegisterRemoteCFunc(const char* req, void* ptr)
{
	remotefuncs[req] = ptr;
}

void HTTPHandler::CallRemoteCFunc(const char* req, void* pParam, void* pParam2)
{
	(decltype(&_default)(remotefuncs[req]))(pParam, pParam2);
}

std::map <std::string, void* >* HTTPHandler::GetRequestsTable()
{
	return &reqhandlers;
}

std::map <std::string, void* >* HTTPHandler::GetRemoteFuncsTable()
{
	return &remotefuncs;
}

void HTTPHandler::ParseHTTPRequest(char* in_http_request, cl_http_packet* parsed_request)
{
	auto htemp = strExplode(in_http_request, '\r\n');
	auto request_details = strExplode(htemp[0], ' ');

	parsed_request->request_method		= request_details[0].c_str();
	parsed_request->request_data	    = request_details[1].c_str();
	parsed_request->request_connection  = request_details[2].c_str();

	std::vector<std::string> reqheaders;

	auto it = reqheaders.begin();
	for (int i = 0; i <= htemp.size() - 3; i++)
	{
		reqheaders.push_back(htemp[i]);
	}

	for (int k = 1; k <= (int)reqheaders.size(); k++)
	{
		auto req_data = strExplode(reqheaders[k], ':');

		std::string value, prev;
		for (auto const& g : req_data) {
			value = g.substr(1, g.size());

			if (g[0] != ' ') {
				parsed_request->http_headers[value] = "";

				prev = value;
			} else {
				parsed_request->http_headers[prev] = value;
			}
		}
	}
}

std::string HTTPHandler::ConstructHTTPPacket(sv_http_packet headers)
{
	return headers.httpversion + std::string(" ") + headers.responsecode + "\r\n"
		+ headers.server		+ "\r\n"
		+ "Date: " + systime()  + "\r\n"
		+ headers.content_security_policy			  + "\r\n"
		+ "Content-Length: " + headers.content_length + "\r\n"
		+ "Content-Type: "	 + headers.content_type	  + "\r\n"
		+ "Connection:"		 + headers.connection	  + "\r\n\r\n"
		+ headers.response_content;
}

cl_http_packet HTTPHandler::ConstructHTTPStruct(CSOCKET* cl)
{
	cl_http_packet out;

	out.client = cl;

	return out;
}
/*
void HTTPHandler::http_read(WEBXM* srv, cl_http_packet* clReq, CSOCKET_ENUM protocol)
{
	char buff[1501];
	ZeroMemory(buff, 1501);

	CSOCKET* clcon = clReq->client;

	if (protocol == CSOCKET_HTTPS) {
		clcon->SSL_Init(CERT_FILE, KEY_FILE);
		clcon->SSLBindSocket();

		if (clcon->SSLAccept() != CSOCKET_FATAL_ERROR)
		{
			do {
				if (clcon->Recv(buff, 1500) != CSOCKET_FATAL_ERROR) {

					srv->httph->ParseHTTPRequest(buff, clReq);
				} else {
					break;
				}
			} while (clcon->SSLWantRead());
		}
	} else if (protocol == CSOCKET_HTTP) {
		while (int got = clcon->Recv(buff, 1500))
		{
			if (got == CSOCKET_FATAL_ERROR)
				break;

			srv->httph->ParseHTTPRequest(buff, clReq);
		}
	}
}*/