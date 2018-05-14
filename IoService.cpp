#include "IoService.h"

bool IoService::Associate(HANDLE h)
{
  auto result=CreateIoCompletionPort(h,iocpHandle,0,0);
  return iocpHandle!=result;
}

bool IoService::Initialize(DWORD numWorkers)
{
  iocpHandle=CreateIoCompletionPort(INVALID_HANDLE_VALUE,nullptr,0,numWorkers);
  if(NULL==iocpHandle)
    return false;
  for(DWORD workerIndex=0;numWorkers>workerIndex;++workerIndex)
    workers.push_back(thread(bind(&IoService::_Run,this)));
  return true;
}

bool IoService::Finalize()
{
  for(auto ii=0;workers.size ()>ii;++ii)
    PostQueuedCompletionStatus(iocpHandle,0,0,nullptr);
  workers.clear();

  if(nullptr!=iocpHandle)
  {
    CloseHandle(iocpHandle);
    iocHandle=nullptr;
  }
}

void IoService::_Run()
{
  while(true)
  {
    const auto r=GetQueuedCompletionStatus(iocpHandle,&numBytes,&cmpl,&ovl,INFINITE);
    if(!r||0==cmpl||nullptr==ovl)
      break;
    auto pOvl=reinterpret_cast<IoOvl*>(ovl);
    auto pOwn=reinterpret_cast<IoOwn*>(cmpl);
    pOvl->OnComplete(pOwn);
  }
}