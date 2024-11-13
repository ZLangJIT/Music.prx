export DIR=$(pwd)
export PSPSDK=$(psp-config --pspsdk-path)

. $DIR/common.sh

compile plugin

mkdir seplugins
mkdir seplugins/music2
mkdir seplugins/music2/plugins

build_plugin display

cd $DIR

emit_info music2_prx music2_prx

compile main
link music2

cd $DIR
mv -v music2.prx seplugins/music2/music2.prx

zip -r seplugins seplugins
