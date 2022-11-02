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

/*
    Changes Log:
        1. 2022-11-01 Initial C++ implementation by Wuping Xin.
*/

#ifndef VM_PLUGIN
#define VM_PLUGIN

#include "pch.h"

namespace vm_plugin {

template<size_t N>
struct VehicleMonitorName
{
    constexpr VehicleMonitorName(const wchar_t(&str)[N])
    {
        std::copy_n(str, N, value);
    }

    wchar_t value[N];
};

using VehicleMonitorOptions = unsigned long;

template<typename T>
concept UserVehicleType = std::derived_from<T, IUserVehicle> && std::is_constructible_v<T>;

template<UserVehicleType T, VehicleMonitorOptions op, VehicleMonitorName name>
    requires (((op << 2) & 0x00000001) == 0) && (((op << 3) & 0x00000001) == 0)
class VehicleMonitor : public CUserVehicleMonitor
{
public:
    ~VehicleMonitor()
    {
        ::SysFreeString(name_);
    }

    VehicleMonitor(VehicleMonitor&) = delete;
    VehicleMonitor(VehicleMonitor&&) = delete;
    VehicleMonitor& operator=(VehicleMonitor&) = delete;
    VehicleMonitor& operator=(VehicleMonitor&&) = delete;

    auto GetName() const -> const BSTR override { return name_; };

    IUserVehicle* AttachVehicle(long id, const SVehicleProperty& prop, unsigned long* pFlags)
    {
        *pFlags = op;
        return new T();
    }

    static bool Load()
    {
        vm_ = std::unique_ptr<VehicleMonitor<T, op, name>>{ new VehicleMonitor<T, op, name>() };
        return VehicleMonitor::RegisterVehicleMonitor(vm_.get());
    }

    static bool Unload()
    {
        auto result = VehicleMonitor::UnregisterVehicleMonitor(vm_.get());
        vm_ = nullptr;
        return result;
    }

    /// <summary> Fires when a simulation project is being opened. </summary>
    ///
    /// <param name="fname"> Filename of the project file. </param>
    void OpenProject(const BSTR fname) override
    {
    }

    /// <summary> Fires before starting the simulation. </summary>
    void StartSimulation(short iRun, TsmRunType iRunType, VARIANT_BOOL bPreload) override
    {
    }

    /// <summary> Fires after simulation has been successful started. </summary>
    void SimulationStarted() override
    {
    }

    /// <summary> Fires after simulation has been stopped. </summary>
    void SimulationStopped(TsmState iState) override
    {
    }

    /// <summary> Fires at the end of the simulation. </summary>
    void EndSimulation(TsmState iState) override
    {
    }

    /// <summary> Fires when closing the project. </summary>
    void CloseProject() override
    {
    }

    /// <summary> Fires on application exit. </summary>
    void ExitApplication() override
    {
    }

protected:
    VehicleMonitor()
    {
        name_ = ::SysAllocString(name.value);
    }

private:
    BSTR name_{ nullptr };
    inline static std::unique_ptr<VehicleMonitor<T, op, name>> vm_{ nullptr };
};

class MyVehicle : public IUserVehicle
{
public:

    /// <summary> Fires when a vehicle entering the network. </summary>
    ///
    /// <param name="dTime"> Current time of simulation clock. </param>
    void Departure(double dTime)
    {
        // Fill in user logic
    }

    /// <summary>
    ///     Fires when a vehicle arrives at its destination or drop location.
    /// </summary>
    ///
    /// <remarks>
    ///     1) The subject IUserVehicle instance would have been detached from
    ///     associated TransModeler ITsmVehicle instance at this point.
    ///     2) The subject IUserVehicle instance can be released here, unless it
    ///     is still being used, e.g., representing a parked vehicle in 3D viewer.
    ///     3) After Arrival is called, the subject IUserVehicle instance will no
    ///     longer receive any further callbacks.
    /// </remarks>
    ///
    /// <param name="dTime"> Current time of simulation clock. </param>
    void Arrival(double dTime) override
    {
        // Fill in user logic
    }

    /// <summary> Fires whenever vehicle state is changed. </summary>
    ///
    /// <param name="dTime"> Current time of simulation clock. </param>
    /// <param name="vs">    The vehicle state data. </param>
    void Update(double dTime, const SVehicleBasicState& vs) override
    {
        // Fill in user logic
    }

