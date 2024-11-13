. $DIR/common.sh
dir=$1
shift 1

compile main
compile_S sceDisplay_driver
link_before -lpspdebug
link $dir