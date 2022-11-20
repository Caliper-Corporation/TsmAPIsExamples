#pragma once
#include <rtcsdk/interfaces.h>

namespace rtcsdk {

inline HRESULT DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) noexcept
{
    try
    {
        auto factory = details::Factory::create_instance(rclsid).to_ptr<IUnknown>();
        return factory->QueryInterface(riid, ppv);
    }
    catch (const std::bad_alloc&)
    {
        return E_OUTOFMEMORY;
    }
    catch (const bad_hresult& err)
    {
        return err.hr();
    }
    catch (...)
    {
        return E_FAIL;
    }
}

inline HRESULT DllCanUnloadNow() noexcept
{
    return details::ModuleCount::lock_count.load(std::memory_order_relaxed) ? S_FALSE : S_OK;
}

namespace details {

class __declspec(novtable) Factory : public object<Factory, IClassFactory>
{
public:
    Factory(const CLSID& clsid) noexcept : clsid_{ clsid }
    {
    }

    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject) noexcept override
    {
        return rtcsdk::create_object(clsid_, riid, ppvObject, pUnkOuter);
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL fLock) noexcept override
    {
        if (fLock)
            ModuleCount::lock_count.fetch_add(1, std::memory_order_relaxed);
        else
            ModuleCount::lock_count.fetch_sub(1, std::memory_order_relaxed);

        return S_OK;
    }

private:
    CLSID clsid_;
};

} // namespace details

} // end namespace rtcsdk
