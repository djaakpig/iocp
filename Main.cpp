#pragma comment(lib, "ws2_32.lib") 
#pragma comment(lib, "mswsock.lib") 

#include "Arguments.h"
#include "WinsockStarter.h"
#include "IoService.h"
#include "TcpServerService.h"
#include "TcpClientService.h"

int main( int argc, char** args )
{
	Arguments arguments;

	if( !arguments.Load( argc, args ) )
		return 1;

	SetLogLevel( arguments.GetLogLevel() );

	WinsockStarter wsStarter;
	IoService ioService;

	if( ioService.Start( arguments.GetNumWorkers() ) )
	{
		shared_ptr<TcpSessionService> servicePtr;

		if( arguments.IsServerMode() )
			servicePtr = make_shared<TcpServerService>( ioService );
		else
			servicePtr = make_shared<TcpClientService>( ioService );

		if( servicePtr->Start( arguments.GetAddr(), arguments.GetNumSessions() ) )
		{
			LogForce( "[press ENTER if you want to stop...]" );

			getchar();

			LogForce( "stop!" );

			servicePtr->Stop();
		}

		ioService.Stop();
	}

	return 0;
}