#pragma once
#include "Message.h";
#include <list>
#include "PostBox.h"


class PostOffice{

	static std::list<PostBox*> blockOfPostBoxes;
	
public:
	static void send(Message *message, int recipientPID);
	static Message* receive(int postBoxOwnerPID);
	static void setPostBoxPartOwner(int postBoxOwnerPID, int postBoxPartOwnerPID);
	static void deletePostBox(int postBoxOwnerPID);
	static bool processHasPostBox(int PID);
};

