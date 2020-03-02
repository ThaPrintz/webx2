#include "pch.h"

struct HTTPRequestHandlers
{
	//these handlers are neccesary for webx 2.0 to function
	//as a static file server. The default hander MUST NOT
	//be deleted unless you plan on writing your own
	static void WEBX_ADMIN(LPVOID);
	static void WEBX_DEFAULT_HANDLER(LPVOID);

	//any personal request handler that are nonessential to
	//webx's standard operations will be declared here, for security
	//purposes although the custom request handlers here are declared
	//(and presumably later defined) you still must call HTTPHandler::RegisterRequestHandler(cosnt char*, void*)
	static void LIBRARY(LPVOID, LPVOID);
	static void ABOUTME(LPVOID, LPVOID);
	static void DOCS(LPVOID, LPVOID);
	static void PORTFOLIO(LPVOID, LPVOID);

	static void LUXZCALL(LPVOID);
	static void CRYPTX(LPVOID);
};

//typedef struct RemoteCFunctions
//{
//
//};