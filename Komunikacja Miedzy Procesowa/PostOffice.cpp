#include "PostOffice.h"
#include "NoPostBoxException.h"
#include <iostream>

using namespace std;

list<PostBox*> PostOffice::blockOfPostBoxes;

void PostOffice::send(Message *message, int recipientPID)
{
	bool sent = false;
	for(auto temp : blockOfPostBoxes)
	{
		if(temp->getPostBoxOwnerPID() == recipientPID)
		{
			temp->addMessage(message);
			sent = true;
		}
	}

	if(sent == false)
	{
		PostBox* box = new PostBox(recipientPID);
		box->addMessage(message);
		blockOfPostBoxes.push_back(box);
	}
}

Message* PostOffice::receive(int postBoxOwnerPID)
{
		for (auto temp : blockOfPostBoxes)
		{
			if (temp->getPostBoxOwnerPID() == postBoxOwnerPID)
			{
				return temp->getMessage();
			}
		}
		throw NoPostBoxException();
}

void PostOffice::setPostBoxPartOwner(int postBoxPartOwnerPID, int postBoxOwnerPID)
{
	if(czyDziecko(postBoxPartOwnerPID,postBoxOwnerPID))
	{
		for (auto temp : blockOfPostBoxes)
		{
			if (temp->getPostBoxOwnerPID() == postBoxOwnerPID)
			{
				temp->addPostBoxPartOwner(postBoxPartOwnerPID);
			}
		}
	}
	else{cout << "Unable to set a part-owner";}
}

void PostOffice::deletePostBox(int postBoxOwnerPID)
{
	bool deleted = false;
	list<PostBox*>::iterator it;
	for (it = blockOfPostBoxes.begin(); it != blockOfPostBoxes.end(); ++it)
	{
		if ((*it)->getPostBoxOwnerPID() == postBoxOwnerPID)
		{
			PostBox* todelete = *it;
			delete todelete;
			it = --blockOfPostBoxes.erase(it);
			deleted = true;
		}
	}

	if(!deleted)
	{
		throw NoPostBoxException();
	}
}

bool PostOffice::processHasPostBox(int PID)
{
	for(auto temp : blockOfPostBoxes)
	{
		if (temp->getPostBoxOwnerPID() == PID)
			return true;
	}
	return false;
}