    /// <summary>
    ///     Fires to receive user-calculated car-following acceleration rate
    ///     subject to TransModeler's internal constraints.
    /// </summary>
    ///
    /// <remarks>
    ///     VM_CF_SUBJECT bit must have been set via
    ///     CUserVehicleMonitor::AttachVehicle call in order to receive this
    ///     callback. Also to receive this call back, ITsmVehicle::AccOverride
    ///     property of the vehicle cannot be set false by another plugin.
    /// </remarks>
    ///
    /// <param name="dTime">    Current time of simulation clock. </param>
    /// <param name="pCFData">  Detailed car-following data. </param>
    /// <param name="fAccRate"> The acceleration rate calculated by
    ///                         TransModeler's default model. </param>
    ///
    /// <returns>
    ///     Acceleration rate calculated by the user logic. TransModeler would
    ///     still consider other constraints on vehicle's
    ///     acceleration/deceleration such as response to signals and signs. The
    ///     return value is used by TransModeler only if it is more restrictive
    ///     than TransModeler's calculated value. Return flt_miss if this
    ///     function is not of interest.
    /// </returns>
    float CalculateCarFollowingAccRate(double dTime, const SCarFollowingData& pCFData, float fAccRate) override
    {
        return (flt_miss);   // ignored
    }

    /// <summary>
    ///     Fires to receive user-calculated acceleration rate that will be
    ///     directly applied to updating vehicle speed.
    /// </summary>
    ///
    /// <remarks>
    ///     VM_CF_SUBJECT bit must have been set via
    ///     CUserVehicleMonitor::AttachVehicle call in order to receive this
    ///     callback. Also to receive this call back, ITsmVehicle::AccOverride
    ///     property of the vehicle cannot be set false by another plugin.
    /// </remarks>
    ///
    /// <param name="dTime">    Current time of simulation clock. </param>
    /// <param name="fAccRate"> The acceleration rate as calculated by
    ///                         TransModeler that has already considered
    ///                         constraints such as the traffic signal, merging,
    ///                         etc. </param>
    ///
    /// <returns>
    ///     The final acceleration rate to be applied to the subject vehicle. If
    ///     an inappropriate value is returned, the vehicle may stall, violate
    ///     traffic signals, or run through other vehicles. Return flt_miss if
    ///     this function is not of interest.
    /// </returns>
    float Acceleration(double dTime, float fAccRate) override
    {
        return (flt_miss);   // ignored
    }

    /// <summary> Fires when lane change decision is required. </summary>
    ///
    /// <remarks>
    ///     VM_LANE_CHANGE bit must have been set via
    ///     CUserVehicleMonitor::AttachVehicle call in order to receive this
    ///     callback. Also to receive this call back,
    ///     ITsmVehicle::LaneChangeOverride property of the vehicle cannot be set
    ///     to false by another plugin.
    /// </remarks>
    ///
    /// <param name="dTime">      Current time of simulation clock. </param>
    /// <param name="iDirection"> The lane changing decision made by TransModeler. -
    ///                           1=left, 0=none, and 1=right. </param>
    /// <param name="pMandatory"> [in,out] Whether lane-change/stay mandatory. </param>
    ///
    /// <returns>
    ///     A value indicating whether a lane change to the left (-1) or right
    ///     (1) is desired by the vehicle, or whether it should stay in the
    ///     current lane (0). Return short_miss if this function is not of
    ///     interest.
    /// </returns>
    short LaneChange(double dTime, short iDirection, bool* pMandatory) override
    {
        return (short_miss); // ignored
    }

    /// <summary> Fires when a transit vehicle comes to a stop. </summary>
    ///
    /// <param name="dTime">      Current time of simulation clock. </param>
    /// <param name="si">         Transit vehicle stop information. </param>
    /// <param name="fDwellTime"> Dwell time calculated by TransModeler. </param>
    ///
    /// <returns>
    ///     User calculated dwell time. Can simply return the same value as
    ///     fDwellTime, or flt_miss if this function is not of interest.
    /// </returns>
    float TransitStop(double dTime, const STransitStopInfo& si, float fDwellTime) override
    {
        return (flt_miss);   // ignored
    }

    /// <summary> Fires when a vehicle is moved. </summary>
    ///
    /// <param name="dTime"> Current time of simulation clock. </param>
    /// <param name="vp">    Vehicle position data. </param>
    void Position(double dTime, const SVehiclePosition& vp) override
    {
        // Fill in user logic
    }

    /// <summary>
    ///     Fires when the subject vehicle has parked at a parking space. It
    ///     would be called prior to calling Arrival(...).
    /// </summary>
    ///
    /// <param name="dTime"> Current time of simulation clock. </param>
    void Parked(double dTime) override
    {
    }

    /// <summary>
    ///     Fires when a vehicle is stalled or a stalled vehicle starts to move
    ///     again.
    /// </summary>
    ///
    /// <param name="dTime">    Current time of simulation clock. </param>
    /// <param name="bStalled"> True if stalled, false if moving again. </param>
    void Stalled(double dTime, bool bStalled) override
    {
    }

    /// <summary>
    ///     Fires when an error has occurred for this IUserVehicle instance.
    /// </summary>
    ///
    /// <param name="msg"> The error message. </param>
    ///
    /// <returns>
    ///     True to ignore the warning and continue, or false to skip receiving
    ///     further callback notification.
    /// </returns>
    bool OnFail(const BSTR msg) override
    {
        return (false);
    }
};

}
#endif


