#include "webhooks.h"
#include <Shlwapi.h>
#include "config.h"
#include <direct.h>

void* DEFAULT(void* pparam, void* pparam2)
{
	webxlib* srv    = (webxlib*)pparam;
	cl_info* client = (cl_info*)pparam2;

	if (client->rheaders["METHOD"] == "GET") {
		std::string data = client->rheaders["DATA"];

		HTTP_packet resp;
		resp.server					 = std::string("webxlib HTTP Framework");
		resp.connection				 = std::string("close");

		size_t filesz;

		if (strcmp(data.c_str(), "/") == 0) {
			resp.responsecode = std::string("200 OK");
			resp.content_type = std::string("text/html");

			if (srv->fileExists(webxindex)) {
				resp.response_content = std::string((char*)srv->LoadFiletoMem((char*)webxindex, &filesz));
				resp.content_length   = std::to_string(filesz);
			} else {
				resp.response_content = std::string(mainpage);
				resp.content_length   = std::to_string(strlen(mainpage));
			}

			auto reply = srv->BuildResponsePacket(resp);
			client->cl->Send(reply.c_str(), reply.size());

			return NULL;
		} else {
			data = client->rheaders["DATA"].substr(1, client->rheaders["DATA"].size() - 1);

			if (strcmp(data.c_str(), "favicon.ico") == 0)
				return NULL;

			if (srv->fileExists(data.c_str())) {
				auto mimetypes = srv->GetMimetypesTable();
				std::string fType = PathFindExtensionA((LPCSTR)data.c_str());

				if (fType != "\0") {
					fType = fType.substr(1, fType.size() - 1);

					auto it = mimetypes.find(fType.data());
					if (it != mimetypes.end()) {
						resp.content_type = mimetypes[fType];
					} else {
						resp.content_type = std::string("application/octet-stream");
					}

					resp.responsecode		= std::string("200 OK");
					resp.response_content	= "";

					printf("\n\nxf: %s\n\n", data.c_str());
					auto filed = (const char*)srv->LoadFiletoMem((char*)data.data(), &filesz);
					resp.content_length = std::to_string(filesz);


					auto reply = srv->BuildResponsePacket(resp);

					client->cl->Send(reply.c_str(), reply.size());
					client->cl->Send(filed, filesz);

					return NULL;
				} else {
					//this means there was either no file extension found on the file or its a directory, so we'll treat it like a directory?

					//get the filepath to the location of the web server client
					char cCurrentPath[FILENAME_MAX];
					char newdir[MAX_PATH];

					newdir[0] = '\0';

					if (_getcwd(cCurrentPath, sizeof(cCurrentPath))) {
						strncat_s(newdir, MAX_PATH, cCurrentPath, strlen(cCurrentPath));
						strncat_s(newdir, MAX_PATH, client->rheaders["DATA"].data(), client->rheaders["DATA"].length());
					}

					//format the newly concatted filepath + local requested directory to have backslashes instead of forward slashes
					for (int i = 0; i <= strlen(newdir); i++) {
						if (newdir[i] == '/') {

							newdir[i] = '\\';
						}
					}

					//add \\* on the end of the file path to indicated we want to iterate the entire directory
					strncat_s(newdir, MAX_PATH, "\\*", 2);

					//count how large the buffer needs to be to generate HTML
					WIN32_FIND_DATA FindFileData;
					int				fCount = 0;
					int				fNames = 0;

					HANDLE hFindFile = FindFirstFile((LPCWSTR)newdir, &FindFileData);
					if (hFindFile != INVALID_HANDLE_VALUE) {
						do {
							if (FindFileData.cFileName[0] == '.' && FindFileData.cFileName[1] == '\0' || FindFileData.cFileName[0] == '.' && FindFileData.cFileName[1] == '.') continue;

							fCount++;
							fNames += (strlen(client->rheaders["DATA"].data()) + std::char_traits<wchar_t>::length(FindFileData.cFileName) + 2);
						} while (FindNextFile(hFindFile, &FindFileData) != 0);

						FindClose(hFindFile);
					}

					//import canvas page HTML
					size_t fsize2;

					char* ret = (char*) srv->LoadFiletoMem((char*)"canvas1.html", &filesz);
					char* ret2 = (char*)srv->LoadFiletoMem((char*)"canvas2.html", &fsize2);

					//calculate size of buffer needed for HTML & create buffer
					int buffsz = filesz + fsize2 + fNames + (fCount * 115/*115 is the amount of characters in HTML each directory listing needs*/);
					buffsz *= 1.4;

					char* trueret = new char[buffsz];
					trueret[0] = '\0';

					//copy first half of HTML canvas page to buffer
					strncat_s(trueret, buffsz, ret, filesz);

					//copy directory name HTML to buffer
					strncat_s(trueret, buffsz, "<h1 class=\"text-lowercase text-white font-weight-bold\">", strlen("<h1 class=\"text-lowercase text-white font-weight-bold\">"));
					strncat_s(trueret, buffsz, data.data(), data.length());
					strncat_s(trueret, buffsz, "/</h1><hr class=\"divider my-4\"></div><div class=\"col-lg-8 align-self-baseline\">", strlen("/</h1><hr class=\"divider my-4\"></div><div class=\"col-lg-8 align-self-baseline\">"));

					//generate HTML for directory listing
					wchar_t filename[4096] = { 0 };
					MultiByteToWideChar(0, 0, newdir, strlen(newdir), filename, strlen(newdir));

					hFindFile = FindFirstFile(filename, &FindFileData);
					if (hFindFile != INVALID_HANDLE_VALUE) {
						do {
							std::wstring ws(FindFileData.cFileName);
							std::string dp(ws.begin(), ws.end());

							if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
								strncat_s(trueret, buffsz, "<p class=\"text-white-75 font-weight-light mb-5\" style=\"text-align: left; margin-left: 7vw\"\">", strlen("<p class=\"text-white-75 font-weight-light mb-5\" style=\"text-align: left; margin-left: 7vw\"\">"));
								strncat_s(trueret, buffsz, data.data(), data.length());
								strncat_s(trueret, buffsz, "/", strlen("/"));
								strncat_s(trueret, buffsz, dp.c_str(), dp.length());
								strncat_s(trueret, buffsz, "</p>", strlen("</p>"));
								//strncat_s(trueret, buffsz, data.data(), data.length());
								//strncat_s(trueret, buffsz, "/", strlen("/"));
								//strncat_s(trueret, buffsz, (char*)FindFileData.cFileName, std::char_traits<wchar_t>::length(FindFileData.cFileName));
								//strncat_s(trueret, buffsz, "</a><br><br>", strlen("</a><br><br>"));
							} else if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
								if (FindFileData.cFileName[0] == '.' && FindFileData.cFileName[1] == '\0' || FindFileData.cFileName[0] == '.' && FindFileData.cFileName[1] == '.') continue;

								std::vector<std::string>fp = srv->stringExp(dp, '\\');

								strncat_s(trueret, buffsz, "<p class=\"text-white-75 font-weight-light mb-5\" style=\"text-align: left; margin-left: 7vw\"\">", strlen("<p class=\"text-white-75 font-weight-light mb-5\" style=\"text-align: left; margin-left: 7vw\"\">"));
								strncat_s(trueret, buffsz, data.data(), data.length());
								strncat_s(trueret, buffsz, "/", strlen("/"));
								strncat_s(trueret, buffsz, fp[fp.size() - 1].data(), fp[fp.size() - 1].length());
								strncat_s(trueret, buffsz, "/</p>", strlen("/</p>"));
								//strncat_s(trueret, buffsz, "\" role=\"button\">skylers.work", strlen("\" role=\"button\">skylers.work"));
								//strncat_s(trueret, buffsz, data.data(), data.length());
								//strncat_s(trueret, buffsz, "/", strlen("/"));
								//strncat_s(trueret, buffsz, fp[fp.size() - 1].data(), fp[fp.size() - 1].length());
								//strncat_s(trueret, buffsz, "/</a><br><br>", strlen("/</a><br><br>"));
							}
						} while (FindNextFile(hFindFile, &FindFileData) != 0);

						FindClose(hFindFile);
					}
					//copy second half of canvas page HTML to buffer

					strncat_s(trueret, buffsz, ret2, fsize2);

					resp.responsecode = std::string("200 OK");
					resp.content_type = std::string("text/html");
					resp.response_content = "";
					resp.content_length = std::to_string(strlen(trueret));

					auto reply = srv->BuildResponsePacket(resp);
					client->cl->Send(reply.c_str(), reply.size());
					client->cl->Send(trueret, (int)strlen(trueret));

					return NULL;
				}
			} else {
				//404
				resp.responsecode = std::string("200 OK");
				resp.content_type = std::string("text/html");
				resp.response_content = "";

				if (srv->fileExists(fofcp)) {
					const char* sd = (char*)srv->LoadFiletoMem((char*)fofcp, &filesz);
					resp.content_length = std::to_string(filesz);

					auto reply = srv->BuildResponsePacket(resp);

					client->cl->Send(reply.c_str(), reply.size());
					client->cl->Send(sd, filesz);

					return NULL;
				} 
				
				resp.content_length = std::to_string(strlen(FOURZEROFOUR));

				auto reply = srv->BuildResponsePacket(resp);
				client->cl->Send(reply.c_str(), reply.size());
				client->cl->Send(FOURZEROFOUR, strlen(FOURZEROFOUR));

				return NULL;
			}
		}
	} //else if (client->rheaders["METHOD"] == "HEAD") {
	//} else if (client->rheaders["METHOD"] == "POST") {
	//} else if (client->rheaders["METHOD"] == "PUT") {
	//} else if (client->rheaders["METHOD"] == "DELETE") {
	//} else if (client->rheaders["METHOD"] == "CONNECT") {
	//} else if (client->rheaders["METHOD"] == "OPTIONS") {
	//} else if (client->rheaders["METHOD"] == "TRACE") {
	//} else if (client->rheaders["METHOD"] == "PATCH") {
	//}

	return nullptr;
}