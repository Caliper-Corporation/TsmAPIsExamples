# TransModeller Hardware-in-the-Loop Controller Interface

TransModeler is a powerful traffic simulator with great modelling realism of signalized intersections. This hardware-in-the-loop simulation (HILS) C++ project, illustrates how to connect TransModeler with any NEMA-TS2 controllers using direct Port 1 SDLC communications. No CID is needed, and no proprietary NTCIP objects are required.

## About HILS

Conventionally, HILS requires costly CID hardware or using relevant proprietary NTCIP objects. NTCIP 1202v3-2019 now provides standardized object towards that end now, though, in some cases, the NTCIP detour is not really desired.

There are a lot of use cases for HILS using native SDLC communications. These use cases include controller firmware bench testing, controller database verification and seamless migration, SDLC-level cyber-security investigation, SDLC frame timing optimization, and connected vehicle applications that need a closest replica of the field conditions including the latency of serial communications.

You can even utilize the direct control over SDLC frames for some interesting/creative work related to ATSPM, for example, ATSPM-in-the-loop which hooks up ATSPM with a traffic simulator in real-time.

## How to build

TransModeler is a Windows-based software and its Application Programming Interface (API), TsmApi, is a high-performance in-process COM API. Latest MSVC compiler that supports C++ 20 standard is required. 

The project is configured using CMake. You can directly do a CMake build.  Alternatively, you can load the CMake project in Visual Studio, and build from there.

The generated tsmci.dll should be put together with an accompanying tsmci.config.xml file.

## How to configure

- The simulation project must have a simulation step of 0.1 sec.  While TransModeller supports simulation step smaller than 0.1 seconds, such as 0.05, or 0.01, a step-size of 0.1 second is the value to go with hardware-in-loop simulation.

- The desired real-time factor must be set to 1.0. This is the best way to have a synchronized clock time across TransModeler simulator and the connected controller hardware.

- The tsmci.config.xml file is where to specify the mappings of _TransModeler's signals_ and _turning movements to loadswitch channels and detector channels_ in the field. Check out the [sample configuration file](tsm/sample/tsmci.config.xml) here for more details.


# Virtual Traffic Cabinet
Virtual Traffic Cabinet (VTC) is a modern C++ library (C++20) for the virtualization of traffic cabinet in adherence with NEMA-TS2 and ATC Standards.

## Compiler
Requires a C++ compiler that supports C++20. Visual Studio or CLion IDE with CMake build system.

## Example

### First Example
The following code illustrates a use case that an incoming SDLC Type 3 command frame gets processed and a corresponding Type 131 response frame generated based on the internal MMU channel compatibility status data.

The incoming data - typically read from the serial bus by an SDLC serial adapter, is represented as a ```std::array<Byte>```. 

Type 3 command frame is about requesting the MMU for programmed channel compatibility. 
```cpp
// l_data_in read from serial bus by the serial adapter
std::array<Byte, 3> l_data_in = {0x10, 0x83, 0x03};
```
Upon receipt, Type 3 command frame is dispatched by calling ```serial::Dispatch(l_data_in)```:

```cpp
// MMU channel compatibility status data converted to Type 131 response frame.
auto result = serial::Dispatch(l_data_in);
```
The returned ```result``` is a tuple of ```<bool, std::span<Byte>>```.  The bool value indicates ```success``` or ```failure``` of the operation. The span is the generated response frame, which can be transmitted back to the Controller Unit via the serial bus, by the SDLC serial adapter interface (which is omitted here).
```cpp
// Check the frame size is correct
assert(std::get<1>(result).size() == serial::FrameType<131>::type::bytesize);

// Check Byte 3 for Channel 1 and Channel 2 compatibility
assert(std::get<1>(result)[3] == 0x01); // Byte 3 represents CH1-CH2 compatibility
```

### Another Example

The following code illustrates how to declare an I/O frame type.  It is self documenting, mapping exactly to the relevant Standard specifications.

