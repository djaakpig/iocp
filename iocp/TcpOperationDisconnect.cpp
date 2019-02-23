#include "TcpOperationDisconnect.h"
#include "Socket.h"
#include "WinsockExtension.h"
#include "TcpSession.h"

void TcpOperationDisconnect::OnComplete(const int32_t e)
{
	const auto adjustedError = ERROR_NETNAME_DELETED == e ? ERROR_SUCCESS : e;

	_Invoke(adjustedError, _session);
	Clear();
}

bool TcpOperationDisconnect::Post(const std::shared_ptr<WinsockExtension>& extension)
{
	const auto r = extension->disconnectEx(_session->GetSocket()->GetValue(), this, TF_REUSE_SOCKET, 0);
	return r ? true : _HandleError();
}