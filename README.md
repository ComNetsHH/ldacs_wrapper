# intarnet-omnet-wrapper


## Add libs 
The project needs access to three repositories. Clone those somewhere, in this example one folder above, and add symbolic links with the following names (use the exact same names please, because some `.cc` files will include stuff from there).

ln -s ../intairnet-linklayer-glue/cmake-build-debug ./glue-lib
ln -s ../intairnet-linklayer-glue ./glue-lib-headers

ln -s ../avionic-rlc/cmake-build-debug ./avionic-rlc
ln -s ../avionic-rlc ./avionic-rlc-headers

ln -s ../mc-sotdma/cmake-build-debug ./mc-sotdma
ln -s ../mc-sotdma ./mc-sotdma-headers

## Set project to compile using `makemake`
- Rightlick project -> OMNeT++ -> Makemake 
- Set `Build` to `Makemake`
- enter its options
  - Under `Compile` add the three library header locations
    - `../glue-lib-headers` `../avionic-rlc-headers` `../mc-sotdma-headers`    

## Install shared libraries
ln /Users/fu/TUHH/Projects/intairnet-linklayer-glue/cmake-build-debug/libintairnet_linklayer_glue.dylib /usr/local/lib/libintairnet_linklayer_glue.dylib


ln /Users/fu/TUHH/Projects/avionic-rlc/cmake-build-debug/libtuhh_intairnet_rlc.dylib /usr/local/lib/libtuhh_intairnet_rlc.dylib
