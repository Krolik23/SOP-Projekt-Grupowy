#include "PostOffice.h"
#include <iostream>
#include "Procesy.h"
#include "Semaphore.h"
#include "Scheduler.h"

using namespace std;

list<PostBox*> PostOffice::blockOfPostBoxes;

void PostOffice::send(Message *message, int recipientPID)
{
	bool sent = false;
	for (auto temp : blockOfPostBoxes)
	{
		if (temp->getPostBoxOwnerPID() == recipientPID)
		{
			temp->addMessage(message);
			sent = true;
		}
	}

	if (sent == false)
	{
		PostBox* box = new PostBox(recipientPID);
		box->addMessage(message);
		blockOfPostBoxes.push_back(box);
	}

	//SemaphorePOST
	PCB* temp;
	for (int i = 0; i < semaphorePOST.waiting.size(); i++) {
		temp = semaphorePOST.waiting.front();
		semaphorePOST.waiting.pop_front();
		if (temp->PID == recipientPID) {
			semaphorePOST.opening();
			semaphorePOST.tryPassing(temp);
			if (!(semaphorePOST.waiting.empty())) semaphorePOST.closing();
		}
		else semaphorePOST.waiting.push_back(temp);
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
	semaphorePOST.P(Scheduler::Running);
	return nullptr;
}

void PostOffice::setPostBoxPartOwner(int postBoxPartOwnerPID, int postBoxOwnerPID)
{
	if (czyDziecko(postBoxPartOwnerPID, postBoxOwnerPID))
	{
		for (auto temp : blockOfPostBoxes)
		{
			if (temp->getPostBoxOwnerPID() == postBoxOwnerPID)
			{
				temp->addPostBoxPartOwner(postBoxPartOwnerPID);
			}
		}
	}
	else { cout << "Unable to set a part-owner"; }
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
}

bool PostOffice::processHasPostBox(int PID)
{
	for (auto temp : blockOfPostBoxes)
	{
		if (temp->getPostBoxOwnerPID() == PID)
			return true;
	}
	return false;
}

void PostOffice::showPostBox(int postboxOwnerPID)
{
	if (blockOfPostBoxes.empty())
	{
		cout << "There is no postbox" << endl;
	}
	else {
		for (auto temp : blockOfPostBoxes)
		{
			if (temp->getPostBoxOwnerPID() == postboxOwnerPID)
			{
				temp->show();
			}
		}
	}
}

void PostOffice::showPostOffice()
{
	if (blockOfPostBoxes.empty())
	{
		cout << "There is no postbox" << endl;
	}
	else {
		int counter = 1;
		cout << "There are following postboxes: ";
		for (auto temp : blockOfPostBoxes)
		{
			cout << counter << ". " << "Postbox for PID: " << temp->getPostBoxOwnerPID() << endl;
			counter++;
		}
	}
}
