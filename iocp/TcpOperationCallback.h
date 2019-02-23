#pragma once
#include <memory>
#include <functional>

class TcpSession;
using TcpOperationCallback = std::function<bool(const int32_t, const std::shared_ptr<TcpSession>&)>;