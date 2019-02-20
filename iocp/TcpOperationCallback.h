#pragma once
#include <WinSock2.h>
#include <memory>
#include <functional>

class TcpSession;
class CircularBuffer;

using TcpOperationCallback = std::function<bool( const int, const std::shared_ptr<TcpSession>& )>;
using TcpOperationCallbackRecv = std::function<bool( const int, const std::shared_ptr<TcpSession>&, CircularBuffer& )>;