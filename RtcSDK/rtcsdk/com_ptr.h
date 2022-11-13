#pragma once

#if !defined(RTCSDK_COM_NO_LEAK_DETECTION) && defined(_DEBUG)
#define RTCSDK_HAS_LEAK_DETECTION 1
#else
#define RTCSDK_HAS_LEAK_DETECTION 0
#endif

#if !defined(RTCSDK_COM_NO_CHECKED_REFS) && defined(_DEBUG)
#define RTCSDK_HAS_CHECKED_REFS 1
#else
#define RTCSDK_HAS_CHECKED_REFS 0
#endif


#if RTCSDK_HAS_LEAK_DETECTION
#include <atomic>
#include <mutex>

// If you get a compilation error at the following line, do one of the following:
//		- Add boost 1.73.0 or later to your project
//		or
//		- #define RTCSDK_COM_NO_LEAK_DETECTION before including any library's header to disable leak detection
#include <boost/stacktrace.hpp>
#endif

#if RTCSDK_HAS_CHECKED_REFS
#include <vector>
#endif

#include <cassert>
#include <unknwn.h>

#include <rtcsdk/guid.h>
#include <rtcsdk/srwlock.h>
#include <rtcsdk/onexit.h>
#include <rtcsdk/errors.h>

namespace rtcsdk {

namespace details {

template<typename Interface>
class com_ptr;

#if RTCSDK_HAS_LEAK_DETECTION

struct leak_detection
{
    int ordinal;
    boost::stacktrace::stacktrace stack;

    static int get_next() noexcept
    {
        static std::atomic<int> global_ordinal{};
        return ++global_ordinal;
    }

    leak_detection() noexcept : ordinal{ get_next() }, stack{ boost::stacktrace::stacktrace() }
    {
    }
};

inline void init_leak_detection() noexcept
{
    using namespace std::literals;
    SYSTEM_INFO si{};
    GetSystemInfo(&si);
    HANDLE section = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, si.dwPageSize, (L"BELT_LEAK_DETECTION_SECTION."s + std::to_wstring(GetCurrentProcessId())).c_str());
    *reinterpret_cast<DWORD*>(MapViewOfFile(section, FILE_MAP_WRITE, 0, 0, si.dwPageSize)) = TlsAlloc();
    // We intentionally leave section mapped
}

inline DWORD get_slot() noexcept
{
    using namespace std::literals;
    SYSTEM_INFO si{};
    GetSystemInfo(&si);
    HANDLE section = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, si.dwPageSize, (L"BELT_LEAK_DETECTION_SECTION."s + std::to_wstring(GetCurrentProcessId())).c_str());
    assert(section && GetLastError() == ERROR_ALREADY_EXISTS && "init_leak_detection() must be called from EXE module as early as possible! It has not yet been called.");
    auto ptr = reinterpret_cast<DWORD*>(MapViewOfFile(section, FILE_MAP_WRITE, 0, 0, si.dwPageSize));
    assert(ptr);
    CloseHandle(section);
    auto ret = *ptr;
    UnmapViewOfFile(ptr);
    return ret;
}

inline void set_current_cookie(int cookie) noexcept
{
    static DWORD slot = get_slot();
    TlsSetValue(slot, reinterpret_cast<void*>(static_cast<uintptr_t>(cookie)));
}

inline int get_current_cookie() noexcept
{
    static DWORD slot = get_slot();
    auto retval = TlsGetValue(slot);
    TlsSetValue(slot, nullptr);
    return static_cast<int>(reinterpret_cast<uint64_t>(retval));
}
#else
inline void init_leak_detection() noexcept
{
}
#endif

struct attach_t
{
};

template<typename Interface>
class ref;

template<typename Interface>
class __declspec(empty_bases)com_ptr
{
    friend class ref<Interface>;

    friend Interface*& internal_get(com_ptr<Interface>& obj) noexcept
    {
        return obj.p_;
    }

private:
    Interface* p_{};

#if RTCSDK_HAS_CHECKED_REFS
    std::vector<ref<Interface>*> weaks_;
#endif

#if RTCSDK_HAS_LEAK_DETECTION
    int cookie_{};

    friend int& internal_get_cookie(com_ptr<Interface>& obj) noexcept
    {
        return obj.cookie_;
    }
#endif

