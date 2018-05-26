#pragma comment(lib, "ws2_32.lib") 
#pragma comment(lib, "mswsock.lib") 

#include <iostream>
#include <atomic>
#include <unordered_map>
#include "WinsockStarter.h"
#include "IoService.h"
#include "SockaddrIn.h"
#include "Socket.h"
#include "TcpListener.h"
#include "TcpSession.h"
#include "CircularBuffer.h"
#include "Lock.h"

class Service
{
public:
	explicit Service( IoService& ioService ) : _ioService( ioService )
	{
	}
	virtual ~Service() = default;

	shared_ptr<TcpSession> Find( const uint64_t id )
	{
		return DoShared( _lock, [this, id]()
		{
			const auto iter = _sessionMap.find( id );
			return _sessionMap.end() != iter ? iter->second : nullptr;
		} );
	}

	virtual bool Start( const SockaddrIn& listenAddr, const WORD numReserved ) = 0;
	virtual void Stop() = 0;

protected:
	shared_mutex _lock;
	IoService& _ioService;
	atomic_uint64_t _currentSessionId = 0;
	unordered_map<uint64_t, shared_ptr<TcpSession>> _sessionMap;
};

class Server final : public Service
{
	using Service::Service;

public:
	bool Start( const SockaddrIn& listenAddr, const WORD numReserved ) override
	{
		_listenerPtr = make_shared<TcpListener>();

		if( !_listenerPtr->Create() )
			return false;

		if( !_ioService.Associate( _listenerPtr.get() ) )
			return false;

		const auto acceptCallback = bind( &Server::_OnAccept, this, placeholders::_1, placeholders::_2 );

		if( !_listenerPtr->Listen( listenAddr, numReserved, acceptCallback ) )
			return false;

		cout << "SERVER start!" << endl;

		return true;
	}

	void Stop() override
	{
		DoShared( _lock, [this]()
		{
			for( const auto sessionPair : _sessionMap )
			{
				const auto sessionPtr = sessionPair.second;
				sessionPtr->Disconnect();
				sessionPtr->Close();
			}
		} );

		WaitCondition( 100, [this]()
		{
			return !_sessionMap.empty();
		} );

		if( _listenerPtr )
		{
			_listenerPtr->Close();
			_listenerPtr = nullptr;
		}
	}

private:
	bool _OnAccept( const int e, const shared_ptr<TcpSession>& sessionPtr )
	{
		if( e )
		{
			cout << "accept fail! error:" << e << endl;
			return false;
		}

		if( !_ioService.Associate( sessionPtr.get() ) )
		{
			cout << "accept associate fail!" << endl;
			return false;
		}

		sessionPtr->SetOnDisconnect(bind(&Server::_OnDisconnect, this, placeholders::_1, placeholders::_2));
		sessionPtr->SetOnRecv(bind(&Server::_OnRecv, this, placeholders::_1, placeholders::_2, placeholders::_3));
		sessionPtr->SetOnSend(bind(&Server::_OnSend, this, placeholders::_1, placeholders::_2, placeholders::_3));

		if( !sessionPtr->Recv() )
		{
			cout << "first recv fail!" << endl;
			sessionPtr->Disconnect();
			return false;
		}

		sessionPtr->SetId( ++_currentSessionId );

		DoExclusive( _lock, [this, sessionPtr]()
		{
			_sessionMap.emplace( sessionPtr->GetId(), sessionPtr );
		} );

		cout << "accept! id:" << sessionPtr->GetId() << endl;

		return true;
	}

	bool _OnDisconnect( const int e, const shared_ptr<TcpSession>& sessionPtr )
	{
		DoExclusive( _lock, [this, sessionPtr]()
		{
			_sessionMap.erase( sessionPtr->GetId() );
		} );

		if( e )
		{
			cout << "disconnect fail! id:" << sessionPtr->GetId() << " error:" << e << endl;
			return false;
		}

		return sessionPtr->Accept( _listenerPtr );
	}

