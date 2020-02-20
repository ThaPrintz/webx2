#include "pch.h"

LockX::LockX()
{
    InitializeCriticalSection(&this->m_CriticalSection);
}

LockX::~LockX()
{
    DeleteCriticalSection(&this->m_CriticalSection);
}

void LockX::Acquire()
{
    EnterCriticalSection(&this->m_CriticalSection);
}

void LockX::Release()
{
    LeaveCriticalSection(&this->m_CriticalSection);
}