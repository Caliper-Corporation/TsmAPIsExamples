# TransModeler APIs Demo
This repo provides various open-source demo projects,  illustrating the usage of TransModeler Application Programming Interfaces (APIs).

# Introduction
TransModeler features a comprehensive set of flexible and powerful Application Programming Interfaces (APIs). The APIs can be used to develop *plugins* and *extensions* that extend TransModeler user interface, customize the underlying models, integrate new signal optimization algorithms and congestion mitigation strategies, and incorporate state-of-the-art ITS and Connected and Autonomous Vehicle (CAV) technologies.  

For ITS system integrators, hardware system manufactures, or auto makers, the APIs also allow TransModeler to be used as a powerful high-performance real-time simulation engine for Artificial Intelligence (AI) applications,  hardware-in-the-loop and system-in-the-loop simulations, and Decision Support Systems (DSS),  in distributed, cloud-based, or edge computing environments.

TransModeler APIs support the following programming languages:

- **Caliper Script** is Caliper-proprietary scripting language. Versatile and flexible, it is a domain specific language (DSL) designed to faciliate everyday activities of transportation and traffic modelling. Its many modern language features of a dynamic language are similar to Javascript, yet much more powerful and specialized when it comes to GIS, high-performance matrix computations, transportation modeling and traffic simulation tasks.
- **Any COM-compliant Languages** TransModeler APIs features both performant **in-process** and flexible **out-process** COM interfaces that allow using any COM-compliant language to develop TransModeler plugins/extensions/custom applications. This includes .NET (C#, F#, C++/CLI), C++, Delphi among others.  C++ would provide the best performance while .NET the most convenient choice.

# Deployment
Applications developed using TransModeler APIs can be deployed as **in-process plugins** that share the same process space as TransModeler, or as a **stand-alone application** in its own process space (i.e., out-of-process client with TransModeler as the COM automation server). This renders great flexiblity to meet the needs of different development context.

# Supported Platform
Windows only, or WINE on Linux.

# License
BSD-3-Clause License

# How to Build
Projects written in native C++ or managed .NET (C#, F#, C++/CLI) languages can be opened by Visual Studio 2022 IDE and built from there.  Projects written in Caliper Script need to be compiled by the Caliper Script compiler shipped with TransModeler. 

Please consult the README of each respective project.
