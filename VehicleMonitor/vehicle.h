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

#ifndef VMPLUGIN_VEHICLE
#define VMPLUGIN_VEHICLE

// Pre-compiled header
#include "pch.h"
//
#include "monitor.h"

namespace vmplugin {

class MyVehicle : public IUserVehicle
{
public:
  MyVehicle(const long id, const SVehicleProperty &prop) noexcept : id_{id}, prop_{prop} {}

  ~MyVehicle();

  /**
     Fires when a vehicle entering the network.

     @param     time    Current time of the simulation clock.
     */
  void Departure(double time) override;

  /**
     Fires when a vehicle arrives at its destination or drop location.

     @param     time    Current time of the simulation clock.
     */
  void Arrival(double time) override;

  /**
     Fires at each simulation step.

     @param     time    Current time of the simulation clock.
     @param     state   Vehicle state data.
     */
  void Update(double time, const SVehicleBasicState &state) override;

  /**
     Fires to receive user-calculated car-following acceleration rate subject
     to TransModeler's internal constraints.

     @param     time    Current time of the simulation clock.
     @param     data    Car-following data.
     @param     acc     Acceleration rate calculated by TransModeler's default
                        model.

     @returns   Acceleration rate calculated by the user logic. Return flt_miss
                if this function is not of interest.

     @remarks   TransModeler would still consider other constraints on
                vehicle's acceleration/deceleration such as response to signals
                and signs. The return value will be used by TransModeler only if
                it is more restrictive than TransModeler's calculated value.

                CUserVehicleMonitor::AttachVehicle(...) must have set the
                VM_CF_SUBJECT bit in order to receive this callback. Also to
                receive this call back, ITsmVehicle::AccOverride property of the
                vehicle cannot be set false by another plugin.
     */
  float CalculateCarFollowingAccRate(double time, const SCarFollowingData &data, float acc) override;

  /**
     Fires to receive user-calculated acceleration rate that will be directly
     applied to updating the vehicle's speed.

     @param     time    Current time of the simulation clock.
     @param     acc     Acceleration rate calculated by TransModeler that has
                        already considered constraints such as the traffic signal,
                        merging, etc.

     @returns   The final acceleration rate to be applied to the subject vehicle.
                Return flt_miss if this function is not of interest.

     @remarks   If an inappropriate value is returned, the vehicle may stall,
                violate traffic signals, or run through other vehicles.
     */
  float Acceleration(double time, float acc) override;

  /**
     Fires when lane change decision is required.

     @param             time        Current time of the simulation clock.
     @param             dir         Lane changing decision made by TransModeler,
                                    left(-1), stay(0), and right(1).
     @param [in,out]    mandatory   Whether lane-change decision is mandatory.

     @returns   A value indicating moving to the left (-1) lane, right (1) lane,
                or stay (0) current. Return short_miss if this function is not of
                interest.

     @remarks   CUserVehicleMonitor::AttachVehicle(...) must have set the
                VM_LANE_CHANGE bit in order to receive this callback. Also to
                receive this call back, ITsmVehicle::LaneChangeOverride property
                of the vehicle cannot be set false by another plugin.
     */
  short LaneChange(double time, short dir, bool *mandatory) override;

  /**
     Fires when a transit vehicle comes to a stop.

     @param     time    Current time of the simulation clock.
     @param     info    Transit vehicle stop information.
     @param     dwell   Dwell time calculated by TransModeler.

     @returns   Dwell time calculated by user logic. Return flt_miss if this
                function is not of interest.
     */
  float TransitStop(double time, const STransitStopInfo &info, float dwell) override;

  /**
     Fires when a vehicle is moved.

     @param     time    Current time of the simulation clock.
     @param     pos     Vehicle position data.
     */
  void Position(double time, const SVehiclePosition &pos) override;

  /**
     Fires when the subject vehicle has parked at a parking space.
     Arrival(...) will be fired after this event.

     @param     time    Current time of the simulation clock.
     */
  void Parked(double time) override;

  /**
     Fires when a vehicle is stalled, or a stalled vehicle starts to move
     again.

     @param     time    Current time of the simulation clock.
     @param     stalled True if stalled, false moving again.
     */
  void Stalled(double time, bool stalled) override;

  /**
     Fires when an error has occurred for this IUserVehicle instance.

     @param     msg The error message.

     @returns   True to ignore the message and continue, false to skip receiving
                further callback notification.
     */
  bool OnFail(BSTR msg) override;

  [[maybe_unused]] [[nodiscard]] auto id() const -> long
  {
    return id_;
  }

private:
  /** Returns a logger for logging. */
  [[nodiscard]] static auto logger();

  /** Vehicle property */
  [[maybe_unused]] const SVehicleProperty prop_{};

  /** ID of the vehicle */
  const long id_{0};

  /** User calculated acceleration */
  float accel_{0};
};

/** Specify the Vehicle Monitor associated with the user-defined vehicle type. */
using MyVehicleMonitor = vmplugin::VehicleMonitor<
    MyVehicle,                                   // User vehicle type
    VM_UPDATE | VM_POSITION | VM_CF,             // Callback options
    L"MyVehicleMonitor"                          // Vehicle monitor name
    >;

}// namespace vmplugin

#endif
