#include "PostBox.h"

using namespace std;


PostBox::PostBox(int ownerPID)
{
	postBoxOwnerPID = ownerPID;
}

int PostBox::getPostBoxOwnerPID() const
{
	return postBoxOwnerPID;
}

void PostBox::addPostBoxPartOwner(int partOwnerPID)
{
	this->postBoxPartOwnerPID.push_back(partOwnerPID);
}

list<int>::iterator PostBox::getPostBoxPartOwnerPID(int postBoxOwnerPID)
{
	list<int>::iterator it = postBoxPartOwnerPID.begin();
	return it;
}


void PostBox::addMessage(Message* message)
{
	this->messageQueue.push(message);
}

Message* PostBox::getMessage()
{
	if (this->messageQueue.size() != 0) {
		Message* message = this->messageQueue.front();
		this->messageQueue.pop();
		return message;
	}
	return nullptr;
}

