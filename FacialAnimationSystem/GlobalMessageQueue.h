#ifndef _GLOBAL_MESSAGE_QUE_H_
#define _GLOBAL_MESSAGE_QUE_H_

#include <queue>

enum MessageTypes
{
	MT_QUIT = 1
};

class GlobalMessageQueue
{
public:
	static GlobalMessageQueue * getInstance();
	~GlobalMessageQueue();

	unsigned int getNumMessages();
	unsigned short popMessage();

	void addMessage(unsigned short msg);

private:
	GlobalMessageQueue();

	std::queue<unsigned short> m_queue;
};


#endif