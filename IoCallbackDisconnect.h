#pragma once
#include "IoCallbackImpl.hpp"
#include "IoCallbackFn.h"

struct ExtensionTable;
class IoCallbackDisconnect final : public IoCallbackImpl<IoCallbackFn>
{
public:
	bool OnComplete( const int e ) override;
	bool Post( const ExtensionTable& extensionTable );
};