    // smart construction from other interfaces
    template<typename OtherInterface>
    com_ptr(OtherInterface* punk, std::true_type) noexcept : p_{ static_cast<Interface*>(punk) }
    {
        addref_pointer(p_);
    }

    template<typename OtherInterface>
    com_ptr(OtherInterface* punk, std::false_type) noexcept
    {
        if (punk) {
            punk->QueryInterface(get_interface_guid(interface_wrapper<Interface>{}), reinterpret_cast<void**>(&p_));
            store_cookie();
        }
    }
    //

    //
    template<typename OtherInterface>
    com_ptr(com_ptr<OtherInterface>&& o, std::false_type) noexcept
    {
        if (o) {
            if (SUCCEEDED(o->QueryInterface(get_interface_guid(interface_wrapper<Interface>{}), reinterpret_cast<void**>(&p_)))) {
                store_cookie();
                o.release();
            }
        }
    }

    template<typename OtherInterface>
    com_ptr(com_ptr<OtherInterface>&& o, std::true_type) noexcept : p_{ static_cast<Interface*>(o.get()) }
#if RTCSDK_HAS_LEAK_DETECTION
        , cookie_{ internal_get_cookie(o) }
#endif
    {
        internal_get(o) = nullptr;
#if RTCSDK_HAS_LEAK_DETECTION
        internal_get_cookie(o) = 0;
#endif
    }
    //
    template<typename OtherInterface>
    com_ptr(const com_ptr<OtherInterface>& o, std::false_type) noexcept
    {
        if (o) {
            o->QueryInterface(get_interface_guid(interface_wrapper<Interface>{}), reinterpret_cast<void**>(&p_));
            store_cookie();
        }
    }

    template<typename OtherInterface>
    com_ptr(const com_ptr<OtherInterface>& o, std::true_type) noexcept :
        p_{ static_cast<Interface*>(o.get()) }
    {
        addref_pointer(p_);
    }

#if RTCSDK_HAS_LEAK_DETECTION
    void store_cookie() noexcept
    {
        cookie_ = get_current_cookie();
    }
#else
    static void store_cookie() noexcept
    {
    }
#endif

    void addref_pointer(IUnknown* pint) noexcept
    {
        if (pint) {
            pint->AddRef();
            store_cookie();
        }
    }

    void release_pointer(IUnknown* pint) noexcept
    {
        if (pint) {
#if RTCSDK_HAS_LEAK_DETECTION
            set_current_cookie(std::exchange(cookie_, 0));
#endif
            pint->Release();
        }
    }

public:
    explicit operator bool() const noexcept
    {
        return !!p_;
    }

    com_ptr() = default;
    com_ptr(std::nullptr_t) noexcept
    {
    }

    com_ptr(Interface* p) noexcept :
        p_{ p }
    {
        addref_pointer(p);
    }

    // the following constructor does not call addref (attaching constructor)
    com_ptr(attach_t, Interface* p) noexcept : p_{ p }
    {
        store_cookie();		// might get incorrect cookie, check
    }

    template<typename OtherInterface>
    com_ptr(OtherInterface* punk) noexcept : com_ptr(punk, std::is_base_of<Interface, OtherInterface>{})
    {
    }

    template<typename OtherInterface>
    com_ptr(const com_ptr<OtherInterface>& o) noexcept : com_ptr(o, std::is_base_of<Interface, OtherInterface>{})
    {
    }

    template<typename OtherInterface>
    com_ptr(com_ptr<OtherInterface>&& o) noexcept : com_ptr(std::move(o), std::is_base_of<Interface, OtherInterface>{})
    {
    }

    void release() noexcept
    {
        if (p_) {
            release_pointer(p_);
            p_ = nullptr;
        }
    }

    void reset() noexcept
    {
        release();
    }

    ~com_ptr() noexcept
    {
        release_pointer(p_);
#if BELT_HAS_CHECKED_REFS
        assert(weaks.empty() && "There was ref<Interface> constructed from this com_ptr that outlived this object!");
#endif
    }

    com_ptr(const com_ptr& o) noexcept : p_{ o.p_ }
    {
        addref_pointer(p_);
    }

    template<typename OtherInterface>
    com_ptr(ref<OtherInterface> o) noexcept;

