#pragma once
#include "IoCallbackImpl.hpp"
#include "IoCallbackFn.h"

class ExtensionTable;

class IoCallbackDisconnect final : public IoCallbackImpl<IoCallbackFn>
{
public:
	void OnComplete( const int e ) override;
	bool Post( const shared_ptr<ExtensionTable>& extensionTablePtr );
};