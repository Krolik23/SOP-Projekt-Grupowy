#pragma once
#include <queue>
#include "Message.h"
#include <list>

class PostBox{

	std::queue<Message*> messageQueue;

	int postBoxOwnerPID;

	std::list<int> postBoxPartOwnerPID;

public:
	PostBox(int ownerPID);
	int getPostBoxOwnerPID() const;
	void addPostBoxPartOwner(int partOwnerPID);
	std::list<int>::iterator getPostBoxPartOwnerPID(int postBoxOwnerPID);
	void addMessage(Message* message);
	Message* getMessage();

};

