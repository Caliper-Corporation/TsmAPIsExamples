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

#include "pch.h" // Pre-compiled header
#include "vehicle.h"

namespace vmplugin {

[[maybe_unused]] auto MyVehicle::logger()
{
  return MyVehicleMonitor::instance()->logger();
}

MyVehicle::~MyVehicle()
{
  // Add destructor logic to clean up
}

void MyVehicle::Departure(double time)
{
  // Fill in user logic
  if (id_ == 366) {
    auto tid = std::hash<std::thread::id>{}(std::this_thread::get_id());
    logger()->info("OnDeparture: time={:.1f},tid={}", time, tid);
  }
}

void MyVehicle::Arrival(double time)
{
  // Fill in user logic

  if (id_ == 366) {
   auto tid = std::hash<std::thread::id>{}(std::this_thread::get_id());
    logger()->info("OnArrival: time={:.1f},tid={}", time, tid);
  }
}

void MyVehicle::Update(double time, const SVehicleBasicState &state)
{
  // Fill in user logic.
  // We can save the data to private data members of the subject vehicle instance.
  // Note - MyVehicle::Update is fired with every simulation step.
  // Here we just print out the vehicle's current state.
  if (id_ == 366) {
    // Log the current acceleration
    auto tid = std::hash<std::thread::id>{}(std::this_thread::get_id());
    logger()->info("OnUpdate: time={:.1f},tid={},veh={},acc={:.2f},grade={},speed={:.3f},idSegment={}",
                   time, tid, id_, state.fAcc, state.fGrade, state.fSpeed, state.idSegment);
  }
}

float MyVehicle::CalculateCarFollowingAccRate(double time, const SCarFollowingData &data, float acc)
{
  return flt_miss; // ignored
}

float MyVehicle::Acceleration(double time, float acc)
{
  if (id_ == 366) {
    accel_ = accel_ + 0.1f; // Each time we increment the acceleration by 0.1 m/s^2
    auto tid = std::hash<std::thread::id>{}(std::this_thread::get_id());
    logger()->info("OnAcceleration: time={:.1f},tid={}, veh={},tsm_suggested_acc={:.2f}, new_acc={:.2f}",
                   time, tid, id_, acc, accel_);
    return accel_;
  } else {
    return flt_miss;
  }
}

short MyVehicle::LaneChange(double time, short dir, bool *mandatory)
{
  return short_miss;// ignored
}

float MyVehicle::TransitStop(double time, const STransitStopInfo &info, float dwell)
{
  return flt_miss; // ignored
}

void MyVehicle::Position(double time, const SVehiclePosition &pos)
{
  // Fill in user logic
  if (id_ == 366) {
    // log simulation step time, vehicle id, longitude, and latitude
    auto tid = std::hash<std::thread::id>{}(std::this_thread::get_id());
    logger()->info("OnPosition: time={:.1f},tid={},veh={},pos.x={},pos.y={}", time, tid, id_, pos.x, pos.y);
  }
}

void MyVehicle::Coordinate(double time, const SVehicleCoordinate& coord)
{
    // Fill in user logic
    if (id_ == 366) {
        // log simulation step time, vehicle id, longitude, and latitude
        auto tid = std::hash<std::thread::id>{}(std::this_thread::get_id());
        logger()->info("OnPosition: time={:.1f},tid={},veh={},lon={},lat={}", time, tid, id_, coord.lon, coord.lat);
    }
}

void MyVehicle::Parked(double time)
{
  // Fill in user logic
}

void MyVehicle::Stalled(double time, bool stalled)
{
  // Fill in user logic
}

bool MyVehicle::OnFail(BSTR msg)
{
  return false;
}

}// namespace vmplugin