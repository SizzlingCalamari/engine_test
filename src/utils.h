
#pragma once

class RunOnce
{
public:
    template<typename T>
    explicit inline RunOnce(T fn) { fn(); }
};

template<class T>
class RValPtr
{
public:
    RValPtr(const T& p): m_ptr(p) {}
    operator T*() { return &m_ptr; }
private:
    T m_ptr;
};

typedef RValPtr<const char*> StrRValPtr;
