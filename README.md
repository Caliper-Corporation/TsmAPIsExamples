# TransModeler APIs Examples
This repo provides various example projects,  illustrating the usage of TransModeler Application Programming Interfaces (APIs).

# Introduction
TransModeler offers a comprehensive set of flexible and powerful APIs that enable users to:
- Extend the user interface, 
- Customize models, 
- Integrate new signal optimization algorithms and congestion mitigation strategies, and 
- Incorporate cutting-edge ITS and CAV technologies. 

For ITS system integrators, hardware system manufacturers, and automakers, the APIs allow TransModeler to serve as:
- A powerful real-time simulation engine for AI applications, 
- Hardware-in-the-loop (HILS) and system-in-the-loop  (SILS) simulations, and 
- Decision Support Systems (DSS), in distributed, cloud-based, or edge computing environments.

The TransModeler APIs support several programming languages, including:
- Caliper Script (a.k.a. GISDK), a proprietary scripting language designed specifically for transportation and traffic modeling tasks, and 
- Any COM-compliant languages, such as C++, .NET (e.g., C#, F# or C++/CLI), Delphi, and Python. 

Using Python would be particularly useful if direct access to existing numerical, machine learning, artificial intelligence, or reinforcement learning libraries is needed, such as numpy, Pandas, FastAI, TensorFlow, or OpenAI API. 

C++ provides the best performance, while .NET is the most convenient.

# Deployment
Developers can deploy applications created with TransModeler APIs as in-process plugins, which share the same process space as TransModeler, or as a stand-alone application in its own process space. This provides great flexibility to meet the needs of different development contexts.

# Supported Platform
Windows only, or WINE on Linux.

# License
BSD-3-Clause License.

# How to Build

- Visual Studio 2022 IDE can open projects written in native C++ or managed .NET languages (C#, F#, C++/CLI), which can then be built from within the IDE. 

- Projects written in Caliper Script (i.e., GISDK) require compilation by the Caliper Script compiler included with TransModeler. 

- Python samples requires [pywin32](https://pypi.org/project/pywin32/) version 302 or later.

It's recommended to refer to the README file of each project for specific instructions.

# List of Examples

Additional examples will be added to TransModeler's library. The current examples include:

- [Vehicle Monitor](https://github.com/Caliper-Corporation/TsmAPIsExamples/tree/main/VehicleMonitor), which serves as a scaffold example of a Vehicle Monitor plugin written in modern C++/20.
- [Hardware-in-the-Loop](https://github.com/Caliper-Corporation/TsmAPIsExamples/tree/main/HILS), which demonstrates how to connect TransModeler with a NEMA-TS2 traffic signal controller via direct SDLC communication using C++.
- [Python](https://github.com/Caliper-Corporation/TsmAPIsExamples/tree/main/Python), which provides various Python-based samples for TsmApi applications.
