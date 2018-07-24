#pragma once
#include <WinSock2.h>
#include <memory>
#include <functional>
using namespace std;

class TcpSession;
class CircularBuffer;

using TcpOperationCallback = function<bool( const int, const shared_ptr<TcpSession>& )>;
using TcpOperationCallbackRecv = function<bool( const int, const shared_ptr<TcpSession>&, CircularBuffer& )>;