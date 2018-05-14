#include "IoService.h"

bool IoService::Associate (HANDLE h)
{
  auto result=CreateIoCompletionPort (h,iocpHandle,0,0);
  return iocpHandle!=result;
}

bool IoService::Initialize (DWORD numWorkers)
{
  iocpHandle=CreateIoCompletionPort (INVALID_HANDLE_VALUE,NULL,0,numWorkers);
  if (NULL==iocpHandle)
    return false;
  for (DWORD workerIndex=0;numWorkers>workerIndex;++workerIndex)
  {
    workers.push_back (thread (bind (&IoService::_Run,this)));
  }
  return true;
}

bool IoService::Finalize ()
{
  for (auto ii=0;workers.size ()>ii;++ii)
    PostQueuedCompletionStatus (iocpHandle,);
  workers.clear ();
}