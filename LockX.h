#pragma once
#include "pch.h"

class LockX
{
public:
    LockX();
    ~LockX();

    void Acquire();
    void Release();
protected:
    CRITICAL_SECTION m_CriticalSection;
};
