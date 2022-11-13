#pragma once

#include <exception>

/** The following code adapted from Andrei Alexandrescu talk on CppCon 2015. */
namespace rtcsdk::details {

class UncaughtExceptionsCounter
{
public:
    bool has_new_exceptions() const noexcept
    {
        return std::uncaught_exceptions() > exceptions_on_enter_;
    }

private:
    int exceptions_on_enter_{ std::uncaught_exceptions() };
};

template<typename F, bool execute_on_exception>
class scope_guard
{
public:
    explicit scope_guard(const F& f) noexcept : f_{ f }
    {
    }

    explicit scope_guard(F&& f) noexcept : f_{ std::move(f) }
    {
    }

    ~scope_guard() noexcept(execute_on_exception)
    {
        if (execute_on_exception == counter_.has_new_exceptions())
        {
            f_();
        }
    }

private:
    F f_;
    UncaughtExceptionsCounter counter_;
};

template<typename F>
class scope_exit
{
public:
    explicit scope_exit(const F& f) noexcept : f_{ f }
    {
    }

    explicit scope_exit(F&& f) noexcept : f_{ std::move(f) }
    {
    }

    ~scope_exit() noexcept
    {
        f_();
    }

private:
    F f_;
};

template<typename F>
class scope_exit_cancellable
{
public:
    explicit scope_exit_cancellable(const F& f) noexcept : f_{ f }
    {
    }

    explicit scope_exit_cancellable(F&& f) noexcept : f_{ std::move(f) }
    {
    }

    void cancel()
    {
        cancelled_ = true;
    }

    ~scope_exit_cancellable() noexcept
    {
        if (!cancelled_)
        {
            f_();
        }
    }

private:
    F f_;
    bool cancelled_{ false };
};

enum class ScopeGuardOnFail
{
};

template<typename F>
inline scope_guard<std::decay_t<F>, true> operator +(ScopeGuardOnFail, F&& f) noexcept
{
    return scope_guard<std::decay_t<F>, true>(std::forward<F>(f));
}

enum class ScopeGuardOnSuccess
{
};

template<typename F>
inline scope_guard<std::decay_t<F>, false> operator +(ScopeGuardOnSuccess, F&& f) noexcept
{
    return scope_guard<std::decay_t<F>, false>(std::forward<F>(f));
}

enum class ScopeGuardOnExit
{
};

template<typename F>
inline scope_exit<std::decay_t<F>> operator +(ScopeGuardOnExit, F&& f) noexcept
{
    return scope_exit<std::decay_t<F>>(std::forward<F>(f));
}

enum class ScopeGuardOnExitCancellable
{
};

template<typename F>
inline scope_exit_cancellable<std::decay_t<F>> operator +(ScopeGuardOnExitCancellable, F&& f) noexcept
{
    return scope_exit_cancellable<std::decay_t<F>>(std::forward<F>(f));
}
}

#define PP_CAT(a,b) a##b
#define ANONYMOUS_VARIABLE(x) PP_CAT(x,__COUNTER__)

#define SCOPE_FAIL \
auto ANONYMOUS_VARIABLE(SCOPE_FAIL_STATE) \
= ::rtcsdk::details::ScopeGuardOnFail() + [&]() noexcept \
// end of macro

#define SCOPE_SUCCESS \
auto ANONYMOUS_VARIABLE(SCOPE_SUCCESS_STATE) \
= ::rtcsdk::details::ScopeGuardOnSuccess() + [&]() \
// end of macro

#define SCOPE_EXIT \
auto ANONYMOUS_VARIABLE(SCOPE_EXIT_STATE) \
= ::rtcsdk::details::ScopeGuardOnExit() + [&]() noexcept \
// end of macro

#define SCOPE_EXIT_CANCELLABLE(name) \
auto name\
= ::rtcsdk::details::ScopeGuardOnExitCancellable() + [&]() noexcept \
// end of macro
