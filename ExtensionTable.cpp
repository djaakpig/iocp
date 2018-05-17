#include "ExtensionTable.h"
#include "Socket.h"

bool ExtensionTable::Load(const Socket* const pSocket)
{
	TransmitFile = static_cast<LPFN_TRANSMITFILE>(pSocket->GetExtension(WSAID_TRANSMITFILE));
	if(!TransmitFile) return false;

	AcceptEx = static_cast<LPFN_ACCEPTEX>(pSocket->GetExtension(WSAID_ACCEPTEX));
	if(!AcceptEx) return false;

	GetAcceptExSockaddrs = static_cast<LPFN_GETACCEPTEXSOCKADDRS>(pSocket->GetExtension(WSAID_GETACCEPTEXSOCKADDRS));
	if(!GetAcceptExSockaddrs) return false;

	ConnectEx = static_cast<LPFN_CONNECTEX>(pSocket->GetExtension(WSAID_CONNECTEX));
	if(!ConnectEx) return false;

	DisconnectEx = static_cast<LPFN_DISCONNECTEX>(pSocket->GetExtension(WSAID_DISCONNECTEX));
	if(!DisconnectEx) return false;

	return true;
}