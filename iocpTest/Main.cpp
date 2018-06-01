#pragma comment(lib, "ws2_32.lib") 
#pragma comment(lib, "mswsock.lib") 
#pragma comment(lib, "iocp.lib") 

#include <WinsockStarter.h>
#include <IoService.h>
#include <Log.h>
#include "Arguments.h"
#include "EchoServer.h"
#include "EchoClient.h"

int main( int argc, char** args )
{
	Arguments arguments;

	if( !arguments.Load( argc, args ) )
		return 1;

	SetConsoleTitleA( arguments.ToTitle().c_str() );
	SetLogLevel( arguments.GetLogLevel() );

	WinsockStarter wsStarter;
	IoService ioService;

	if( ioService.Start( arguments.GetNumWorkers() ) )
	{
		shared_ptr<TcpSessionService> servicePtr;

		if( arguments.IsServerMode() )
			servicePtr = make_shared<EchoServer>( ioService );
		else
			servicePtr = make_shared<EchoClient>( ioService );

		if( servicePtr->Start( arguments.GetAddr(), arguments.GetNumSessions() ) )
		{
			cout << "[Press ENTER if you want to stop...]" << endl;

			getchar();

			cout << "[Stop...]" << endl;
		}

		servicePtr->Stop();
	}

	ioService.Stop();

	return 0;
}