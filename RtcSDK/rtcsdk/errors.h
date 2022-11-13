#pragma once
#include<winerror.h>

namespace rtcsdk {

/**
 bad_hresult is the type of the object thrown as an error reporting a bad
 HRESULT has occurred.
 */
class bad_hresult
{
public:
    constexpr bad_hresult() = default;
    explicit constexpr bad_hresult(const HRESULT hr) noexcept : hr_{ hr }
    {
    }

    constexpr HRESULT hr() const noexcept
    {
        return hr_;
    }

    constexpr bool is_aborted() const noexcept
    {
        return hr_ == HRESULT_FROM_WIN32(ERROR_OPERATION_ABORTED);
    }

private:
    HRESULT hr_{ E_FAIL };
};

[[noreturn]]
inline void throw_bad_hresult(HRESULT hr)
{
    throw bad_hresult{ hr };
}

/**
 Encode window 32 error as HRESULT and throw bad_hresult.

 @param     err The win32 error.
 */
[[noreturn]]
inline void throw_win32_error(DWORD err)
{
    throw_bad_hresult(HRESULT_FROM_WIN32(err));
}

/** Throw last win32 error as bad_hresult. */
[[noreturn]]
inline void throw_last_error()
{
    throw_win32_error(GetLastError());
}

/**
 Throw bad_hresult on failed HRESULT.

 @param     hr  The HRESULT value.
 */
[[noreturn]]
inline void throw_on_failed(HRESULT hr)
{
    if (FAILED(hr))
    {
        throw_bad_hresult(hr);
    }
}

} // end of namespace rtcsdk

