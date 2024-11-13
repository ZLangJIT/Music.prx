. $DIR/common.sh
dir=$1
shift 1

compile main
compile_s sceDisplay_driver
link_before -lpspdebug
link $dir