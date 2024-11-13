export DIR=$(pwd)
export PSPSDK=$(psp-config --pspsdk-path)

. $DIR/common.sh

cd $DIR

mkdir seplugins
mkdir seplugins/music2
mkdir seplugins/music2/plugins

emit_info_k m2prx m2prx
compile_c plugin_main
link_k music2
cd $DIR
mv -v music2.prx seplugins/music2/music2.prx

compile plugin

build_plugin display display
zip -r seplugins seplugins
