
#pragma once

#include <utility>

// Runs a section of code only once. Passing over it after the first run.
// Create static RunOnce variable and place in method scope.
// Example:
//      static RunOnce glew([]
//      {
//          glewExperimental = true;
//          glewInit();
//      });
//
class RunOnce
{
public:
    template<typename T>
    explicit inline RunOnce(const T& fn) noexcept { fn(); }
};

// Creates and returns a pointer to an rvalue.
// Example:
//      void func(char **str);
//      char c;
//      func(RValPtr(&c));
//
template<class T>
class RValPtrClass
{
public:
    explicit inline RValPtrClass(T&& p) noexcept: m_ptr(std::forward<T>(p)) {}
    inline operator T*() noexcept { return &m_ptr; }
private:
    T m_ptr;
};

template<class T>
inline auto RValPtr(T&& p) -> RValPtrClass<T>
{
    return RValPtrClass<T>(std::forward<T>(p));
}

// Turns a pair of begin/end iterators into a class
// that can be used with c++11 for each loops.
// Example:
//      auto h = IteratorPair(m_events.cbegin(), m_events.cbegin() + num_events);
//      for (auto &event : h) { ... }
//
template<class T>
class IteratorPair
{
public:
    explicit inline IteratorPair(T begin, T end) noexcept :
        m_begin(begin),
        m_end(end)
    {
    }

    inline T begin() const noexcept { return m_begin; }
    inline T end() const noexcept { return m_end; }
private:
    T m_begin;
    T m_end;
};

template<class T>
inline auto IteratorPairHelper(T begin, T end) -> IteratorPair<T>
{
    return IteratorPair<T>(begin, end);
}

namespace utils
{
    template<typename T>
    inline T min(T a, T b)
    {
        return (a < b ? a : b);
    }

    template<typename T>
    inline T max(T a, T b)
    {
        return (a > b ? a : b);
    }

    template<typename T>
    inline void lessswap(T& a, T& b)
    {
        const T x = min(a, b);
        const T y = max(a, b);
        a = x;
        b = y;
    }

    //
    // sorting networks from
    // http://pages.ripco.net/~jgamble/nw.html
    //
    template<typename T>
    void sort4(T* elems)
    {
        lessswap(elems[0], elems[1]);
        lessswap(elems[2], elems[3]);
        lessswap(elems[0], elems[2]);
        lessswap(elems[1], elems[3]);
        lessswap(elems[1], elems[2]);
    }
}
