#include "EchoClient.h"
#include "Log.h"
#include "TcpSession.h"
#include "WsaBuf.h"

bool EchoClient::_OnConnect(const int32_t e, const std::shared_ptr<TcpSession>& sessionPtr)
{
	if(!__super::_OnConnect(e, sessionPtr))
		return false;

	//	{{ECHO_TEST}}
	sessionPtr->Send(std::make_shared<WsaBuf>("1234567890", 10));
	//	{{ECHO_TEST}}

	return true;
}

bool EchoClient::_OnPacket(const std::shared_ptr<TcpSession>& sessionPtr, const WSABUF& buf)
{
	LogNormal("received packet! id:", sessionPtr->GetId(), ", len:", buf.len, ", msg:", std::string{buf.buf, buf.len});

	return true;
}