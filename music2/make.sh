export DIR=$(pwd)
export PSPSDK=$(psp-config --pspsdk-path)

. $DIR/common.sh

cd $DIR

mkdir -p seplugins/music2/plugins

build_plugin main main
build_plugin display display
zip -r seplugins seplugins
