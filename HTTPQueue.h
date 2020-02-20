#pragma once
#include "pch.h"

typedef struct cl_http_packet
{
	//client request information for when dissecting a packet
	CSOCKET* client;
	const char* request_method;
	const char* request_data;
	const char* request_connection;
	bool secure;
	std::map<std::string, std::string> http_headers;
} cl_http_packet;

typedef struct sv_http_packet
{
	//server headers to send when constructing a packet
	const char* httpversion;
	const char* responsecode;
	const char* server = "Server: webx 2.0 HTTP framework\r\n";
	const char* date;
	const char* content_security_policy = "Content-Security-Policy: default-src 'self'\r\n";
	const char* content_length;
	const char* content_type;
	const char* connection;
	char* response_content;
} sv_http_packet;

typedef int WEBX_STATUS_CODE;
/*queue status codes*/
constexpr WEBX_STATUS_CODE WEBXCLIENT_BUSY			 = 0;
constexpr WEBX_STATUS_CODE WEBXCLIENT_WANT_READ	     = 1;
constexpr WEBX_STATUS_CODE WEBXCLIENT_WANT_WRITE	 = 2;
constexpr WEBX_STATUS_CODE WEBXCLIENT_WANT_CONNECT   = 3;
constexpr WEBX_STATUS_CODE WEBXCLIENT_WANT_ACCEPT	 = 4;
constexpr WEBX_STATUS_CODE WEBXCLIENT_WANT_SSLACCEPT = 5;

typedef struct QPair
{
	WEBX_STATUS_CODE connection_status;
	cl_http_packet connection_info;

	bool operator==(const QPair& other) { return connection_info.client == other.connection_info.client; };
} QPair;

class HTTPQueue
{
public:
	void PushToQueue(cl_http_packet cl, WEBX_STATUS_CODE code);
	void PopFromQueue(const QPair& cl);

	void UpdateStatus(QPair cl, WEBX_STATUS_CODE status);

	std::vector<QPair> GetQueue();

	int GetQueueCount();
	void ClearQueue();

protected:
	std::vector<QPair> queue;
};
