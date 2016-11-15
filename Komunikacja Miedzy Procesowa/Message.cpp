#include "Message.h"
using namespace std;


Message::Message(int senderPID, string messageText){

	this->senderPID = senderPID;
	this->messageText = messageText;
	messageSize = messageText.size();
}

int Message::getSenderPID() const
{
	return senderPID;
}

string Message::getMesageText() const
{
	return messageText;
}

int Message::getMessageSize() const
{
	return messageSize;
}
