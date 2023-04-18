#!/usr/bin/env python

#
# Caliper Python 3 module
#
# Use it to access TransCAD, Maptitude or TransModeler from a Python 3 script
# Requires a version of a Caliper product released after July 7, 2020
#
# For usage and license information, see the file license.txt in the Caliper product
# program folder (e.g. c:\\Program Files\\Transcad)
# Copyright (c) 2016-2023 Caliper Corporation, all rights reserved.
#
# Required Python Windows package: win32com
#

# importing core module with complete access to its namespace
from .caliper3 import *

# Example usage:
if __name__ == "__main__":
    print(""" Caliper Package for Python 3 (c) 2016-2023 Caliper Corporation, Newton MA, USA.
              For usage and license information, see the file license.txt in the Caliper product program folder (e.g. c:\\Program Files\\TransCAD)
              Example Usage:
              dk = caliperpy.TransCAD.connect() """)