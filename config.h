#pragma once

/**************************************************************************
defaults, if no custom file is found
***************************************************************************/
const char* mainpage =
"<html><body><center><h1>webserver running!</h1></br><h3>powered by webxlib HTTP framework</h3></center></body></html>";

const char* FOURZEROZERO =
"<html><body><center><h1>400 Bad Request</h1></br><h3>the server cannot respond due a client error</h3></br><h3>powered by webxlib HTTP framework</h3></center></body></html>";

const char* FOURZEROONE =
"<html><body><center><h1>401 Unauthorizedd</h1></br><h3>target resource lacks valid authentication credentials</h3></br><h3>powered by webxlib HTTP framework</h3></center></body></html>";

const char* FOURZEROTHREE =
"<html><body><center><h1>403 Forbidden</h1></br><h3>request method recognized by server, but you are not authorized to use it.</h3></br><h3>powered by webxlib HTTP framework</h3></center></body></html>";

const char* FOURZEROFOUR =
"<html><body><center><h1>404 Not Found</h1></br><h3>server can't find the requested resource</h3></br><h3>powered by webxlib HTTP framework</h3></center></body></html>";

const char* FOURZEROFIZE =
"<html><body><center><h1>405 Method Not Allowed</h1></br><h3>request method recognized by server, but not supported by target resource.</h3></br><h3>powered by webxlib HTTP framework</h3></center></body></html>";

/**************************************************************************
default file names, if this file is found, it will load it instead of 
sending one of the defaults defined above
***************************************************************************/
//first page server loads when recieving HTTP GET request for '/'
const char* webxindex = "index.html";

//server response codes in which loading a custom file would make sense
const char* foocp = "400.html";
const char* fowcp = "401.html";
const char* fotcp = "403.html";
const char* fofcp = "404.html";
const char* fopcp = "405.html";