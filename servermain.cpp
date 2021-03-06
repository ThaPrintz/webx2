#include "rsc.h"

#define CERT_FILE "certificate.crt"
#define KEY_FILE "private.key"

DWORD WINAPI http_listen(LPVOID);
DWORD WINAPI request_proc(LPVOID);

webxlib* master;
webxlib::webhook* webhks;

int main()
{
	master = new webxlib();
	webhks = master->NewWebhookInterface();

	csockdata httpsl;
	httpsl.address		 = "0.0.0.0";
	httpsl.port			 = "443";
	httpsl.dataprotocol  = TCPSOCK;
	httpsl.ipprotocol	 = IPV4SOCK;
	httpsl.socktype		 = SSLSOCK;

	webxlib::socket* httpsListener = master->NewWebsock(&httpsl);

	auto ret2 = httpsListener->Bind();
	if (ret2 != SOCK_SUCCESS)
		printf("[webx] Server boot failed, master listening socket failed to Bind\n");

	ret2 = httpsListener->Listen();
	if (ret2 != SOCK_SUCCESS)
		printf("[webx] Server boot failed, master listening socket failed to begin Listening\n");

	webhks->RegisterWebhook("INDEX", DEFAULT);

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
			if (httpsListener->Bind() == SOCK_SUCCESS) {
				if (httpsListener->Listen() == SOCK_SUCCESS) {
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

				if (client->SSLAccept() == SOCK_SUCCESS) {
					HANDLE process_cl_request = CreateThread(NULL, NULL, request_proc, (LPVOID)client, 0, NULL);
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

	csockdata httpl;
	httpl.address		= "0.0.0.0";
	httpl.port			= "80";
	httpl.dataprotocol  = TCPSOCK;
	httpl.ipprotocol	= IPV4SOCK;
	httpl.socktype		= SIMPLESOCK;

	webxlib::socket* httpListener = srv->NewWebsock(&httpl);

	auto ret = httpListener->Bind();
	if (ret != SOCK_SUCCESS)
		printf("[webx] Server boot failed, master listening socket failed to Bind\n");

	ret = httpListener->Listen();
	if (ret != SOCK_SUCCESS)
		printf("[webx] Server boot failed, master listening socket failed to begin Listening\n");

	while (true) {
		if (!httpListener->IsValid()) {
			printf("[webx 2.0] critical error! Server HTTP Listener failed!\n");

			printf("[webx 2.0] webx attempting to reboot server HTTP listener!\n");

			delete httpListener;
			httpListener = srv->NewWebsock(&httpl);
			if (httpListener->Bind() == SOCK_SUCCESS) {
				if (httpListener->Listen() == SOCK_SUCCESS) {
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
				HANDLE process_cl_request = CreateThread(NULL, NULL, request_proc, (LPVOID)client, 0, NULL);
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

	if (!client->IsValid())
		return NULL;

	char buff[1501];
	ZeroMemory(buff, 1501);
	auto shut = false;

	while (!shut) {
		int got = client->Recv(buff, 1500);
		if (got == SOCK_ERROR || strcmp(buff, "") == 0) {
			break;
		}

		printf("\n%s\n", buff);

		cl_info cl;
		cl.cl		= client;
		cl.rheaders = master->ParseHTTPRequest(buff);

		if (cl.rheaders["DATA"].substr(1, cl.rheaders["DATA"].size() - 1) == "favicon.ico")
			break;

		auto cll = cl.rheaders["DATA"].substr(1, cl.rheaders["DATA"].size() - 1);

		if (webhks->hookIsValid(cll) ){
			webhks->CallWebhook(cll, (void*)master, (void*)&cl);
		} else {
			webhks->CallWebhook("INDEX", (void*)master, (void*)&cl);
		}

		shut = true;
	}

	delete client;

	return NULL;
}