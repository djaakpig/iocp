#include "IoService.h"

bool IoService::Associate (HANDLE h)
{
  auto result=CreateIoCompletionPort (h,iocpHandle,0,0);
  return iocpHandlt!=result;
}

bool IoService::Initialize (DWORD numWorkers)
{
  iocpHandle=CreateIoCompletionPort (INVALID_HANDLE_VALUE,NULL,0,numWorkers);
  if (NULL==iocpHandle)
    return false;
  for (DWORD workerIndex=0;numWorkers>workerIndex;++workerIndex)
  {
    threads.push_back (thread (bind (&IoService::_Run,this)));
  }
  return true;
}

bool IoService::Finalize ()
{
}