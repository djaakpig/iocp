#pragma once
#include "IIoObject.h"
#include <string>
using namespace std;

class Socket;
class TcpListener final : public IIoObject
{
public:
	TcpListener();
	~TcpListener();

	HANDLE GetHandle() const override;
	bool Start(const string& ip, const WORD port, const WORD numReserved);
	void Stop();

private:
	Socket* _pSocket = nullptr;
};