    com_ptr& operator =(const com_ptr& o) noexcept
    {
        release_pointer(p_);
        p_ = o.p_;
        addref_pointer(p_);
        return *this;
    }

    com_ptr(com_ptr&& o) noexcept : p_{ o.p_ }
#if RTCSDK_HAS_LEAK_DETECTION
        , cookie_{ o.cookie_ }
#endif
    {
        o.p_ = nullptr;
#if RTCSDK_HAS_LEAK_DETECTION
        o.cookie_ = 0;
#endif
    }

    com_ptr& operator =(com_ptr&& o) noexcept
    {
#if RTCSDK_HAS_LEAK_DETECTION
        std::swap(cookie_, o.cookie_);
#endif
        std::swap(p_, o.p_);
        return *this;
    }

    Interface* operator ->() const noexcept
    {
        return p_;
    }

    // Comparison operators
    bool operator ==(const com_ptr& o) const noexcept
    {
        return p_ == o.p_;
    }

    bool operator !=(const com_ptr& o) const noexcept
    {
        return p_ != o.p_;
    }

    bool operator ==(Interface* pother) const noexcept
    {
        return p_ == pother;
    }

    bool operator !=(Interface* pother) const noexcept
    {
        return p_ != pother;
    }

    friend bool operator ==(Interface* p1, const com_ptr& p2) noexcept
    {
        return p1 == p2.p_;
    }

    friend bool operator !=(Interface* p1, const com_ptr& p2) noexcept
    {
        return p1 != p2.p_;
    }

    // Ordering operator
    bool operator<(const com_ptr& o) const noexcept
    {
        return p_ < o.p_;
    }

    // Attach and detach operations
    void attach(Interface* p_) noexcept
    {
        assert(!p_);
        p_ = p_;
        store_cookie();	// might get incorrect cookie
    }

    [[nodiscard]]
    Interface* detach() noexcept
    {
        Interface* cur{};
        std::swap(cur, p_);
#if BELT_HAS_LEAK_DETECTION
        cookie = 0;
#endif

        return cur;
    }

    // Pointer accessors
    Interface* get() const noexcept
    {
        return p_;
    }

#if RTCSDK_HAS_LEAK_DETECTION
    int get_cookie() const noexcept
    {
        return cookie_;
    }
#endif

    Interface** put() noexcept
    {
        assert(!p_ && "Using put on non-empty object is prohibited");
        return &p_;
    }

    // Conversion operations
    template<typename Interface>
    auto as() const noexcept
    {
        return com_ptr<Interface>{*this};
    }

    template<typename T>
    HRESULT QueryInterface(T** ppresult) const
    {
        return p_->QueryInterface(get_interface_guid(interface_wrapper<T>{}), reinterpret_cast<void**>(ppresult));
    }

    HRESULT CoCreateInstance(const GUID& clsid, IUnknown* pUnkOuter = nullptr, DWORD dwClsContext = CLSCTX_ALL) noexcept
    {
        assert(!p_ && "Calling CoCreateInstance on initialized object is prohibited");
        SCOPE_EXIT
        {
            store_cookie();
        };

        return ::CoCreateInstance(clsid, pUnkOuter, dwClsContext, get_interface_guid(interface_wrapper<Interface>{}), reinterpret_cast<void**>(&p_));
    }

    HRESULT create_instance(const GUID& clsid, IUnknown* pUnkOuter = nullptr, DWORD dwClsContext = CLSCTX_ALL) noexcept
    {
        return CoCreateInstance(clsid, pUnkOuter, dwClsContext);
    }

    static com_ptr create(const GUID& clsid, IUnknown* pUnkOuter = nullptr, DWORD dwClsContext = CLSCTX_ALL)
    {
        com_ptr result;
        auto hr = result.CoCreateInstance(clsid, pUnkOuter, dwClsContext);
        if (SUCCEEDED(hr))
            return result;
        else
            throw_bad_hresult(hr);
    }

#if RTCSDK_HAS_CHECKED_REFS
    void add_weak(ref<Interface>* pweak)
    {
        weaks_.push_back(pweak);
    }

    void remove_weak(ref<Interface>* pweak)
    {
        std::erase(weaks_, pweak);
    }
#endif
};

constexpr const attach_t attach = {};


template<typename Interface>
class __declspec(empty_bases)ref
{
    Interface* p_{};
#if RTCSDK_HAS_CHECKED_REFS
    com_ptr<Interface>* parent_{};
#endif
    struct move_tag
    {
    };

