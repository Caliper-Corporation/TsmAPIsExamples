#pragma once
#include<winerror.h>

namespace rtcsdk {

namespace details {

/**
 bad_hresult is the type of the object thrown as an error reporting a bad
 HRESULT has occurred.
 */
class bad_hresult
{
public:
    constexpr bad_hresult() = default;
    explicit constexpr bad_hresult(const HRESULT hr) noexcept : hr_{ hr } {}

    constexpr auto hr() const noexcept -> HRESULT
    {
        return hr_;
    }

    constexpr auto is_aborted() const noexcept -> bool
    {
        return hr_ == HRESULT_FROM_WIN32(ERROR_OPERATION_ABORTED);
    }

private:
    HRESULT hr_{ E_FAIL };
};

[[noreturn]] inline auto throw_bad_hresult(HRESULT hr) -> void
{
    throw bad_hresult{ hr };
}

/**
 Encode window 32 error as HRESULT and throw bad_hresult.

 @param     err The win32 error.
 */
[[noreturn]] inline auto throw_win32_error(DWORD err) -> void
{
    throw_bad_hresult(HRESULT_FROM_WIN32(err));
}

/** Throw last win32 error as bad_hresult. */
[[noreturn]] inline auto throw_last_error() -> void
{
    throw_win32_error(GetLastError());
}

/**
 Throw bad_hresult on failed HRESULT.

 @param     hr  The HRESULT value.
 */
inline auto throw_on_failed(HRESULT hr) -> void
{
    if (FAILED(hr)) {
        throw_bad_hresult(hr);
    }
}

} // end of namespace rtcsdk::details

using details::bad_hresult;
using details::throw_bad_hresult;
using details::throw_win32_error;
using details::throw_last_error;
using details::throw_on_failed;

}
