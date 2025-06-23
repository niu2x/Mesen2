#pragma once
#include "pch.h"
#include "Debugger/Debugger.h"

class DebugBreakHelper
{
private:
	Debugger * _debugger;
	bool _needBreak = false;

public:
	DebugBreakHelper(Debugger* debugger, bool breakBetweenInstructions = false);

	~DebugBreakHelper()
	{
		if(_needBreak) {
			_debugger->BreakRequest(true);
		}
	}
};