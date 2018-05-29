#include "TcpServerService.h"
#include "IoService.h"
#include "Log.h"
#include "TcpSession.h"
#include "CircularBuffer.h"
#include "TcpListener.h"

bool TcpServerService::_Start( const SockaddrIn& listenAddr, const DWORD numReserved )
{
	const auto thisPtr = shared_from_this();
	_listenerPtr = make_shared<TcpListener>();

	if( !_listenerPtr->Create( thisPtr ) )
		return false;

	if( !GetIoService().Associate( _listenerPtr->GetSocket() ) )
		return false;

	const auto acceptCallback = bind( &TcpServerService::_OnAccept, this, placeholders::_1, placeholders::_2 );

	if( !_listenerPtr->Listen( listenAddr, numReserved, acceptCallback ) )
		return false;

	LogForce( "SERVER start!" );

	return true;
}

void TcpServerService::_Stop()
{
	if( _listenerPtr )
	{
		_listenerPtr->Close();
		_listenerPtr = nullptr;
	}

	LogForce( "SERVER stop!" );
}

bool TcpServerService::_OnAccept( const int e, const shared_ptr<TcpSession>& sessionPtr )
{
	if( e )
	{
		LogError( "accept fail! error:", e );
		return false;
	}

	if( !GetIoService().Associate( sessionPtr->GetSocket() ) )
	{
		LogWarning( "accept associate fail!" );
		return false;
	}

	sessionPtr->SetOnDisconnect( bind( &TcpServerService::_OnDisconnect, this, placeholders::_1, placeholders::_2 ) );
	sessionPtr->SetOnRecv( bind( &TcpServerService::_OnRecv, this, placeholders::_1, placeholders::_2, placeholders::_3 ) );
	sessionPtr->SetOnSend( bind( &TcpServerService::_OnSend, this, placeholders::_1, placeholders::_2, placeholders::_3 ) );

	if( !sessionPtr->Recv() )
	{
		LogWarning( "first recv fail!" );
		return false;
	}

	if( !IsInProgress() )
	{
		LogWarning( "refuse!" );
		return false;
	}

	_Add( sessionPtr );

	LogNormal( "accept! id:", sessionPtr->GetId() );

	return true;
}

bool TcpServerService::_OnDisconnect( const int e, const shared_ptr<TcpSession>& sessionPtr )
{
	_Remove( sessionPtr->GetId() );

	if( e )
	{
		LogError( "disconnect fail! id:", sessionPtr->GetId(), " error:", e );
		return false;
	}

	if( !IsInProgress() )
	{
		LogWarning( "refuse!" );
		return false;
	}

	return sessionPtr->Accept( _listenerPtr );
}

bool TcpServerService::_OnRecv( const int e, const shared_ptr<TcpSession>& sessionPtr, CircularBuffer& buf )
{
	if( e )
	{
		LogError( "recv fail! id:", sessionPtr->GetId(), " error:", e );
		return false;
	}

	WSABUF wsaBuf;
	while( buf.BeginRead( wsaBuf ) )
	{
		buf.EndRead( wsaBuf.len );

		LogNormal( "recv! buf:", string( wsaBuf.buf, wsaBuf.len ), ", len:", wsaBuf.len );

		//	{{ECHO_TEST}}
		WSABUF sendBuf;
		sendBuf.buf = new char[wsaBuf.len];
		sendBuf.len = wsaBuf.len;

		CopyMemory( sendBuf.buf, wsaBuf.buf, wsaBuf.len );
		sessionPtr->Send( sendBuf );
		//	{{ECHO_TEST}}
	}

	return true;
}

bool TcpServerService::_OnSend( const int e, const shared_ptr<TcpSession>& sessionPtr, const WSABUF& buf )
{
	delete[] buf.buf;

	if( e )
	{
		LogError( "send fail! id:", sessionPtr->GetId(), " error:", e );
		sessionPtr->Disconnect();
		return false;
	}

	return true;
}