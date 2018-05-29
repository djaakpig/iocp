#include "TcpListener.h"
#include "Global.h"
#include "SockaddrIn.h"
#include "Socket.h"
#include "ExtensionTable.h"
#include "TcpSession.h"
#include "TcpSessionService.h"

TcpListener::TcpListener()
{
	_pSocket = new Socket();
}

TcpListener::~TcpListener()
{
	SafeDelete( _pSocket );
}

bool TcpListener::Create( const shared_ptr<TcpSessionService>& servicePtr )
{
	if( !_pSocket->Create( SOCK_STREAM, IPPROTO_TCP ) )
		return false;

	if( !_pSocket->SetOptionInt( SOL_SOCKET, SO_REUSEADDR, TRUE ) )
		return false;

	if( !_extensionTable.Load( _pSocket ) )
		return false;

	_servicePtr = servicePtr;

	return true;
}

void TcpListener::Close()
{
	_pSocket->Close();
	_servicePtr = nullptr;
}

bool TcpListener::ImbueContextTo( const Socket* const pChild ) const
{
	if( !pChild->SetNonblock( true ) )
		return false;

	auto listenSocket = _pSocket->GetSocketHandle();
	return pChild->SetOptionPtr( SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, &listenSocket );
}

bool TcpListener::Listen( const SockaddrIn& listenAddr, const DWORD numReserved, const IoCallbackFn&& fn )
{
	if( !_servicePtr )
		return false;

	if( !_pSocket->IsValid() )
		return false;

	if( !_pSocket->Bind( listenAddr ) )
		return false;

	if( SOCKET_ERROR == ::listen( _pSocket->GetSocketHandle(), SOMAXCONN ) )
		return false;

	const auto thisPtr = shared_from_this();

	for( DWORD sessionId = 0; numReserved > sessionId && _servicePtr->IsInProgress(); ++sessionId )
	{
		const auto sessionPtr = make_shared<TcpSession>( _extensionTable );

		if( !sessionPtr->Create( _servicePtr ) )
			continue;

		sessionPtr->SetId( sessionId );
		sessionPtr->SetOnAccept( move( fn ) );

		if( !sessionPtr->Accept( thisPtr ) )
			sessionPtr->Close();
	}

	return true;
}