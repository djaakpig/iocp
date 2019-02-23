#include "TcpOperationConnect.h"
#include <WS2tcpip.h>
#include "WinsockExtension.h"
#include "Socket.h"
#include "TcpSession.h"

void TcpOperationConnect::OnComplete(const int32_t e)
{
	const auto r = _Invoke(e, _session);

	if(!r)
		_session->Disconnect();

	Clear();
}

bool TcpOperationConnect::Post(const std::shared_ptr<WinsockExtension>& extension)
{
	const auto r = extension->connectEx(_session->GetSocket()->GetValue(),
										_addr.ToSockAddrPtr(),
										_addr.GetSize(),
										nullptr,
										0,
										nullptr,
										this);

	return r ? true : _HandleError();
}