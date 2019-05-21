#!/bin/sh
export VOLK_GENERIC=1
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/tanvir/workarea-seniordesign/senior-design-sdr/gr-IEEE_802_15_4_CustomBlocks/lib
export PATH=/home/tanvir/workarea-seniordesign/senior-design-sdr/gr-IEEE_802_15_4_CustomBlocks/build/lib:$PATH
export LD_LIBRARY_PATH=/home/tanvir/workarea-seniordesign/senior-design-sdr/gr-IEEE_802_15_4_CustomBlocks/build/lib:$LD_LIBRARY_PATH
export PYTHONPATH=$PYTHONPATH
test-IEEE_802_15_4_CustomBlocks 
