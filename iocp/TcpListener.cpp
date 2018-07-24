#include "TcpListener.h"
#include "Global.h"
#include "SockaddrIn.h"
#include "Socket.h"
#include "WinsockExtension.h"
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

bool TcpListener::Create()
{
	if( !_pSocket->Create( SOCK_STREAM, IPPROTO_TCP ) )
		return false;

	if( !_pSocket->SetOptionInt( SOL_SOCKET, SO_REUSEADDR, TRUE ) )
		return false;

	return true;
}

void TcpListener::Close()
{
	_pSocket->Close();
}

bool TcpListener::SetContextTo( const Socket* const pChild ) const
{
	if( !pChild->SetNonblock( true ) )
		return false;

	auto listenSocket = _pSocket->GetValue();

	return pChild->SetOptionPtr( SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, &listenSocket );
}

bool TcpListener::Listen( const SockaddrIn& listenAddr )
{
	if( !_pSocket->IsValid() )
		return false;

	if( !_pSocket->Bind( listenAddr ) )
		return false;

	if( SOCKET_ERROR == ::listen( _pSocket->GetValue(), SOMAXCONN ) )
		return false;

	return true;
}