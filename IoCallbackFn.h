#pragma once
#include <WinSock2.h>
#include <memory>
#include <functional>
using namespace std;

class TcpSession;
class CircularBuffer;

using IoCallbackFn = function<bool( const int, const shared_ptr<TcpSession>& )>;
using IoCallbackFnRecv = function<bool( const int, const shared_ptr<TcpSession>&, CircularBuffer& )>;
using IoCallbackFnSend = function<bool( const int, const shared_ptr<TcpSession>&, const WSABUF& )>;