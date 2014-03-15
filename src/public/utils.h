
#pragma once

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
    explicit RValPtrClass(T&& p) noexcept: m_ptr(std::forward<T>(p)) {}
    operator T*() noexcept { return &m_ptr; }
private:
    T m_ptr;
};

template<class T>
auto RValPtr(T&& p) -> RValPtrClass<T>
{
    return RValPtrClass<T>(std::forward<T>(p));
}

// Turns a pair of begin/end iterators into a class
// that can be used with c++11 for each loops.
// Example:
//      auto h = ForEachHelper(m_events.cbegin(), m_events.cbegin() + num_events);
//      for (auto &event : h) { ... }
//
template<class T>
class ForEachHelperClass
{
public:
    explicit ForEachHelperClass(T&& begin, T&& end) noexcept:
        m_begin(std::forward<T>(begin)),
        m_end(std::forward<T>(end))
    {
    }

    T begin() const noexcept { return m_begin; }
    T end() const noexcept { return m_end; }
private:
    T m_begin;
    T m_end;
};

template<class T>
auto ForEachHelper(T&& begin, T&& end) -> ForEachHelperClass<T>
{
    return ForEachHelperClass<T>(std::forward<T>(begin), std::forward<T>(end));
}
