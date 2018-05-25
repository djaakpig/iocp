#include "TcpListener.h"
#include <WS2tcpip.h>
#include "Global.h"
#include "SockaddrIn.h"
#include "Socket.h"
#include "ExtensionTable.h"
#include "TcpSession.h"

TcpListener::TcpListener()
{
	_pSocket = new Socket();
}

TcpListener::~TcpListener()
{
	SafeDelete(_pSocket);
}

bool TcpListener::Create()
{
	if( !_pSocket->Create( SOCK_STREAM, IPPROTO_TCP ) )
		return false;

	if( !_pSocket->SetOptionInt( SOL_SOCKET, SO_REUSEADDR, TRUE ) )
		return false;

	if( !_extensionTable.Load( _pSocket ) )
		return false;

	return true;
}

void TcpListener::Close()
{
	_pSocket->Close();
}

HANDLE TcpListener::GetHandle() const
{
	return _pSocket->GetHandle();
}

bool TcpListener::ImbueContextTo(const Socket* const pChild) const
{
	if( !pChild->SetNonblock( true ) )
		return false;

	auto listenSocket = _pSocket->GetSocketHandle();
	return pChild->SetOptionPtr(SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, &listenSocket);
}

bool TcpListener::Listen( const SockaddrIn& listenAddr, const WORD numReserved, const IoCallbackFn&& fn)
{
	if( !_pSocket->IsValid() )
		return false;

	if(!_pSocket->Bind(listenAddr))
		return false;

	if(SOCKET_ERROR == ::listen(_pSocket->GetSocketHandle(), SOMAXCONN))
		return false;

	const auto thisPtr = shared_from_this();
	for( WORD sessionId = 0; numReserved > sessionId; ++sessionId )
	{
		const auto sessionPtr = make_shared<TcpSession>( _extensionTable );
		if( sessionPtr->Create() )
		{
			sessionPtr->SetOnAccept( move( fn ) );

			if( sessionPtr->Accept( thisPtr ) )
			{
			}
			else
			{
				sessionPtr->Close();
			}
		}
	}

	return true;
}