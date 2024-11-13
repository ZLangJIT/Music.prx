. $DIR/common.sh
dir=$1
shift 1

compile main
link_after -lpspdisplay -lpspdebug
link_u $dir