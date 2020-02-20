#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <wolfssl/ssl.h>

#include <Windows.h>
#include <map>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <algorithm>

#include "csocket.h"
#include "HTTPQueue.h"
#include "HTTPHandler.h"
#include "LockX.h"

/******globals******/
constexpr auto CERT_FILE = "certificate.crt";
constexpr auto KEY_FILE  = "private.key";

typedef struct WEBXM
{
	CSOCKET* http_srv;
	CSOCKET* https_srv;

	HTTPQueue* http_queue;
	HTTPQueue* https_queue;

	csockdata HTTP_init;
	csockdata HTTPS_init;

	HTTPHandler* httph;

	LockX* mutex;
} WEBXM;

//DWORD WINAPI http_listen(LPVOID pParam);

/******utils/******/
std::vector<std::string> strExplode(std::string const& s, char delim);
char* systime();
std::map<std::string, std::string> GetExtensionsTable();
/******************/