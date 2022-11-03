/*
BSD 3 - Clause License

Copyright(c) 2022, Caliper Corporation
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and /or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "pch.h"
#include "vm_plugin.hpp"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    using namespace vm_plugin;

    /*
    Type alias for template class VehicleMonitor<T, VehicleMonitorOptions, name>
        T - type for user vehicle class;
        VehicleMonitorOptions - callback options;
        name - name of the vehicle monitor.
  
    Change type and non-type template parameters "T", "VehicleOptions", and "name"
    for a different configuration.

    Note any change to the template parameters would end up a different
    instantiated class, for example, the following are all different classes, and
    each holds different singleton respectively.

    VehicleMonitor<
        MyVehicle,
        VM_UPDATE | VM_POSITION,
        L"Cool Vehicle Monitor"
    >

    VehicleMonitor<
        MyVehicle,
        VM_UPDATE,
        L"Cool Vehicle Monitor"
    >;

    VehicleMonitor<
        MyVehicle,
        VM_POSITION,
        L"Cool Vehicle Monitor"
    >;

    VehicleMonitor<
        MyVehicle,
        VM_UPDATE | VM_POSITION,
        L"Not Cool Vehicle Monitor"
    >;
     */
    using VehicleMonitor = VehicleMonitor<
        MyVehicle,
        VM_UPDATE | VM_POSITION,
        L"Cool Vehicle Monitor"
    >;

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // Internally create a monitor singleton and register it with TransModeler.
        VehicleMonitor::Load();
        break;

    case DLL_PROCESS_DETACH:
        // Unregister the monitor with TransModeler, and release it from memory.
        VehicleMonitor::Unload();
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;
    }

    return TRUE;
}

