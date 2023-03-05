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

// pch.h: This is a pre-compiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#pragma warning(disable : 4068)
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
#ifndef PCH_H
#define PCH_H

#import <TsmApi.tlb>

// add headers that you want to pre-compile here
#include <algorithm>
#include <functional>
#include <memory>

#include <atlbase.h>
#include <Tsm/TsmApi/VehicleMonitor.h>
#include <tsmapi.tlh>

#define SPDLOG_WCHAR_FILENAMES
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include <guiddef.h>

namespace vmplugin {

struct ThePlugin
{
  // CLSID for ITsmApplication interface.
  static constexpr CLSID tsmapp_clsid{0x1E9F5CCD, 0x6AA2, 0x45F2, {0x83, 0x47, 0xF0, 0x33, 0x94, 0x3A, 0x04, 0x9C}};
  /*
   * A faster way of creating TsmApplication interface by avoiding the use of CLSIDFromProgID.
   * The caller is responsible for releasing the interface.
   */
  static TsmApi::ITsmApplicationPtr CreateTsmAppInstance()
  {
    CComPtr<TsmApi::ITsmApplication> app;
    HRESULT hr = app.CoCreateInstance(tsmapp_clsid);
    return SUCCEEDED(hr) ? app.Detach() : nullptr;
  }
};

}
#endif //PCH_H
#pragma clang diagnostic pop
#pragma warning(default : 4068)