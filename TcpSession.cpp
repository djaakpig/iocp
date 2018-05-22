#include "TcpSession.h"
#include "Socket.h"
#include "Global.h"
#include "TcpListener.h"
#include <MSWSock.h>

TcpSession::TcpSession() : _recvCallback( 1024 )
{
	_pSocket = new Socket();
}

TcpSession::~TcpSession()
{
	SafeDelete( _pSocket );
}

HANDLE TcpSession::GetHandle() const
{
	return _pSocket->GetHandle();
}

bool TcpSession::Accept()
{
	SecureZeroMemory( &_localSockaddr, sizeof( _localSockaddr ) );
	SecureZeroMemory( &_remoteSockaddr, sizeof( _remoteSockaddr ) );

    return _acceptCallback.Post();
}

bool TcpSession::Accept( shared_ptr<TcpListener> listenerPtr, const IoCallbackAccept::Fn&& fn )
{
	if( !listenerPtr ) return false;

	_acceptCallback.Bind(listenerPtr,  shared_from_this(), move( fn ) );

	return Accept();
}

void TcpSession::Close()
{
	_pSocket->Close();
}

bool TcpSession::Create()
{
	if( !_pSocket->Create( SOCK_STREAM, IPPROTO_TCP ) )
		return false;

	if( !_pSocket->SetNonblock( true ) )
		return false;

	return true;
}

bool TcpSession::Recv( const IoCallbackRecv::Fn&& fn )
{
	_recvCallback.Bind( shared_from_this(), move( fn ) );

    return _recvCallback.Post();
}

bool TcpSession::Reuse( const IoCallbackReuse::Fn&& fn )
{
	_reuseCallback.Bind( shared_from_this(), move( fn ) );

    return _reuseCallback.Post();
}

bool TcpSession::Send( const IoCallbackSend::Fn&& fn )
{
    //  TODO: if the queue is not empty enqueue packet there.

	_sendCallback.Bind( shared_from_this(), move( fn ) );

    return _sendCallback.Post();
}