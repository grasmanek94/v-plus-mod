#include "ChatMessage.hxx"

ChatMessage::ChatMessage()
{ }

const std::wstring ChatMessage::GetMessage() const
{
	return message;
}

void ChatMessage::SetMessage(const std::wstring& msg)
{
	message = msg;
}

const size_t ChatMessage::GetSender() const
{
	return sender;
}

void ChatMessage::SetSender(size_t id)
{
	sender = id;
}

UniqueClassId_Implement(ChatMessage);
