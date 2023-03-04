#pragma once

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

#ifndef VMPLUGIN_MONITOR
#define VMPLUGIN_MONITOR

#include "pch.h"// Pre-compiled header

namespace vmplugin {

/**
 A utility class for specifying compile-time vehicle monitor name as a non-
 type template parameter.

 @tparam    N   Number of wide characters.
 */
template<size_t N>
struct VehicleMonitorName//
{
  /*!
   * Non-explicit conversion allowed.
   * @param str Unicode name of the monitor.
   */
  constexpr VehicleMonitorName(const wchar_t (&str)[N])
  {
    std::copy_n(str, N, value);
  }

  constexpr VehicleMonitorName() = default;

  wchar_t value[N] = {};
};

using VehicleMonitorOptions = unsigned long;

template<VehicleMonitorOptions Opts>
concept ValidVehicleMonitorOptions = (((Opts << 2) & 0x0000'0001) == 0) && (((Opts << 3) & 0x0000'0001) == 0);

template<typename T>
concept UserVehicleType = std::derived_from<T, IUserVehicle> && std::is_constructible_v<T, long, SVehicleProperty>;

template<UserVehicleType T, VehicleMonitorOptions Opts, VehicleMonitorName Name> /* */
requires ValidVehicleMonitorOptions<Opts>
class VehicleMonitor : public CUserVehicleMonitor
{
public:
  using VehicleMonitorType = VehicleMonitor<T, Opts, Name>;

  ~VehicleMonitor() override
  {
    ::SysFreeString(name_);
    tsmapp_ = nullptr;
  }

  VehicleMonitor(VehicleMonitor &) = delete;
  VehicleMonitor(VehicleMonitor &&) = delete;
  VehicleMonitor &operator=(VehicleMonitor &) = delete;
  VehicleMonitor &operator=(VehicleMonitor &&) = delete;

  [[nodiscard]] const BSTR GetName() const override
  {
    return name_;
  };

  /**
     Attach the user vehicle to an associated TransModeler's vehicle entity.

     @param             id      Vehicle ID, assigned by TransModeler.
     @param             prop    Property of TransModeler vehicle entity.
     @param [in,out]    opts    Monitor options for the vehicle.

     @returns   Null to advise TransModeler not to attach, else a pointer to an
                IUserVehicle.
     */
  IUserVehicle *AttachVehicle(long id, const SVehicleProperty &prop, VehicleMonitorOptions *opts) override
  {
    *opts = Opts;
    return new T(id, prop);
  }

  /**
     Load the singleton monitor to TransModeler.

     @returns   True if it succeeds, false if it fails.
     */
  static bool Load() noexcept
  {
    return vm_ || []() {
      vm_.reset(new VehicleMonitorType());
      return VehicleMonitor::RegisterVehicleMonitor(vm_.get());
    }();
  }

  /**
     The singleton vehicle monitor.

     @returns   A pointer to VehicleMonitor associated with a user-defined vehicle class.
     */
  [[maybe_unused]] static const auto &instance() noexcept
  {
    return vm_;
  }

  /**
     Unloads the singleton monitor from TransModeler.

     @returns   True if it succeeds, false if it fails or there is no vm to unload.
     */
  static bool Unload() noexcept
  {
    return vm_ && []() -> bool {
      auto result = VehicleMonitor::UnregisterVehicleMonitor(vm_.get());
      vm_ = nullptr;
      return result;
    }();
  }

  /**
     Fires when a simulation project is being opened.

     @param     name    Project file name.
     */
  void OpenProject(const BSTR name) override
  {
    [&]() {
      using namespace std;

      if (!tsmapp_)
        return;

      auto project_folder = tsmapp_->GetProjectFolder();
      wstring log_folder = wstring(project_folder) + wstring(&Name.value[0]);
      auto rotating_sink = make_shared<spdlog::sinks::rotating_file_sink_mt>(log_folder + L"/vm-log.txt", 1024*1024, 5);
      logger_ = make_shared<spdlog::logger>("vm_logger", rotating_sink);
    }();
  }

  /**
     Fires before starting the simulation.

     @param     run         Zero-based index of the run.
     @param     run_type    Type of the run.
     @param     preload     Whether this is a preload run.
     */
  void StartSimulation(short run, TsmApi::TsmRunType run_type, VARIANT_BOOL preload)
  {
  }

  /** Fires after simulation has been successful started. */
  void SimulationStarted() override
  {
  }

  /**
     Fires with each simulation step

    @param time Current time of simulation clock.
   */
  void Advance(double time) override
  {
  }

  /**
     Fires after simulation has been stopped.

     @param     state   TransModeler state.
     */
  void SimulationStopped(TsmApi::TsmState state)
  {
  }

  /**
     Fires at the end of the simulation.

     @param     state   TransModeler state.
     */
  void EndSimulation(TsmApi::TsmState state)
  {
  }

  /** Fires when closing the project. */
  void CloseProject() override
  {
    logger_ = nullptr;
  }

  /** Fires on application exit. */
  void ExitApplication() override
  {
  }

  /**
     Gets TransModeller application instance.

     @returns   TsmApi::ITsmApplicationPtr.
     */
  [[maybe_unused]] [[nodiscard]] TsmApi::ITsmApplicationPtr tsmapp() const noexcept
  {
    return tsmapp_;
  };

  [[maybe_unused]] double sim_step() noexcept
  {
    static double step = tsmapp_ ? tsmapp_->StepSize : 0;
    return step;
  }

  const std::shared_ptr<spdlog::logger> logger() const
  {
    return logger_;
  }

protected:
  /** Default constructor with "protected" access level. */
  VehicleMonitor() noexcept: name_{::SysAllocString(Name.value)}
  {
    tsmapp_ = []() -> TsmApi::ITsmApplication * {
      CComPtr<TsmApi::ITsmApplication> app;
      HRESULT hr = app.CoCreateInstance(L"TsmApi.TsmApplication");
      return SUCCEEDED(hr) ? app.Detach() : nullptr;
    }();
  }

private:
  BSTR name_{nullptr};
  inline static std::unique_ptr<VehicleMonitorType> vm_{nullptr};
  TsmApi::ITsmApplicationPtr tsmapp_{nullptr};
  std::shared_ptr<spdlog::logger> logger_{nullptr};
};

}// namespace vmplugin

#endif
