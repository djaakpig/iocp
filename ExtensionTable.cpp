#include "ExtensionTable.h"
#include "Socket.h"

bool ExtensionTable::Load(const Socket* const pSocket)
{
	transmitFile = pSocket->GetExtension<LPFN_TRANSMITFILE>(WSAID_TRANSMITFILE);
	if(!transmitFile) return false;

	acceptEx = pSocket->GetExtension<LPFN_ACCEPTEX>(WSAID_ACCEPTEX);
	if(!acceptEx) return false;

	getAcceptExSockaddrs = pSocket->GetExtension<LPFN_GETACCEPTEXSOCKADDRS>(WSAID_GETACCEPTEXSOCKADDRS);
	if(!getAcceptExSockaddrs) return false;

	connectEx = pSocket->GetExtension<LPFN_CONNECTEX>(WSAID_CONNECTEX);
	if(!connectEx) return false;

	disconnectEx = pSocket->GetExtension<LPFN_DISCONNECTEX>(WSAID_DISCONNECTEX);
	if(!disconnectEx) return false;

	return true;
}