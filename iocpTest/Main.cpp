#include <WinsockStarter.h>
#include <ThreadPool.h>
#include <Log.h>
#include "Arguments.h"
#include "EchoServer.h"
#include "EchoClient.h"

#pragma comment(lib, "iocp.lib")

int main(int argc, char** args)
{
	Arguments arguments;

	if(!arguments.Load(argc, args))
	{
		return 1;
	}

	SetConsoleTitleA(arguments.ToTitle().c_str());
	SetLogLevel(arguments.GetLogLevel());

	WinsockStarter wsStarter;
	ThreadPool threadPool;

	if(threadPool.Start(arguments.GetNumWorkers()))
	{
		std::shared_ptr<TcpSessionService> service;

		if(arguments.IsServerMode())
		{
			service = std::make_shared<EchoServer>(threadPool);
		}
		else
		{
			service = std::make_shared<EchoClient>(threadPool);
		}

		if(service->Start(arguments.GetAddr(), arguments.GetNumSessions()))
		{
			std::cout << "[Press ENTER if you want to stop...]" << std::endl;

			getchar();

			std::cout << "[Stop...]" << std::endl;
		}

		service->Stop();
	}

	threadPool.Stop();

	return 0;
}