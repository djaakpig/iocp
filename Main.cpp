#pragma comment(lib, "ws2_32.lib") 
#pragma comment(lib, "mswsock.lib") 

#include "Arguments.h"
#include "WinsockStarter.h"
#include "IoService.h"
#include "TcpServerService.h"
#include "TcpClientService.h"
#include "Log.h"

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
			servicePtr = make_shared<TcpServerService>( ioService );
		else
			servicePtr = make_shared<TcpClientService>( ioService );

		if( servicePtr->Start( arguments.GetAddr(), arguments.GetNumSessions() ) )
		{
			cout << "[Press ENTER if you want to stop...]" << endl;

			getchar();
			//this_thread::sleep_for( chrono::milliseconds( 2000 ) );

			cout << "[Stop...]" << endl;
		}

		servicePtr->Stop();
	}

	ioService.Stop();

	return 0;
}