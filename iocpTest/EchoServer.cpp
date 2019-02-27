#include "EchoServer.h"
#include "Log.h"
#include "TcpSession.h"
#include "WsaBuf.h"

bool EchoServer::_OnPacket(const std::shared_ptr<TcpSession>& sessionPtr, const WSABUF& buf)
{
	LogNormal("received packet! id:", sessionPtr->GetId(), ", len:", buf.len, ", msg:", std::string{buf.buf, buf.len});

	//	{{ECHO_TEST}}
	sessionPtr->Send(std::make_shared<WsaBuf>(buf));
	//	{{ECHO_TEST}}

	return true;
}