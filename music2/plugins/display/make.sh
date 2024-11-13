. $DIR/common.sh
dir=$1
shift 1

compile main
link_before -lpspdebug -lpspdisplay_driver -lpspdisplay
link $dir