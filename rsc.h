#pragma once
#define WIN32_LEAN_AND_MEAN

#define TCPSOCK webxlib::TCPWEBSOCK
#define UDPSOCK webxlib::UDPWEBSOCK

#define IPV4SOCK webxlib::IPV4WEBSOCK
#define IPV6SOCK webxlib::IPV6WEBSOCK

#define SIMPLESOCK webxlib::SIMPLEWEBSOCK
#define SSLSOCK webxlib::SSLWEBSOCK

#define SOCK_SUCCESS webxlib::WEBSOCK_SUCCESS
#define SOCK_ERROR webxlib::WEBSOCK_ERROR
#define SOCK_INVALID webxlib::WEBSOCK_INVALID

#include "webhooks.h"
#include "webxlib.h"

typedef struct cl_info
{
	webxlib::socket* cl;
	std::map<std::string, std::string> rheaders;
} cl_info;