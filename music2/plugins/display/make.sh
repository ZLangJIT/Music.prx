. $DIR/common.sh
dir=$1
shift 1

compile main
link_before -lpspdisplay_driver -lpspdisplay -lpspdebug
link $dir