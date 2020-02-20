#pragma once
#include "pch.h"

class HTTPHandler
{
public:
	static void RegisterRequestHandler(const char* req, void* ptr);
	static void CallRequestHandler(const char* req, void* pParam, void* pParam2);

	static void RegisterRemoteCFunc(const char* req, void* ptr);
	static void CallRemoteCFunc(const char* req, void* pParam, void* pParam2);

	static void ParseHTTPRequest(char* data, cl_http_packet* cl);
	static std::string ConstructHTTPPacket(sv_http_packet sv);

	cl_http_packet ConstructHTTPStruct(CSOCKET* cl);
	//void http_read(WEBXM* srv, cl_http_packet* cl, CSOCKET_ENUM x);

	static std::map<std::string, void*>* GetRequestsTable();
	static std::map<std::string, void*>* GetRemoteFuncsTable();
};