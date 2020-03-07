#pragma once
#define WIN32_LEAN_AND_MEAN

#include "webhooks.h"

#include "webxlib.h"

typedef struct cl_info
{
	webxlib::socket* cl;
	std::map<std::string, std::string> rheaders;
} cl_info;