	bool _OnRecv( const int e, const shared_ptr<TcpSession>& sessionPtr, CircularBuffer& buf )
	{
		if( e )
		{
			cout << "recv fail! error:" << e << endl;
			sessionPtr->Disconnect();
			return false;
		}

		WSABUF wsaBuf;
		while( buf.BeginRead( wsaBuf ) )
		{
			buf.EndRead( wsaBuf.len );

			cout << "recv! buf:" << string( wsaBuf.buf, wsaBuf.len ) << ", " << "len:" << wsaBuf.len << endl;

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

	bool _OnSend( const int e, const shared_ptr<TcpSession>& sessionPtr, const WSABUF& buf )
	{
		delete[] buf.buf;

		if( e )
		{
			cout << "send fail! error:" << e << endl;
			sessionPtr->Disconnect();
			return false;
		}

		return true;
	}

private:
	shared_ptr<TcpListener> _listenerPtr;
};

class Client final : public Service
{
	using Service::Service;

public:
	bool Start( const SockaddrIn& remoteAddr, const WORD numReserved ) override
	{
		if( !_socket.Create( SOCK_STREAM, IPPROTO_TCP ) )
			return false;

		if( !_extensionTable.Load( &_socket ) )
			return false;

		DoExclusive( _lock, [this, &remoteAddr, numReserved]()
		{
			for( WORD sessionId = 0; numReserved > sessionId; ++sessionId )
			{
				const auto sessionPtr = make_shared<TcpSession>( _extensionTable );

				if( !sessionPtr->Create() )
					continue;

				if( !sessionPtr->GetSocket()->Bind() )
					continue;

				if( !sessionPtr->GetSocket()->SetNonblock( true ) )
					continue;

				if( !_ioService.Associate( sessionPtr.get() ) )
					continue;

				sessionPtr->SetId( ++_currentSessionId );
				sessionPtr->SetOnConnect( bind( &Client::_OnConnect, this, placeholders::_1, placeholders::_2 ) );
				sessionPtr->SetOnDisconnect( bind( &Client::_OnDisconnect, this, placeholders::_1, placeholders::_2 ) );
				sessionPtr->SetOnRecv( bind( &Client::_OnRecv, this, placeholders::_1, placeholders::_2, placeholders::_3 ) );
				sessionPtr->SetOnSend( bind( &Client::_OnSend, this, placeholders::_1, placeholders::_2, placeholders::_3 ) );

				if( !sessionPtr->Connect( remoteAddr ) )
					continue;

				_sessionMap.emplace( sessionPtr->GetId(), sessionPtr );
			}
		} );

		cout << "CLIENT start!" << endl;

		return true;
	}

	void Stop() override
	{
		DoExclusive( _lock, [this]()
		{
			for( const auto sessionPair : _sessionMap )
			{
				const auto sessionPtr = sessionPair.second;
				sessionPtr->Disconnect();
				sessionPtr->Close();
			}
		} );

		WaitCondition( 100, [this]()
		{
			return !_sessionMap.empty();
		} );

		_socket.Close();
	}

private:
	bool _OnConnect( const int e, const shared_ptr<TcpSession>& sessionPtr )
	{
		if( e )
		{
			cout << "connect fail! error:" << e << endl;
			sessionPtr->Disconnect();
			return false;
		}

		if( !sessionPtr->GetSocket()->SetOptionNull( SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT ) )
		{
			cout << "UpdateConnectContext fail!" << endl;
			sessionPtr->Disconnect();
			return false;
		}

		if( !sessionPtr->Recv() )
		{
			cout << "first recv fail!" << endl;
			sessionPtr->Disconnect();
			return false;
		}

		cout << "connect! id:" << sessionPtr->GetId() << endl;

		//	{{SEND}}
		WSABUF sendBuf;
		sendBuf.buf = new char[10];
		sendBuf.len = 10;
		CopyMemory( sendBuf.buf, "1234567890", 10 );

		sessionPtr->Send( sendBuf );
		//	{{SEND}}

		return true;
	}

	bool _OnDisconnect( const int e, const shared_ptr<TcpSession>& sessionPtr )
	{
		DoExclusive( _lock, [this, sessionPtr]()
		{
			_sessionMap.erase( sessionPtr->GetId() );
		} );

		if( e )
		{
			cout << "disconnect fail! id:" << sessionPtr->GetId() << " error:" << e << endl;
			return false;
		}

		return true;
	}

	bool _OnRecv( const int e, const shared_ptr<TcpSession>& sessionPtr, CircularBuffer& buf )
	{
		if( e )
		{
			cout << "recv fail! error:" << e << endl;
			sessionPtr->Disconnect();
			return false;
		}

		WSABUF wsaBuf;
		while( buf.BeginRead( wsaBuf ) )
		{
			buf.EndRead( wsaBuf.len );

			cout << "recv! buf:" << string( wsaBuf.buf, wsaBuf.len ) << ", " << "len:" << wsaBuf.len << endl;
		}

		return true;
	}

	bool _OnSend( const int e, const shared_ptr<TcpSession>& sessionPtr, const WSABUF& buf )
	{
		delete[] buf.buf;

		if( e )
		{
			cout << "send fail! error:" << e << endl;
			sessionPtr->Disconnect();
			return false;
		}

		return true;
	}

	bool _SetSessionOption( const shared_ptr<TcpSession>& sessionPtr )
	{
		if( !sessionPtr->GetSocket()->SetOptionNull( SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT ) )
		{
			cout << "SetOptionNull fail!" << endl;
			return false;
		}

		if( !sessionPtr->GetSocket()->SetNonblock( true ) )
		{
			cout << "SetNonblock fail!" << endl;
			return false;
		}

		return true;
	}

private:
	Socket _socket;
	ExtensionTable _extensionTable;
};

int main( int argc, char** args )
{
	if( 6 != argc )
	{
		cout << args[0] << " [s or c] [numWorkers:1-4] [ip] [port:30000-40000] [numSessions:1-1000]" << endl;
		return 1;
	}

	const auto serverMode = (0 == _stricmp( args[1], "s" ));
	const DWORD numWorkers = static_cast<DWORD>(min( 4, max( 1, atoi( args[2] ) ) ));
	const string ip( args[3] );
	const WORD port = static_cast<WORD>(min( 40000, max( 30000, atoi( args[4] ) ) ));
	const WORD numSessions = static_cast<WORD>(min( 1000, max( 1, atoi( args[5] ) ) ));

	WinsockStarter wsStarter;
	IoService ioService;

	if( ioService.Start( numWorkers ) )
	{
		const SockaddrIn addr( ip, port );
		Service* pService = nullptr;

		if( serverMode )
			pService = new Server( ioService );
		else
			pService = new Client( ioService );

		if( pService->Start( addr, numSessions ) )
		{
			cout << "[press ENTER if you want to stop...]" << endl;

			getchar();

			cout << "stop!" << endl;

			pService->Stop();
		}

		delete pService;

		ioService.Stop();
	}

	return 0;
}
