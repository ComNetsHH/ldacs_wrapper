rm -rf /usr/local/lib/libintairnet_linklayer_glue.dylib
rm -rf /usr/local/lib/libtuhh_intairnet_rlc.dylib
rm -rf /usr/local/lib/libtuhh_intairnet_mc-sotdma.dylib
rm -rf /usr/local/lib/libtuhh_intairnet_arq.dylib

ln /Users/fu/TUHH/Projects/intairnet-linklayer-glue/cmake-build-debug/libintairnet_linklayer_glue.dylib /usr/local/lib/libintairnet_linklayer_glue.dylib
ln /Users/fu/TUHH/Projects/avionic-rlc/cmake-build-debug/libtuhh_intairnet_rlc.dylib /usr/local/lib/libtuhh_intairnet_rlc.dylib
ln /Users/fu/TUHH/Projects/mc-sotdma/cmake-build-debug/libtuhh_intairnet_mc-sotdma.dylib /usr/local/lib/libtuhh_intairnet_mc-sotdma.dylib
ln /Users/fu/TUHH/Projects/avionic-arq/dev/cmake-build-debug/libtuhh_intairnet_arq.dylib /usr/local/lib/libtuhh_intairnet_arq.dylib

#
#ln -s ../mc-sotdma/cmake-build-debug ./mc-sotdma
#ln -s ../mc-sotdma ./mc-sotdma-headers

#
#
#
#
#
#
