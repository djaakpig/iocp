#pragma comment(lib, "ws2_32.lib") 
#pragma comment(lib, "mswsock.lib") 

#include <iostream>
#include "WinsockStarter.h"
#include "IoService.h"
#include "TcpListener.h"
#include "TcpSession.h"


class Server
{
public:
	explicit Server( IoService& ioService ) : _ioService( ioService )
	{
	}
	~Server()
	{
		if( listenerPtr )
		{
			listenerPtr->Close();
			listenerPtr = nullptr;
		}
	}

	bool Start()
	{
		listenerPtr = make_shared<TcpListener>();
		if( !listenerPtr->Create() )
			return false;

		_ioService.Associate( listenerPtr.get() );

		const auto acceptCallback = bind( &Server::OnAccept, this, placeholders::_1, placeholders::_2 );
		if( !listenerPtr->Start( "10.6.5.88", 8801, 5, acceptCallback ) )
			return false;

		return true;
	}

private:
	bool OnAccept( const int e, shared_ptr<TcpSession> sessionPtr )
	{
		if( e )
		{
			cout << "accept fail! error:" << e << endl;
			return false;
		}

		cout << "accept! numBytes:" << numBytes << endl;

		_ioService.Associate( sessionPtr.get() );
		sessionPtr->Recv( bind( &Server::OnRecv, this, placeholders::_1, placeholders::_2, placeholders::_3 ) );

		return true;
	}

	bool OnRecv( const int e, shared_ptr<TcpSession> sessionPtr, CircularBuffer& buf )
	{
		if( e )
		{
			cout << "recv fail! error:" << e << endl;
			return false;
		}

		cout << "recv! numBytes:" << buf.GetSize() << endl;

        while( !buf.IsEmpty() )
        {
            //TODO: parsing...
        }

		sessionPtr->Recv( bind( &Server::OnRecv, this, placeholders::_1, placeholders::_2, placeholders::_3 ) );

		return true;
	}

    bool OnSend(const int e, const WSABUF& buf)
    {
        delete[] buf.buf;

        if( e )
		{
			cout << "send fail! error:" << e << endl;
			return false;
		}

        //  TODO: continue to send a next packet in queue if the queue is not empty.

        return true;
    }

private:
	IoService& _ioService;
	shared_ptr<TcpListener> listenerPtr;
};

int main()
{
	WinsockStarter wsockStarter( 2, 2 );
	IoService ioService;

	if( ioService.Start( 4 ) )
	{
		Server theServer( ioService );
		if( theServer.Start() )
		{
			cout << "hi~" << endl;
			while( true );
		}

		ioService.Stop();
	}

	return 0;
}