# Vehicle Monitor Demo

## Introduction

Vehicle Monitor API is a set of high-performance C++ API that allows developing TransModeller plugin with a direct connection to TransModeler's internal vehicle movement module.

Vehicle Monitor API can be used to integrate user-defined driver-behavior models (CF or LC), Connected and Autonomous Vehicle (CAV) algorithms, Emission Models, Peer to Peer (P2P) Vehicle Communications, VANET Communications, Safety-Surrgate Analysis, and more.  Imagination and creativity are the boundary.

## Advantage

Vehicle Monitor API has certain overlapping use case with TransModeler COM-based API, i.e., IVehicleEvent and ITsmVehicle COM interface.  While COM interface is more flexible in accomodating different programming languages,  Vehicle Monitor API provides the best performance endowed by native C++, and an efficient call-back mechnism which covers more vehicle-related events than COM interface. Vehicle Monitor API takes full advantage of TransModeller multi-threaded parallel compuation.

## Usage

First make sure ```VehicleMonitor.props``` file references to the correct TransModeller installation folder.  Update the XML accordingly:
```
  <PropertyGroup Label="UserMacros">
    <TSM_FOLDER>C:\Program Files\TransModeler 6.1</TSM_FOLDER>
  </PropertyGroup>
```

User-logic code should be implemented at the various overridden virtual methods of ```MyVehicle``` class.  Take a look at ```vm_plugin.hpp``` and those source code comments for better idea.


The generated VehicleMonitor.dll can be **loaded** using Caliper Script (GISDK) *Immediate Execution Dialog*, using code like below:

```
shared mydll
mydll = LoadLibrary(output_path\\VehicleMonitor.dll")
```
Use the following line to explicitly **unload** the DLL: 
```
mydll = null
```