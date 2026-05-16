#!/bin/sh
# SPDX-License-Identifier: BSD-3-Clause
# Copyright Contributors to the OpenColorIO Project.

# For OS X
export DYLD_LIBRARY_PATH="/home/david/Documents/projects/software/MOKM Color Processor/third_parties/install/lib:${DYLD_LIBRARY_PATH}"

# For Linux
export LD_LIBRARY_PATH="/home/david/Documents/projects/software/MOKM Color Processor/third_parties/install/lib:${LD_LIBRARY_PATH}"

export PATH="/home/david/Documents/projects/software/MOKM Color Processor/third_parties/install/bin:${PATH}"
export PYTHONPATH="/home/david/Documents/projects/software/MOKM Color Processor/third_parties/install/:${PYTHONPATH}"
