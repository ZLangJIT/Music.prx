. $DIR/common.sh
dir=$1
shift 1

compile main
compile blit
compile font
compile_S sceDisplay_driver
link $dir