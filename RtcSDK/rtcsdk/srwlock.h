#pragma once

namespace rtcsdk {

/**
 Windows slim reader-writer lock wrapper.Friendly to be used as
 shared_mutex.
 */
class srwlock
{
public:
  srwlock(const srwlock &) = delete;
  srwlock &operator=(const srwlock &) = delete;
  srwlock() noexcept = default;

  /*
     Annotated Locking Behavior using SAL. See the following doc for details
     https://learn.microsoft.com/en-us/cpp/code-quality/annotating-locking-behavior?view=msvc-170
    */

  _Acquires_exclusive_lock_(&lock_) void lock() noexcept
  {
    AcquireSRWLockExclusive(&lock_);
  }

  _Acquires_shared_lock_(&lock_) void lock_shared() noexcept
  {
    AcquireSRWLockShared(&lock_);
  }

  _When_(return, _Acquires_exclusive_lock_(&lock_)) bool try_lock() noexcept
  {
    return 0 != TryAcquireSRWLockExclusive(&lock_);
  }

  _Releases_exclusive_lock_(&lock_) void unlock() noexcept
  {
    ReleaseSRWLockExclusive(&lock_);
  }

  _Releases_shared_lock_(&lock_) void unlock_shared() noexcept
  {
    ReleaseSRWLockShared(&lock_);
  }

private:
  SRWLOCK lock_{};
};

}// namespace rtcsdk
