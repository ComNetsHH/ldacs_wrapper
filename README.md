# intarnet-omnet-wrapper


## Add libs 
ln -s ../intairnet-linklayer-glue/cmake-build-debug ./glue-lib
ln -s ../intairnet-linklayer-glue ./glue-lib-headers

ln -s ../avionic-rlc/cmake-build-debug ./avionic-rlc
ln -s ../avionic-rlc ./avionic-rlc-headers



## Install shared libraries
ln /Users/fu/TUHH/Projects/intairnet-linklayer-glue/cmake-build-debug/libintairnet_linklayer_glue.dylib /usr/local/lib/libintairnet_linklayer_glue.dylib


ln /Users/fu/TUHH/Projects/avionic-rlc/cmake-build-debug/libtuhh_intairnet_rlc.dylib /usr/local/lib/libtuhh_intairnet_rlc.dylib