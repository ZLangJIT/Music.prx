export DIR=$(pwd)
export PSPSDK=$(psp-config --pspsdk-path)

. $DIR/common.sh

mkdir seplugins
mkdir seplugins/music2
mkdir seplugins/music2/plugins

build_plugin display

cd $DIR

echo "PSP_MODULE_INFO(Music2_prx, 0x1000, 1, 1);" > $DIR/music2_plugin_generated.h

compile main
link music2

cd $DIR
mv -v music2.prx seplugins/music2/music2.prx

zip seplugins
