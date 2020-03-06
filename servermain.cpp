#include "rsc.h"

#define CERT_FILE "certificate.crt"
#define KEY_FILE "private.key"

DWORD WINAPI http_listen(LPVOID);
DWORD WINAPI request_proc(LPVOID);

webxlib* master;

int main()
{
	master = CreateWEBXInterface();
		  		
	websockdata httpsl;
	httpsl.address		 = "0.0.0.0";
	httpsl.port			 = "443";
	httpsl.dataprotocol  = TCPWEBSOCK;
	httpsl.ipprotocol	 = IPV4WEBSOCK;
	httpsl.secure		 = HTTPSWEBSOCK;

	webxlib::socket* httpsListener = master->NewWebsock(&httpsl);

	auto ret2 = httpsListener->Bind();
	if (ret2 != WEBSOCK_SUCCESS)
		printf("[webx] Server boot failed, master listening socket failed to Bind\n");

	ret2 = httpsListener->Listen();
	if (ret2 != WEBSOCK_SUCCESS)
		printf("[webx] Server boot failed, master listening socket failed to begin Listening\n");

	printf("[webx 2.0] webx boot procedure completed successfully!\n");
	printf("[webx 2.0] HTTP/S Framework stable & listening for connections!\n");
	printf("-------------------webx 2.0 by Skyler Nelson-------------------\n");

	CreateThread(NULL, NULL, http_listen, (LPVOID)master, NULL, NULL);

	while (true) {
		if (!httpsListener->IsValid()) {
			printf("[webx 2.0] critical error! Server HTTPS Listener failed!\n");

			printf("[webx 2.0] webx attempting to reboot server HTTPS listener!\n");

			delete httpsListener;
			httpsListener = master->NewWebsock(&httpsl);
			if (httpsListener->Bind() == WEBSOCK_SUCCESS) {
				if (httpsListener->Listen() == WEBSOCK_SUCCESS) {
					printf("[webx 2.0] webx rebooted server HTTPS listener!\n");
				}
			} else {
				printf("[webx 2.0] critical error! Server HTTPS reboot failed!\n");
			}

			continue;
		}

		if (httpsListener->SelectReadable({ 0,0 }) > 0) {
			webxlib::socket* client = httpsListener->Accept();
			if (client->IsValid()) {
				client->SSLInit(CERT_FILE, KEY_FILE);
				client->SSLBind();

				if (client->SSLAccept()) {
					if (!client->IsSecure())
						client->SetSecure(true);

					HANDLE process_cl_request = CreateThread(NULL, NULL, request_proc, (LPVOID)&client, 0, NULL);
				}
			} else {
				delete client;

				continue;
			}
		}
	}

	return NULL;
}

DWORD WINAPI http_listen(LPVOID pparam)
{
	webxlib* srv = (webxlib*)pparam;

	websockdata httpl;
	httpl.address		= "0.0.0.0";
	httpl.port			= "80";
	httpl.dataprotocol  = TCPWEBSOCK;
	httpl.ipprotocol	= IPV4WEBSOCK;
	httpl.secure		= HTTPWEBSOCK;

	webxlib::socket* httpListener = srv->NewWebsock(&httpl);

	auto ret = httpListener->Bind();
	if (ret != WEBSOCK_SUCCESS)
		printf("[webx] Server boot failed, master listening socket failed to Bind\n");

	ret = httpListener->Listen();
	if (ret != WEBSOCK_SUCCESS)
		printf("[webx] Server boot failed, master listening socket failed to begin Listening\n");

	while (true) {
		if (!httpListener->IsValid()) {
			printf("[webx 2.0] critical error! Server HTTP Listener failed!\n");

			printf("[webx 2.0] webx attempting to reboot server HTTP listener!\n");

			delete httpListener;
			httpListener = srv->NewWebsock(&httpl);
			if (httpListener->Bind() == WEBSOCK_SUCCESS) {
				if (httpListener->Listen() == WEBSOCK_SUCCESS) {
					printf("[webx 2.0] webx rebooted server HTTP listener!\n");
				}
			} else {
				printf("[webx 2.0] critical error! Server HTTP reboot failed!\n");
			}

			continue;
		}

		if (httpListener->SelectReadable({ 0,0 }) > 0) {
			webxlib::socket* client = httpListener->Accept();
			if (client->IsValid()) {
				if (client->IsSecure())
					client->SetSecure(false);

				HANDLE process_cl_request = CreateThread(NULL, NULL, request_proc, (LPVOID)&client, 0, NULL);
			} else {
				delete client;

				continue;
			}
		}
	}

	return NULL;
}

DWORD WINAPI request_proc(LPVOID pparam)
{
	webxlib::socket* client = (webxlib::socket*)pparam;

	if (!client->IsValid()) {
		return NULL;
	}

	char buff[1501];
	ZeroMemory(buff, 1501);
	auto shut = false;
	while (!shut) {
		int got = client->Recv(buff, 1500);
		printf("\n\n%i\n\n", got);
		if (got == WEBSOCK_ERROR) {
			break;
		}

		printf("%s\n", buff);

		auto dets = master->ParseHTTPRequest(buff);
		for (auto& n : dets) {
			printf("x%s:%s\n", n.first.c_str(), n.second.c_str());
		}

		printf("client 0x%p called request method '%s' for target resource '%s'", client, dets["METHOD"].c_str(), dets["DATA"].c_str());

		shut = true;
	}

	return NULL;
}