#pragma once
#include <string>


class Message{

	int senderPID;

	std::string messageText;

	int messageSize;

public:
	Message(int senderPID, std::string messageText);
	int getSenderPID() const;
	std::string getMesageText() const;
	int getMessageSize() const;

};

