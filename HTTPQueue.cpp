#include "pch.h"

void HTTPQueue::PushToQueue(cl_http_packet http_request, WEBX_STATUS_CODE optStatus)
{
	QPair add;
	add.connection_info	  = http_request;
	add.connection_status = optStatus;

	this->queue.push_back( add );
}

void HTTPQueue::PopFromQueue(const QPair &cl)
{
	this->queue.erase(std::remove(this->queue.begin(), this->queue.end(), cl));
}

std::vector<QPair> HTTPQueue::GetQueue()
{
	return this->queue;
}

void HTTPQueue::UpdateStatus(QPair cl, WEBX_STATUS_CODE status)
{
	cl.connection_status = status;
}

int HTTPQueue::GetQueueCount()
{
	return this->queue.size();
}

void HTTPQueue::ClearQueue()
{
	this->queue.clear();
}