    template<typename OtherInterface>
    ref(OtherInterface* p, std::true_type) noexcept : p_{ static_cast<Interface*>(p) }
    {
    }

    template<typename OtherInterface>
    ref(com_ptr<OtherInterface>&& o, std::true_type) noexcept : p_{ static_cast<Interface*>(o.p_) }
    {
#if RTCSDK_HAS_CHECKED_REFS
        parent_ = &o;
        // We allow construction from temporary com_ptr, but in DEBUG build we make sure the com_ptr lives long enough
        o.add_weak(this);
#endif
    }

public:
    explicit operator bool() const noexcept
    {
        return !!p_;
    }

    ref() = default;

    ref(std::nullptr_t) noexcept
    {
    }

    ref(const com_ptr<Interface>& o) noexcept : p_{ o.p_ }
    {
    }

    ref(com_ptr<Interface>&& o) noexcept : p_{ o.p_ }
    {
#if RTCSDK_HAS_CHECKED_REFS
        parent_ = &o;
        // We allow construction from temporary com_ptr, but in DEBUG build we make sure the com_ptr lives long enough
        o.add_weak(this);
#endif
    }

    // allow construction from derived interfaces
    template<typename OtherInterface>
    ref(const com_ptr<OtherInterface>& o) noexcept : ref(o.get(), std::is_base_of<Interface, OtherInterface>{})
    {
    }

    template<typename OtherInterface>
    ref(com_ptr<OtherInterface>&& o) noexcept : ref(move_tag{}, std::move(o), std::is_base_of<Interface, OtherInterface>{})
    {
    }

    template<typename OtherInterface>
    ref(const ref<OtherInterface>& o) noexcept : ref(o.get(), std::is_base_of<Interface, OtherInterface>{})
    {
    }

#if RTCSDK_HAS_CHECKED_REFS
    ~ref()
    {
        if (parent_) {
            parent_->remove_weak(this);
        }
    }

    ref(const ref& o) : p_{ o.p_ }, parent_{ o.parent_ }
    {
        if (parent_) {
            parent_->add_weak(this);
        }
    }

    ref(ref&& o) noexcept : p_{ o.p_ }, parent_{ o.parent_ }
    {
        if (parent_) {
            parent_->remove_weak(&o);
            parent_->add_weak(this);
        }
    }
#endif

    ref(Interface* p) noexcept : p_{ p }
    {
    }

    template<typename T>
    ref& operator =(const T&) = delete;

    Interface* operator ->() const noexcept
    {
        return p_;
    }

    // Comparison operators
    bool operator ==(const ref& o) const noexcept
    {
        return p_ == o.p_;
    }

    bool operator !=(const ref& o) const noexcept
    {
        return p_ != o.p_;
    }

    bool operator ==(Interface* pother) const noexcept
    {
        return p_ == pother;
    }

    bool operator !=(Interface* pother) const noexcept
    {
        return p_ != pother;
    }

    friend bool operator ==(Interface* p1, const ref& p2) noexcept
    {
        return p1 == p2.p_;
    }

    friend bool operator !=(Interface* p1, const ref& p2) noexcept
    {
        return p1 != p2.p_;
    }

    bool operator ==(const com_ptr<Interface>& o) noexcept
    {
        return p_ == o.p_;
    }

    bool operator !=(const com_ptr<Interface>& o) noexcept
    {
        return p_ != o.p_;
    }

    // Ordering operator
    bool operator<(const ref& o) const noexcept
    {
        return p_ < o.p_;
    }

    // Pointer accessors
    Interface* get() const noexcept
    {
        return p_;
    }

    // Conversion operations
    template<typename Interface>
    auto as() const noexcept
    {
        return com_ptr<Interface>{ p_ };
    }
};

template<typename Interface>
template<typename OtherInterface>
inline com_ptr<Interface>::com_ptr(ref<OtherInterface> o) noexcept : com_ptr{ o.get() }
{
}
}

using details::com_ptr;
using details::ref;

using details::attach;
using details::init_leak_detection;
}

namespace com {
// Possibly guard by macro
template<typename T>
using ptr = rtcsdk::com_ptr<T>;

template<typename T>
using ref = rtcsdk::ref<T>;
}