```cpp
// ----------------------------------------------
// Frame Type 13
// ----------------------------------------------
using TfBiu04_OutputsInputsRequestFrame
= Frame<
    0x03, // TF BIU#1 Address = 3
    0x0D, // FrameID = 13
    8,    // 8 Bytes
    SSR_CommandFrameType,
    // ----------------------------------------------
    // Byte 3
    // ----------------------------------------------
    FrameBit<io::output::PhaseOn<1>, 0x18>,
    FrameBit<io::output::PhaseOn<2>, 0x19>,
    FrameBit<io::output::PhaseOn<3>, 0x1A>,
    FrameBit<io::output::PhaseOn<4>, 0x1B>,
    FrameBit<io::output::PhaseOn<5>, 0x1C>,
    FrameBit<io::output::PhaseOn<6>, 0x1D>,
    FrameBit<io::output::PhaseOn<7>, 0x1E>,
    FrameBit<io::output::PhaseOn<8>, 0x1F>,
    // ----------------------------------------------
    // Byte 4
    // ----------------------------------------------
    FrameBit<io::output::PhaseNext<1>, 0x20>,
    FrameBit<io::output::PhaseNext<2>, 0x21>,
    FrameBit<io::output::PhaseNext<3>, 0x22>,
    FrameBit<io::output::PhaseNext<4>, 0x23>,
    FrameBit<io::output::PhaseNext<5>, 0x24>,
    FrameBit<io::output::PhaseNext<6>, 0x25>,
    FrameBit<io::output::PhaseNext<7>, 0x26>,
    // 0x27 - Reserved.
    // ----------------------------------------------
    // Byte 5
    // ----------------------------------------------
    FrameBit<io::output::PhaseNext<8>, 0x28>,
    FrameBit<io::output::PhaseCheck<1>, 0x29>,
    FrameBit<io::output::PhaseCheck<2>, 0x2A>,
    FrameBit<io::output::PhaseCheck<3>, 0x2B>,
    FrameBit<io::output::PhaseCheck<4>, 0x2C>,
    FrameBit<io::output::PhaseCheck<5>, 0x2D>,
    FrameBit<io::output::PhaseCheck<6>, 0x2E>,
    FrameBit<io::output::PhaseCheck<7>, 0x2F>,
    // ----------------------------------------------
    // Byte 6
    // ----------------------------------------------
    FrameBit<io::output::PhaseCheck<8>, 0x30>
    // 0x31 - Designated Input
    // 0x32 - Designated Input
    // 0x33 - Designated Input
    // 0x34 - Designated Input
    // 0x35 - Designated Input
    // 0x36 - Spare
    // 0x37 - Spare
    // ----------------------------------------------
    // Byte 7
    // ----------------------------------------------
    // 0x38 - Spare
    // 0x39 - Spare
    // 0x3A - Spare
    // 0x3B - Designated Input
    // 0x3C - Designated Input
    // 0x3D - Designated Input
    // 0x3E - Designated Input
    // 0x3F - Designated Input
>;

template<>
struct FrameType<13>
{
  using type = TfBiu04_OutputsInputsRequestFrame;
};

```

### Third Example

This following example illustrates how to set MMU compatibility matrix for a typical NEMA 8 Phase set-up:
```cpp
  using namespace mmu;

  mmu::variable<ChannelCompatibilityStatus<1, 5>>.value = Bit::On;
  mmu::variable<ChannelCompatibilityStatus<1, 6>>.value = Bit::On;
  mmu::variable<ChannelCompatibilityStatus<2, 5>>.value = Bit::On;
  mmu::variable<ChannelCompatibilityStatus<2, 6>>.value = Bit::On;
  mmu::variable<ChannelCompatibilityStatus<3, 7>>.value = Bit::On;
  mmu::variable<ChannelCompatibilityStatus<3, 8>>.value = Bit::On;
  mmu::variable<ChannelCompatibilityStatus<4, 7>>.value = Bit::On;
  mmu::variable<ChannelCompatibilityStatus<4, 8>>.value = Bit::On;
```

### Further Usage

Examine the [test cases](tests/vtc_tests.cpp) to learn how to use this library.
