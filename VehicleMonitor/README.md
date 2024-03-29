# Vehicle Monitor Demo

## Introduction

Vehicle Monitor API is a set of high-performance C++ API that allows developing TransModeller plugin with a direct connection to TransModeler's internal vehicle movement module.

Vehicle Monitor API can be applied to many use cases that require customized user logic:
- User-defined driver-behavior models, such as Car-Following or Lane Change Models that take into account the latest driver behavioral or human factor research outcomes
- Advanced Connected and Autonomous Vehicle (CAV) algorithms, such as Adaptive Platooning, Connected Eco-Driving, Automatic Emergency Braking, Forward Collision Warning etc.
- New computational approach for emission modelling
- P2P communication and latency modelling in a Vehicular Ad hoc Network (VANET)
- Trajectory big-data approaches such as using trajectories for Safety-Surrogate Measures, Location-based Data Service modelling
- Connected Vehicle Cyber-security and Location Privacy Protection schemes
- Driver Simulator or 3D visualizations

The application boundary is only limited by your imagination and creativity.

## Framework Design Pattern

The framework is designed with performance-critical applications in mind, while paying attention to intuition and ease of use. The following figure illustrates the design.

<p align="left">
  <img width="60%" src="https://github.com/Caliper-Corporation/TsmAPIsExamples/blob/main/VehicleMonitor/img/vm_framework_design.svg" alt="">
</p>

## Advantage

Vehicle Monitor API has overlapping use cases with TransModeler COM-based API, e.g. `IVehicleEvent` and `ITsmVehicle` COM interface.  While COM interface is more flexible in accommodating a wide range of programming languages,  Vehicle Monitor native C++ API has the following benefits:
- Provide the best performance needed for performance-critical applications;
- Feature an efficient call-back mechanism covering a comprehensive set of vehicle-related events; 
- Multi-threaded callbacks taking full advantage of TransModeller's powerful parallel computation.

## Usage

* First make sure [`VehicleMonitor.props`](https://github.com/Caliper-Corporation/TsmAPIsDemo/blob/main/VehicleMonitor/VehicleMonitor.props) references to the correct TransModeller installation folder. Update the XML accordingly:
  ```
    <PropertyGroup Label="UserMacros">
      <TSM_FOLDER>C:\Program Files\TransModeler 6.1</TSM_FOLDER>
    </PropertyGroup>
  ```

* At the DLL entry point, use the following code to Load/Unload Vehicle Monitor with TransModeler host.
  ```
  BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
  {
      switch (ul_reason_for_call)
      {
      case DLL_PROCESS_ATTACH:
          vmplugin::MyVehicleMonitor::Load();
          break;

      case DLL_PROCESS_DETACH:
          vmplugin::MyVehicleMonitor::Unload();
          break;
      }

      return TRUE;
  }
  ```

* In the header file where the user vehicle class `MyVehicle` is defined, i.e., [`vehicle.h`](https://github.com/Caliper-Corporation/TsmAPIsDemo/blob/main/VehicleMonitor/vehicle.h), type-alias the instantiated template `VehicleMonitor` class:
  ```
  /** Specify the Vehicle Monitor associated with the user-defined vehicle type. */
  using MyVehicleMonitor = vmplugin::VehicleMonitor<
      MyVehicle,                  // User vehicle type
      VM_UPDATE | VM_POSITION,    // Callback options
      L"MyVehicleMonitor"         // Vehicle monitor name
  >;
  ```

  ```VehicleMonitor<MyVehicle, VM_UPDATE | VM_POSITION, L"Cool Vehicle Monitor">``` instantiates a VehicleMonitor template class - `MyVehicle` class is the user-defined vehicle class to be monitored. ```VM_UPDATE|VM_POSITION``` means *Vehicle State Update* and *Position Change* events will be fired, and the user logic can process relevant information in the respective event handlers. The last non-type template parameter allows specifying a name for the vehicle monitor, in this case, "Cool Vehicle Monitor".

    Vehicle Monitor provides the following events:
    - Arrival
    - Departure
    - PositionChange
    - StateUpdate
    - Stalled
    - Parked
    - TransitStop
    - LaneChange
    - Acceleration
    - CarFollowingAccelerateRateCalculation
  
* **User-logic** code should be implemented by overriding the virtual methods of `MyVehicle` class. Take a look at [`vehicle.h`](https://github.com/Caliper-Corporation/TsmAPIsDemo/blob/main/VehicleMonitor/vehicle.h) and those source code comments for a better idea.  For example, if you are interested in obtaining detailed vehicle position information, you can implement the following virtual method of `MyVehicle` class:
  ```
  /**
   Fires when a vehicle is moved.

   @param     time    Current time of the simulation clock.
   @param     pos     Vehicle position data.
  */

  void Position(double time, const SVehiclePosition& pos) override
  {
      // Fill in user logic
  }
  ```
* The generated `VehicleMonitor.dll` can be **loaded** using Caliper Script (GISDK) *Immediate Execution Dialog*, using code like below:
  ```
   shared mydll
   output_path = "D:\\Caliper Repos\\VehicleMonitor\\.out\\Debug"
   mydll = LoadLibrary(output_path + "\\VehicleMonitor.dll")
  ```
  
  `output_path` is where the DLL is output to. Replace it with the actual path.
  Use the following line to explicitly **unload** the DLL: 
  ```
  mydll = null
  ```
  
### Thread-Safety
Accessing instance-specific data from inside the callbacks are thread-safe. However, accessing global data inside the callbacks are not thread-safe. 

### Logging
A log file will be created in {ProjectFolder}\MyVehicleMonitor\vm_log.txt.  Note that "MyVehicleMonitor" is the name of the vehicle monitor specified by the user when instantiating the vehicle monitor template.

To reference the logger inside user define vehicle, use MyVehicleMonitor::instance->logger()

## How to Debug

* Set breakpoints  
* Launch TransModeler
* From Visual Studio IDE, make sure it is `Debug` configuration, then click `Local Windows Debugger`
* Use *GISDK Immediate Execution Dialog*, execute the following script to load the DLL. Remember to replace `output_path` with actual path.
  ```
  shared mydll
  mydll = LoadLibrary(output_path + "\\VehicleMonitor.dll")
  ```
* Have fun
