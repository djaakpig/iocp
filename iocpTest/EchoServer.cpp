#include "EchoServer.h"
#include "Log.h"
#include "TcpSession.h"
#include "WsaBuf.h"

bool EchoServer::_OnPacket( const shared_ptr<TcpSession>& sessionPtr, const WSABUF& buf )
{
	LogNormal( "received packet! id:", sessionPtr->GetId(), ", len:", buf.len, ", msg:", string( buf.buf, buf.len ) );

	sessionPtr->Send( make_shared<WsaBuf>( buf ) );

	return true;
}