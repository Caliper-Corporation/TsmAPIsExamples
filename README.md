# TransModeler API Extensions Demo
This repo provides various open-source demo projects,  illustrating the usage of TransModeler Application Programming Interfaces (APIs).

# Introduction
TransModeler features a comprehensive set of flexible and powerful Application Programming Interfaces (APIs) for advanced users to develop *plugins* that extend TransModeler user interface, customize the underlying models, integrate new signal control and congestional mitigation algorithms, and incorporate state-of-the-art ITS and Connected and Autonomous Vehicle (CAV) technologies.  

For ITS system integrators, hardware system manufactures, or auto makers, the APIs also allow TransModeler to be used as a powerful high-performance real-time simulation engine for Artificial Intelligence (AI) applications,  hardware-in-the-loop and system-in-the-loop simulations, and Decision Support Systems (DSS),  in distributed, cloud-based, or edge computing environments.

TransModeler APIs support the following programming languages:

- **GISDK** is the native scripting language supported by TransModeler. Versatile and flexible, it is a domain specific language (DSL) to faciliate the everyday activities of transportation and traffic modelling. Its flexiblity and power are similar to Javascript, yet much more powerful and focused when it comes to GIS, transportation modeling and traffic simulation tasks.
- **Any COM-compliant Languages** TransModeler APIs featurs a performant in-process COM interface that allows using any COM-compliant language to develop TransModeler plugins. This includes, .NET (C#, F#, C++/CLI), C++, Delphi among others.  C++ would provide the best performance while .NET the most convenient choice to start with.

# Supported Platform
Windows Only, or WINE on Linux.

# License
BSD-3-Clause License

# How to Build
Projects written in C++ or .NET (C# or F#) can be opened by Visual Studio 2022 IDE and built from there.  Projects written in GISDK script need to be compiled by the GISDK compiler shipped with TransModeler. 

Please consult the README of each respective project.
