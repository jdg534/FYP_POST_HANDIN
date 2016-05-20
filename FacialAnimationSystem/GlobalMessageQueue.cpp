#include "GlobalMessageQueue.h"

GlobalMessageQueue * f_instancePtr = nullptr;

GlobalMessageQueue * GlobalMessageQueue::getInstance()
{
	if (f_instancePtr == nullptr)
	{
		f_instancePtr = new GlobalMessageQueue();
	}
	return f_instancePtr;
}

GlobalMessageQueue::~GlobalMessageQueue()
{
	f_instancePtr = nullptr;
}

unsigned int GlobalMessageQueue::getNumMessages()
{
	return m_queue.size();
}

unsigned short GlobalMessageQueue::popMessage()
{
	unsigned short rv = m_queue.front();
	m_queue.pop();
	return rv;
}

void GlobalMessageQueue::addMessage(unsigned short msg)
{
	m_queue.push(msg);
}

GlobalMessageQueue::GlobalMessageQueue()
{

}