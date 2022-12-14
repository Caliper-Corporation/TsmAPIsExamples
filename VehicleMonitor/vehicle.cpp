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

/*
 Add implementation to individual callbacks. Use the following code to access
 network elements.

 MyVehicleMonitor::instance()->tsmapp()->Network

*/

auto logger = spdlog::basic_logger_mt("vm_logger", "D:/logs/vm-log.txt");

void MyVehicle::Departure(double time)
{
  // Fill in user logic
}

void MyVehicle::Arrival(double time)
{
  // Fill in user logic
}

void MyVehicle::Update(double time, const SVehicleBasicState &state)
{
  // Fill in user logic
}

float MyVehicle::CalculateCarFollowingAccRate(double time, const SCarFollowingData &data, float acc)
{
  return (flt_miss);// ignored
}

float MyVehicle::Acceleration(double time, float acc)
{
  return (flt_miss);// ignored
}

short MyVehicle::LaneChange(double time, short dir, bool *mandatory)
{
  return (short_miss);// ignored
}

float MyVehicle::TransitStop(double time, const STransitStopInfo &info, float dwell)
{
  return (flt_miss);// ignored
}

void MyVehicle::Position(double time, const SVehiclePosition &pos)
{
  // Fill in user logic 
  logger->info("At time {} Position() called for vehicle {}", time, id_);
}

void MyVehicle::Parked(double time)
{
  // Fill in user logic
}

void MyVehicle::Stalled(double time, bool stalled)
{
  // Fill in user logic
}

bool MyVehicle::OnFail(const BSTR msg)
{
  return false;
}

}// namespace vmplugin