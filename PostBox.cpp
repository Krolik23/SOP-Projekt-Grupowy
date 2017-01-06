#include "PostBox.h"
#include <iostream>

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
	this->messageQueue.push_back(message);
}

Message* PostBox::getMessage()
{
	if (this->messageQueue.size() != 0) {
		Message* message = this->messageQueue.front();
		this->messageQueue.pop_front();
		return message;
	}
	return nullptr;
}

void PostBox::show() const
{
	if (messageQueue.empty())
	{
		cout << "Postbox is empty";
	}
	else {
		int counter = 1;
		cout << "Postbox has following messages:\n";
		for (auto temp : messageQueue)
		{
			cout << counter << ". \"" << temp->getMesageText() << "\"\n";
			counter++;
		}
	}
}

