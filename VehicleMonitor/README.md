# Vehicle Monitor Demo

## Introduction

Vehicle Monitor API is a set of high-performance C++ API that allows developing TransModeller plugin with a direct connection to TransModeler's internal vehicle movement module.

Vehicle Monitor API can be used to integrate user-defined driver-behavior models (CF or LC), Connected and Autonomous Vehicle (CAV) algorithms,  and Emission Models. It can also be used to model VANET Communications, generate trajectories for Safety-Surrgate Measures, emulate Location-based Data Service, and even to evaluate Connected Vehicle Cybersecurity and Location Privacy Protection schemes. Only imagination and creativity are the boundary of Vehicle Monitor API and other TransModeler API applications.

## Advantage

Vehicle Monitor API shares certain overlapping use cases with TransModeler COM-based API, i.e., ```IVehicleEvent``` and ```ITsmVehicle``` COM interface.  While COM interface is more flexible in accomodating different programming languages,  Vehicle Monitor API provides the best performance endowed by using native C++, and by an efficient call-back mechnism which covers more vehicle-related events than the COM interface. Importantly, Vehicle Monitor API takes full advantage of TransModeller's powerful multi-threaded parallel compuation.

## Usage

* First make sure [```VehicleMonitor.props```](https://github.com/Caliper-Corporation/TsmAPIsDemo/blob/main/VehicleMonitor/VehicleMonitor.props) references to the correct TransModeller installation folder. Update the XML accordingly:
  ```
    <PropertyGroup Label="UserMacros">
      <TSM_FOLDER>C:\Program Files\TransModeler 6.1</TSM_FOLDER>
    </PropertyGroup>
  ```

* **User-logic** code should be implemented through the overridden virtual methods of ```MyVehicle``` class.  Take a look at [```vm_plugin.hpp```](https://github.com/Caliper-Corporation/TsmAPIsDemo/blob/main/VehicleMonitor/vm_plugin.hpp) and those source code comments for a better idea.

* The generated ```VehicleMonitor.dll``` can be **loaded** using Caliper Script (GISDK) *Immediate Execution Dialog*, using code like below:
  ```
  shared mydll
  mydll = LoadLibrary(output_path + "\\VehicleMonitor.dll")
  ```
  
  ```output_path``` is where the DLL is output to. Replace it with the actual path.

  Use the following line to explicitly **unload** the DLL: 
  ```
  mydll = null
  ```

## How to Debug

* Set breaking points  
* Launch TransModeler
* From Visual Studio IDE, select menu *```Debug->Attach to Process...```*, or *```Ctrl+Alt+P```*. From the list of running processes, choose ```tsm.exe```
* Use *GISDK Immediate Execution Dialog* to load the DLL
* Have fun
