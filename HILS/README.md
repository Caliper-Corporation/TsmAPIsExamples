# TransModeller Hardware-in-the-Loop Controller Interface

TransModeler is a powerful traffic simulator with great modelling realism of signalized intersections. This hardware-in-the-loop simulation (HILS) C++ project, illustrates how to connect TransModeler with any NEMA-TS2 controllers using direct Port 1 SDLC communications. No proprietary NTCIP objects are required.

## About HILS

Conventionally, HILS requires costly CID hardware or using relevant proprietary NTCIP objects. NTCIP 1202v3-2019 now provides standardized object towards that end now, though, in some cases, the NTCIP detour is not really desired.

There are a lot of use cases for HILS using native SDLC communications. These use cases include controller firmware bench testing, controller database verification and seamless migration, SDLC-level cyber-security investigation, SDLC frame timing optimization, and connected vehicle applications that need a closest replica of the field conditions including the latency of serial communications.

You can even utilize the direct control over SDLC frames for some interesting/creative work related to ATSPM, for example, ATSPM-in-the-loop which hooks up ATSPM with a traffic simulator in real-time.

## Benefits of HILS

The TransModeler HILS Plugin offers several benefits for traffic management professionals:

* **Realism** - The plugin enables the use of actual controller settings, bringing real-world functionality to the simulation environment. 

* **Flexibility** - Users can test various signal operational strategies, actuated signal parameters, and signal timing plans in a controlled environment without affecting real-world traffic operations. 

* **Safety** - Testing and validating new controller firmware in a simulation environment allows for the identification of potential issues without putting road users at risk. 

* **Seamless Integration** - By allowing for the transfer of tested controller databases from the simulation environment to the field, the plugin minimizes errors and streamlines the implementation process.

## How to build

TransModeler is a Windows-based software and its Application Programming Interface (API), TsmApi, is a high-performance in-process COM API. Latest MSVC compiler that supports C++ 20 standard is required. 

The project is configured using CMake. You can directly do a CMake build.  Alternatively, you can load the CMake project in Visual Studio, and build from there.

The generated tsmci.dll should be put together with an accompanying tsmci.config.xml file.

## How to configure

- The simulation project must have a simulation step of 0.1 sec.  While TransModeller supports simulation step smaller than 0.1 seconds, such as 0.05, or 0.01, a step-size of 0.1 second is the value to go with hardware-in-loop simulation.

- The desired real-time factor must be set to 1.0. This is the best way to have a synchronized clock time across TransModeler simulator and the connected controller hardware.

- The tsmci.config.xml file is where to specify the mappings of _TransModeler's signals_ and _turning movements to loadswitch channels and detector channels_ in the field. Check out the [sample configuration file](tsm/sample/tsmci.config.xml) here for more details.

## Dependencies

This TransModeler HILS Controller Interface depends on the `Virtual Traffic Cabinet` (VTC) framework, an open-source C++ library. More information about VTC can be found at [vtc](https://github.com/wxinix/vtc). 