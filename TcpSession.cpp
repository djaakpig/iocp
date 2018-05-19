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
	return _Accept();
}

bool TcpSession::Accept( shared_ptr<TcpListener> listenerPtr, const IoCallbackAccept::Fn&& fn )
{
	SecureZeroMemory( &_localSockaddr, sizeof( _localSockaddr ) );
	SecureZeroMemory( &_remoteSockaddr, sizeof( _remoteSockaddr ) );

	if( !listenerPtr ) return false;

	_acceptCallback.Bind(listenerPtr,  shared_from_this(), move( fn ) );

	return _Accept();
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

	DWORD flags = 0;
	WSABUF wsaBuf{ 0,nullptr };
	const auto r = WSARecv( _pSocket->GetSocketHandle(), &wsaBuf, 1, nullptr, &flags, &_recvCallback, nullptr );

	if( SOCKET_ERROR == r )
	{
		const auto lastError = WSAGetLastError();
		if( WSA_IO_PENDING != lastError )
			return false;
	}

	return true;
}

bool TcpSession::Reuse( const IoCallbackReuse::Fn&& fn )
{
	_reuseCallback.Bind( shared_from_this(), move( fn ) );

	const DWORD flags = TF_DISCONNECT | TF_REUSE_SOCKET;
	const auto r = TransmitFile( _pSocket->GetSocketHandle(), nullptr, 0, 0, &_reuseCallback, nullptr, flags );

	if( !r )
	{
		const auto lastError = WSAGetLastError();
		if( WSA_IO_PENDING != lastError )
			return false;
	}

	return true;
}

bool TcpSession::Send( const IoCallbackSend::Fn&& fn )
{
	_sendCallback.Bind( shared_from_this(), move( fn ) );

	const DWORD flags = 0;
	WSABUF wsaBuf{ 0,nullptr };
	const auto r = WSASend( _pSocket->GetSocketHandle(), &wsaBuf, 1, nullptr, flags, &_sendCallback, nullptr );

	if( SOCKET_ERROR == r )
	{
		const auto lastError = WSAGetLastError();
		if( WSA_IO_PENDING != lastError )
			return false;
	}

	return true;
}

bool TcpSession::_Accept()
{
	const auto pListenSocket = _acceptCallback.GetListenerSocket();
	if( !pListenSocket ) return false;

	const auto r = AcceptEx( pListenSocket->GetSocketHandle(),
							 _pSocket->GetSocketHandle(),
							 _acceptCallback.GetBuffer(),
							 0,
							 LocalSockaddrLen,
							 RemoteSockaddrLen,
							 nullptr,
							 &_acceptCallback );

	if( SOCKET_ERROR == r )
	{
		const auto lastError = WSAGetLastError();
		if( WSA_IO_PENDING != lastError )
			return false;
	}

	return true;
}