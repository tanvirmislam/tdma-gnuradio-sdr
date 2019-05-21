#!/bin/sh
export VOLK_GENERIC=1
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/tanvir/workarea-seniordesign/senior-design-sdr/gr-IEEE_802_15_4_CustomBlocks/python
export PATH=/home/tanvir/workarea-seniordesign/senior-design-sdr/gr-IEEE_802_15_4_CustomBlocks/build/python:$PATH
export LD_LIBRARY_PATH=/home/tanvir/workarea-seniordesign/senior-design-sdr/gr-IEEE_802_15_4_CustomBlocks/build/lib:$LD_LIBRARY_PATH
export PYTHONPATH=/home/tanvir/workarea-seniordesign/senior-design-sdr/gr-IEEE_802_15_4_CustomBlocks/build/swig:$PYTHONPATH
/usr/bin/python2 /home/tanvir/workarea-seniordesign/senior-design-sdr/gr-IEEE_802_15_4_CustomBlocks/python/qa_Coordinator.py 
