#include "IoService.h"
#include "IoCallback.h"
#include "IIoObject.h"

bool IoService::Associate(HANDLE h)
{
  const auto result=CreateIoCompletionPort(h, iocpHandle, 0, 0);
  return iocpHandle!=result;
}

bool IoService::Start(DWORD numWorkers)
{
 iocpHandle=CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, numWorkers);
  if(nullptr==iocpHandle)
    return false;

  for(DWORD workerId=0; numWorkers>workerId; ++workerId)
    workers.push_back(thread(bind(&IoService::_Run,this)));

  return true;
}

bool IoService::Stop()
{
  for(size_t workerId=0; workers.size()>workerId; ++workerId)
    PostQueuedCompletionStatus(iocpHandle, 0, 0, nullptr);

  workers.clear();

  if(nullptr!=iocpHandle)
  {
    CloseHandle(iocpHandle);
    iocpHandle=nullptr;
  }
}

void IoService::_Run()
{
  DWORD numBytes=0;
  ULONGPTR cmpl=0;
  POVERLAPPED pOvl=nullptr;

  while(true)
  {
    const auto r=GetQueuedCompletionStatus(iocpHandle,
                                           &numBytes,
                                           &cmpl,
                                           &pOvl,
                                           INFINITE);
    if(!r || 0==cmpl || nullptr==pOvl)
      break;

    const auto pCallback=reinterpret_cast<IoCallback*>(pOvl);
    const auto pObj=reinterpret_cast<IIoObject*>(cmpl);
    pCallback->OnComplete(pObj, numBytes);
  }
}